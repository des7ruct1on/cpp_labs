#ifndef MATH_PRACTICE_AND_OPERATING_SYSTEMS_CLIENT_LOGGER_H
#define MATH_PRACTICE_AND_OPERATING_SYSTEMS_CLIENT_LOGGER_H

#include <logger.h>
#include "client_logger_builder.h"
#include <map>
#include <set>
#include <fstream>

class client_logger final: public logger {

    std::map<std::string, std::set<logger::severity>> streams;
    static std::map<std::string, std::pair<std::ofstream, size_t>> user_streams;

    std::string struct_of_log;
    size_t size_struct_of_log;
public:

    client_logger(client_logger const &other);

    client_logger(const std::map<std::string, std::set<logger::severity>> &paths, const std::string &_struct_of_log);

    void end_streams();

    void message_format(std::string &to_format, const std::string &flag, const std::string &replace_with) const noexcept;

    client_logger &operator=(client_logger const &other);

    client_logger(client_logger &&other) noexcept;

    client_logger &operator=(client_logger &&other) noexcept;

    ~client_logger() noexcept final;

    [[nodiscard]] logger const *log(const std::string &message, logger::severity severity) const noexcept override;

};

#endif //MATH_PRACTICE_AND_OPERATING_SYSTEMS_CLIENT_LOGGER_H