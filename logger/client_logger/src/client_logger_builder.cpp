#include "../include/client_logger_builder.h"

client_logger_builder::client_logger_builder() : _format("[%s] %m\n") {}

client_logger_builder::client_logger_builder(client_logger_builder const &other) :
    _format(other._format), _streams(other._streams) {}

client_logger_builder &client_logger_builder::operator=(client_logger_builder const &other)
{
    if (this == &other) return *this;
    _streams = other._streams;
    _format = other._format;
    return *this;
}

client_logger_builder::client_logger_builder(client_logger_builder &&other) noexcept :
    _streams(std::move(other._streams)), _format(std::move(other._format)) {}

client_logger_builder &client_logger_builder::operator=(client_logger_builder &&other) noexcept
{
    if (this == &other) return *this;
    _format = std::move(other._format);
    _streams = std::move(other._streams);
    return *this;
}

client_logger_builder::~client_logger_builder() noexcept {}

logger_builder *client_logger_builder::add_file_stream(std::string const &stream_file_path, logger::severity severity)
{
    _streams[stream_file_path].insert(severity);
    return this;
}

logger_builder *client_logger_builder::add_console_stream(logger::severity severity)
{
    add_file_stream(CONSOLE, severity);
    return this;
}

/*
формат файла:
{
    "files" :
    [
        [
            "file1",
            [
                "WARNING",
                "DEBUG"
            ]
        ],
        
        [
            "file2",
            [
                "WARNING",
                "ERROR"
            ]
        ]
    ]   
}
*/
logger_builder* client_logger_builder::transform_with_configuration(std::string const &configuration_file_path, std::string const &configuration_path)
{
    std::runtime_error nonexistent_file("Configuration file doesn't exist\n");
    std::runtime_error empty_file("Can't find configuration path\n");

    nlohmann::json configuration;
    std::ifstream configuration_file(configuration_file_path, std::ios::binary);
    if (!(configuration_file.is_open())) throw nonexistent_file;

    if (configuration_file.peek() == EOF) throw empty_file;
    configuration_file >> configuration;
    if (configuration.find(configuration_path) == configuration.end()) throw empty_file;

    std::string file_name;
    std::string string_severity;
    logger::severity logger_severity;

    for (auto & file : configuration[configuration_path])
    {
        file_name = file[0];
        for (auto & severity : file[1])
        {
            string_severity = severity;
            logger_severity = string_to_severity(string_severity);
            _streams[file_name].insert(logger_severity);
        }
    }
    return this;
}

logger_builder *client_logger_builder::clear()
{
    for (auto & file : _streams) file.second.clear();
    _streams.clear();
    return this;
}

logger *client_logger_builder::build() const
{
    return new client_logger(_streams, _format);
}

logger_builder * client_logger_builder::set_format(std::string &format)
{
    _format = format;
    return this;
}