#include "../include/server_logger.h"

#define MESSAGE_SIZE 100

#ifdef __APPLE__
#include <mach/mach.h>
#endif

#ifdef _WIN32

std::map<std::string, std::pair<HANDLE, int>> server_logger::_queues_users = std::map<std::string, std::pair<HANDLE, int>>();

#elif __linux__

std::map<std::string, std::pair<mqd_t, int>> server_logger::_queues_users = std::map<std::string, std::pair<mqd_t, int>>();

#elif __APPLE__

std::map<std::string, std::pair<mach_port_t, int>> server_logger::_queues_users = std::map<std::string, std::pair<mach_port_t, int>>();

#endif

server_logger::server_logger(std::map<std::string, std::set<logger::severity>> const logs)
{
    std::runtime_error opening_queue_error("Error opening queue");

    #ifdef _WIN32
        
        for (auto &[file, severities] : logs)
        {
            if (_queues_users.find(file) == _queues_users.end())
            {
                HANDLE pipe = CreateFileA(file.c_str(), GENERIC_WRITE, 0, nullptr, OPEN_EXISTING, 0, nullptr);
                if (pipe == INVALID_HANDLE_VALUE) throw opening_queue_error;
                _queues_users[file].first = pipe;
            }
            _queues_users[file].second++;
            _queues[file].first = _queues_users[file].first;
            _queues[file].second = severities;
        }

    #elif __linux__

        struct mq_attr queue_attributes; // атрибуты очереди
        queue_attributes.mq_maxmsg = 10; // максимальное количество сообщений в очереди
        queue_attributes.mq_msgsize = MESSAGE_SIZE; // максимальная размер сообщения

        for (auto &[file, severities] : logs)
        {
            if (_queues_users.find(file) == _queues_users.end())
            {
                mqd_t descriptor = mq_open(file.c_str(), O_WRONLY, 0644, &queue_attributes);
                if (descriptor < 0) throw opening_queue_error;
                _queues_users[file].first = descriptor;
            }
            _queues_users[file].second++;
            _queues[file].first = _queues_users[file].first;
            _queues[file].second = severities;
        }
        
    #elif __APPLE__

        for (auto &[file, severities] : logs)
        {
            if (_queues_users.find(file) == _queues_users.end())
            {
                mach_port_t port;
                kern_return_t result = mach_port_allocate(mach_task_self(), MACH_PORT_RIGHT_RECEIVE, &port);
                if (result != KERN_SUCCESS) throw opening_queue_error;
                _queues_users[file].first = port;
            }
            _queues_users[file].second++;
            _queues[file].first = _queues_users[file].first;
            _queues[file].second = severities;
        }

    #endif

    _request = 0;
}


server_logger::server_logger(server_logger const &other) :
    _queues(other._queues)
{
    for (auto &[key, pair] : _queues_users) pair.second++;
}

server_logger &server_logger::operator=(server_logger const &other)
{
    if (this == &other) return *this;
    close_streams();
    _queues = other._queues;
    for (auto &[key, pair] : _queues) _queues_users[key].second++;
    return *this;
}

server_logger::server_logger(server_logger &&other) noexcept :
    _queues(std::move(other._queues)) {}

server_logger &server_logger::operator=(server_logger &&other) noexcept
{
    if (this == &other) return *this;
    close_streams();
    _queues = std::move(other._queues);
    return *this;
}

void server_logger::close_streams()
{
    for (auto & [file, pair] : _queues)
    {
        if (--_queues_users[file].second != 0) continue;
        #ifdef _WIN32
            ClodeHandle(_queues_users[file].first);
        #elif __linux__
            mq_close(_queues_users[file].first);
        #elif __APPLE__
            mach_port_destroy(mach_task_self(), _queues_users[file].first);
        #endif
        _queues_users.erase(file);

    }
}

server_logger::~server_logger() noexcept
{
    close_streams();
}


logger const *server_logger::log(const std::string &text, logger::severity severity) const noexcept
{
    size_t meta_size = sizeof(size_t) + sizeof(size_t) + sizeof(pid_t) + sizeof(const char *) + sizeof(bool);
    size_t message_size = MESSAGE_SIZE - meta_size;
    size_t packets_count = text.size() / message_size + 1;

    char info_message[meta_size];
    char *ptr;

    ptr = info_message;
    *reinterpret_cast<bool*>(ptr) = false; // если ложь, то идет инфо сообщение
    ptr += sizeof(bool);
    *reinterpret_cast<size_t*>(ptr) = packets_count;
    ptr += sizeof(size_t);
    *reinterpret_cast<size_t*>(ptr) = _request;
    ptr += sizeof(size_t);
    *reinterpret_cast<pid_t*>(ptr) = _process_id;
    ptr += sizeof(pid_t);
    char const * severity_string = severity_to_string(severity).c_str();
    strcpy(ptr, severity_string);

    char message[MESSAGE_SIZE];

    for (auto & [file, pair] : _queues)
    {
        if (pair.second.find(severity) == pair.second.end()) continue;

        // Отправка информационного сообщения
        #ifdef _WIN32
            DWORD bytes_written;
            WriteFile(pair.first, info_message, MESSAGE_SIZE, &bytes_written, nullptr);
        #elif __linux__
            mq_send(pair.first, info_message, MESSAGE_SIZE, 0); // отправка инфо сообщения
        #elif __APPLE__
            mach_msg_header_t msg_header;
            msg_header.msgh_bits = MACH_MSGH_BITS_SET(MACH_MSG_TYPE_COPY_SEND, 0, 0, 0);
            msg_header.msgh_size = sizeof(info_message);
            msg_header.msgh_remote_port = pair.first;
            msg_header.msgh_local_port = MACH_PORT_NULL;
            msg_header.msgh_id = 0;

            mach_msg(&msg_header, MACH_SEND_MSG, sizeof(info_message), 0, MACH_PORT_NULL, MACH_MSG_TIMEOUT_NONE, MACH_PORT_NULL);
        #endif

        ptr = message;
        *reinterpret_cast<bool*>(ptr) = true;
        ptr += sizeof(bool);
        *reinterpret_cast<size_t*>(ptr) = _request;
        ptr += sizeof(size_t);
        *reinterpret_cast<pid_t*>(ptr) = _process_id;
        ptr += sizeof(pid_t);
        for (size_t i = 0; i < packets_count; ++i)
        {
            size_t pos = i * message_size;
            size_t rest = text.size() - pos;
            size_t substr_size = (rest < message_size) ? rest : message_size;
            memcpy(ptr, text.substr(pos, substr_size).c_str(), substr_size);
            *(ptr + substr_size) = 0;
            // Отправка сообщения с данными
            #ifdef _WIN32
                WriteFile(pair.first, message, MESSAGE_SIZE, &bytes_written, nullptr);
            #elif __linux__
                mq_send(pair.first, message, MESSAGE_SIZE, 0);
            #elif __APPLE__
                mach_msg(&msg_header, MACH_SEND_MSG, sizeof(message), 0, MACH_PORT_NULL, MACH_MSG_TIMEOUT_NONE, MACH_PORT_NULL);
            #endif
        }
    }
    _request++;
    return this;
}
