#include <fstream>
#include <gtest/gtest.h>
#include "server_logger.h"
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>

#define MESSAGE_SIZE 100

TEST(ServerLoggerTest, Log) {
    std::string socket_path = "/tmp/logger_socket";
    std::remove(socket_path.c_str());

    int server_socket = socket(AF_UNIX, SOCK_STREAM, 0);
    ASSERT_NE(server_socket, -1) << "Failed to create server socket";

    struct sockaddr_un server_addr;
    server_addr.sun_family = AF_UNIX;
    strncpy(server_addr.sun_path, socket_path.c_str(), sizeof(server_addr.sun_path) - 1);

    ASSERT_NE(bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)), -1) << "Failed to bind server socket";
    ASSERT_NE(listen(server_socket, 1), -1) << "Failed to listen on server socket";

    // Logging operation
    {
        server_logger_builder builder;
        builder.add_console_stream(logger::severity::information);
        logger *logger_instance = builder.build();
        ASSERT_TRUE(logger_instance) << "Failed to create logger instance";

        std::string message = "Test message";
        logger::severity severity = logger::severity::information;
        const logger *result = logger_instance->log(message, severity);
        ASSERT_EQ(result, logger_instance) << "Logging failed";
    }

    // Set timeout for accept
    struct timeval timeout;
    timeout.tv_sec = 5; // 5 seconds timeout
    timeout.tv_usec = 0;

    setsockopt(server_socket, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
    
    int client_socket = accept(server_socket, nullptr, nullptr);
    ASSERT_NE(client_socket, -1) << "Failed to accept connection";

    close(server_socket);

    char buffer[MESSAGE_SIZE];
    ssize_t bytes_read = recv(client_socket, buffer, sizeof(buffer), 0);
    ASSERT_NE(bytes_read, -1) << "Failed to read data from socket";

    close(client_socket);
    std::remove(socket_path.c_str());
}

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
