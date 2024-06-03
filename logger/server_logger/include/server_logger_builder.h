#ifndef MATH_PRACTICE_AND_OPERATING_SYSTEMS_SERVER_LOGGER_BUILDER_H
#define MATH_PRACTICE_AND_OPERATING_SYSTEMS_SERVER_LOGGER_BUILDER_H

#include <nlohmann/json.hpp>
#include <fstream>

#include <unistd.h>

#include "../../logger/include/logger_builder.h"
#include "server_logger.h"

#ifdef _WIN32
    #define CONSOLE "CON"
    #include <windows.h>
    
#elif __linux__
    #define CONSOLE "/dev/tty"
    #include <mqueue.h>
#elif __APPLE__
    #define CONSOLE "/dev/tty"
#endif

#include <map>
#include <set>
#include <utility>

class server_logger_builder final: public logger_builder
{
    
    std::map<std::string, std::set<logger::severity>> _logs;

public:

    server_logger_builder() = default;

    server_logger_builder(server_logger_builder const &other) = default;

    server_logger_builder &operator=(server_logger_builder const &other) = default;

    server_logger_builder(server_logger_builder &&other) noexcept = default;

    server_logger_builder &operator=(server_logger_builder &&other) noexcept = default;

    ~server_logger_builder() noexcept override = default;

public:

    logger_builder *add_file_stream(std::string const &stream_file_path, logger::severity severity) override;

    logger_builder *add_console_stream(logger::severity severity) override;

    logger_builder* transform_with_configuration(std::string const &configuration_file_path, std::string const &configuration_path) override;

    logger_builder *clear() override;

    [[nodiscard]] logger *build() const override;

};

#endif