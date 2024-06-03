#ifndef MATH_PRACTICE_AND_OPERATING_SYSTEMS_SERVER_LOGGER_H
#define MATH_PRACTICE_AND_OPERATING_SYSTEMS_SERVER_LOGGER_H

#include "../../logger/include/logger.h"
#include "server_logger_builder.h"

#include <cstring>
#include <set>

class server_logger final:
    public logger
{
    
    friend class server_logger_builder;

private:

    pid_t _process_id;

    size_t mutable _request;

    #ifdef _WIN32
        std::map<std::string, std::pair<HANDLE, std::set<logger::severity>>> _queues; // name, (queue id, severities)

        static std::map<std::string, std::pair<HANDLE, int>> _queues_users; // name, (queue id, number of users)
    #elif __linux__

        std::map<std::string, std::pair<mqd_t, std::set<logger::severity>>> _queues; // name, (queue id, severities)

        static std::map<std::string, std::pair<mqd_t, int>> _queues_users; // name, (queue id, number of users)
    #elif __APPLE__

        std::map<std::string, std::pair<mach_port_t, std::set<logger::severity>>> _queues; // name, (queue id, severities)

        static std::map<std::string, std::pair<mach_port_t, int>> _queues_users; // name, (queue id, number of users)

    #endif

    server_logger(std::map<std::string, std::set<logger::severity>> const logs);

    void close_streams();

public:

    server_logger(server_logger const &other);

    server_logger &operator=(server_logger const &other);

    server_logger(server_logger &&other) noexcept;

    server_logger &operator=(server_logger &&other) noexcept;

    ~server_logger() noexcept final;

public:

    [[nodiscard]] logger const *log(const std::string &message, logger::severity severity) const noexcept override;

};

#endif //MATH_PRACTICE_AND_OPERATING_SYSTEMS_SERVER_LOGGER_H