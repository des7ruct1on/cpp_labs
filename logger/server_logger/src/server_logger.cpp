#include "../include/server_logger.h"

#ifdef _WIN32
std::map<std::string, std::pair<HANDLE, size_t>> server_logger::queues_users = std::map<std::string, std::pair<HANDLE, size_t>>();
#else
std::map<std::string, std::pair<mqd_t, size_t>> server_logger::queues_users = std::map<std::string, std::pair<mqd_t, size_t>>();
#endif

server_logger::server_logger(server_logger const &other) = default;

server_logger::server_logger(std::map<std::string, std::set<logger::severity>> const keys) {
    for (auto it = keys.begin(); it != keys.end(); ++it) {
        auto &key = it->first;
        auto &set = it->second;
        if (queues_users.find(key) == queues_users.end()) {
#ifdef _WIN32
            HANDLE q = CreateFileA(key.c_str(), GENERIC_WRITE, 0, nullptr, OPEN_EXISTING, 0, nullptr);
            if (q == INVALID_HANDLE_VALUE) {
                throw std::runtime_error("Can`t open queue");
            }

#else
            mqd_t q = mq_open(key.c_str(), O_WRONLY, 0644);
            if (q == (mqd_t) - 1) {
                throw std::runtime_error("Can`t open queue");
            }

#endif
            queues_users[key].first = q;
            queues_users[key].second = 1;
            queues[key].first = q;

        } else {
            queues_users[key].second++;
            queues[key].first = queues_users[key].first;
        }
        queues[key].second = set;
    }
#ifdef _WIN32
    process_id = GetCurrentProcessId();
#else
    process_id = getpid();
#endif
    session_id = 0;
}

server_logger &server_logger::operator=(server_logger const &other) = default;

server_logger::server_logger(server_logger &&other) noexcept = default;

server_logger &server_logger::operator=(server_logger &&other) noexcept = default;

server_logger::~server_logger() noexcept {
    for (auto it = queue.begin(); it != queue.end(); ++it) {
        auto &key = it->first;
        auto &pr = it->second;
        if (--_queues_users[key].second == 0) {
#ifdef _WIN32
            CloseHandle(pair.first);
            queues_users.erase(key);
#else
            mq_close(pair.first);
            queues_users.erase(key);
#endif

        }
    }
}

logger const *server_logger::log(const std::string &text, logger::severity severity) const noexcept {

}