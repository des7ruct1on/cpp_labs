#include "../include/server_logger_builder.h"

logger_builder * server_logger_builder::add_file_stream(std::string const &stream_file_path, logger::severity severity)
{
    std::string file;
    if (stream_file_path[0] != '/') 
    {
        #ifdef __linux__
            file = "/";
        #elif _WIN32
            file = "//./pipe/";
        #elif __APPLE__
            file = "/";
        #endif
        file += stream_file_path;
    }
    else
    {
        #ifdef __linux__
            file = stream_file_path;
        #elif _WIN32
            file = "//./pipe";
            file += stream_file_path;
        #elif __APPLE__
            file = stream_file_path;
        #endif
    }
    _logs[file].insert(severity);
    return this;
}

logger_builder * server_logger_builder::add_console_stream(logger::severity severity)
{
    add_file_stream(CONSOLE, severity);
    return this;
}

logger_builder * server_logger_builder::transform_with_configuration(std::string const &configuration_file_path, std::string const &configuration_path)
{
    std::runtime_error opening_file("Can't open file\n");
    nlohmann::json configuration;
    std::ifstream configuration_file(configuration_file_path, std::ios::binary);
    if (!configuration_file.is_open()) {
        throw opening_file;
    }
    configuration_file >> configuration;

    std::string file_name;
    std::string string_severity;
    logger::severity logger_severity;

    for (auto & file : configuration[configuration_path]) {
        file_name = file[0];
        for (auto & severity : file[1]) {
            string_severity = severity;
            logger_severity = string_to_severity(string_severity);
            _logs[file_name].insert(logger_severity);
        }
    }
}

logger_builder * server_logger_builder::clear()
{
    for (auto &[file, pair] : _logs) pair.clear();
    _logs.clear();
    return this;
}

logger * server_logger_builder::build() const
{
    return new server_logger(_logs);
}