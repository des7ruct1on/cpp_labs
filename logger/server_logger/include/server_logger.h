#ifndef MATH_PRACTICE_AND_OPERATING_SYSTEMS_SERVER_LOGGER_H
#define MATH_PRACTICE_AND_OPERATING_SYSTEMS_SERVER_LOGGER_H

#include <cstring>
#include "../../logger/include/logger.h"
#include "server_logger_builder.h"
#define MESSAGE_SIZE 1024
class server_logger final: public logger {

    std::map<std::string, std::set<logger::severity>> keys;

    server_logger(std::map<std::string, const std::set<logger::severity>> keys);

#ifdef _WIN32
    std::map<std::string, std::pair<HANDLE, std::set<logger::severity>>> queues;

    static std::map<std::string, std::pair<HANDLE, size_t>> queues_users;

    DWORD process_id;
#else
    std::map<std::string, std::pair<mqd_t, std::set<logger::severity>>> queues;

    static std::map<std::string, std::pair<mqd_t, size_t>> queues_users;

    pid_t process_id;
#endif

    mutable size_t session_id;


public:

    server_logger(server_logger const &other);

    server_logger &operator=(server_logger const &other);

    server_logger(server_logger &&other) noexcept;

    server_logger &operator=(server_logger &&other) noexcept;

    ~server_logger() noexcept final;

    [[nodiscard]] logger const *log(const std::string &message, logger::severity severity) const noexcept override;

};

#endif