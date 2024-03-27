#include "../include/client_logger.h"

std::map<std::string, std::pair<std::ofstream, size_t>> client_logger::user_streams = std::map<std::string, std::pair<std::ofstream, size_t>>();

client_logger::client_logger(client_logger const &other) 
    : struct_of_log(other.struct_of_log),
    size_struct_of_log(other.size_struct_of_log),
    streams(other.streams) 
{
    for (auto &[path, severities] : streams) {
        user_streams[path].second++;
    }
}

void client_logger::end_streams() {
    for (auto &[path, severities] : streams) {
        if (!--user_streams[path].second) {
            user_streams[path].first.close();
            user_streams.erase(path);
        }
    }
}

client_logger::client_logger(const std::map<std::string, std::set<logger::severity>> &paths, const std::string &_struct_of_log)
: struct_of_log(_struct_of_log), size_struct_of_log(_struct_of_log.size())
{

    for (auto &[path, severities] : paths) {
        if (user_streams.find(path) == user_streams.end()) {
            if (path != "/console") {
                user_streams[path].first.open(path);
                if (!user_streams[path].first.is_open()) {
                    throw std::runtime_error("Unable to open " + path);
                }
            }
            user_streams[path].second = 1;
        } else  {
            user_streams[path].second++;
        }
        streams[path] = severities;
    }
}

client_logger &client_logger::operator=(client_logger const &other) {
    if (this == &other) return *this;
    end_streams();
    streams = other.streams;
    struct_of_log = other.struct_of_log;
    size_struct_of_log = other.size_struct_of_log;

    for (auto &[path, pair] : streams) {
        user_streams[path].second++;
    }
    return *this;
}

client_logger::client_logger(client_logger &&other) noexcept 
    :streams(std::move(other.streams)),
    struct_of_log(std::move(other.struct_of_log)),
    size_struct_of_log(std::move(other.size_struct_of_log)) {}

client_logger &client_logger::operator=(client_logger &&other) noexcept {
    if (this == &other) return *this;

    end_streams();

    streams = std::move(other.streams);
    struct_of_log = std::move(other.struct_of_log);
    size_struct_of_log = std::move(other.size_struct_of_log);

    return *this;
}

client_logger::~client_logger() noexcept {
    end_streams();
}

void client_logger::message_format(std::string &to_format, std::string const &flag, std::string const &replace) const noexcept {
    for (auto pos = 0; pos != std::string::npos;){
        pos = to_format.find(flag, pos);
        if (pos != std::string::npos) {
            to_format.replace(pos, flag.size(), replace);
            pos += replace.size();
        }
    }
}

logger const *client_logger::log(const std::string &text, logger::severity severity) const noexcept {
    std::string datetime_str = current_datetime_to_string();
    auto sep = datetime_str.find(' ');
    std::string date = datetime_str.substr(0, sep);
    std::string time = datetime_str.substr(sep);
    for (auto &[path, severities] : streams) {
        if (severities.find(severity) == severities.end()) continue;

        std::string message_log = struct_of_log;
        message_format(message_log, "%d", date);
        message_format(message_log, "%t", time);
        message_format(message_log, "%s", severity_to_string(severity));
        message_format(message_log, "%m", text);
        if (user_streams[path].first.is_open()) {
            user_streams[path].first << message_log;
        } else {
            std::cout << message_log;
        }

    }
    return this;
}