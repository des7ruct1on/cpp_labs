#ifndef MATH_PRACTICE_AND_OPERATING_SYSTEMS_SERVER_LOGGER_BUILDER_H
#define MATH_PRACTICE_AND_OPERATING_SYSTEMS_SERVER_LOGGER_BUILDER_H

#include <logger_builder.h>
#include <cstring>
//#include "../../../common/mini/ini.h"
#include "../../logger/include/logger_builder.h"
#include "server_logger.h"
#include <map>
#include <set>
#include <string>

#ifdef _WIN32
    #define STREAM "CONSOLE"
    #include <conio.h>
    #include <windows.h>
#else
    #define STREAM "/dev/tty"
    #include <unistd.h>
    #include <sys/msg.h>
    #include <sys/fcntl.h>
    #include <sys/stat.h>
    #include <sys/types.h>
#endif



class server_logger_builder final: public logger_builder {
    std::map<std::string, std::set<logger::severity>> keys;
public:

    server_logger_builder();

    server_logger_builder(server_logger_builder const &other);

    server_logger_builder &operator=(server_logger_builder const &other);

    server_logger_builder(server_logger_builder &&other) noexcept;

    server_logger_builder &operator=(server_logger_builder &&other) noexcept;

    ~server_logger_builder() noexcept override;

    logger_builder *add_file_stream(std::string const &stream_file_path, logger::severity severity) override;

    logger_builder *add_console_stream(logger::severity severity) override;

    logger_builder* transform_with_configuration(std::string const &configuration_file_path, std::string const &configuration_path) override;

    logger_builder *clear() override;

    [[nodiscard]] logger *build() const override;

};

#endif