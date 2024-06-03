#include <fstream>
#include <gtest/gtest.h>
#include "server_logger.h"
#include <fstream>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>

#define MESSAGE_SIZE 100

TEST(ServerLoggerTest, Log) {
    // Подготовка данных для теста
    std::string socket_path = "/tmp/logger_socket";
    std::remove(socket_path.c_str()); // Удаляем сокет, если он уже существует

    // Создаем сокет
    int server_socket = socket(AF_UNIX, SOCK_STREAM, 0);
    ASSERT_NE(server_socket, -1) << "Failed to create server socket";

    // Создаем структуру для адреса сокета
    struct sockaddr_un server_addr;
    server_addr.sun_family = AF_UNIX;
    strncpy(server_addr.sun_path, socket_path.c_str(), sizeof(server_addr.sun_path) - 1);

    // Привязываем сокет к адресу
    ASSERT_NE(bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)), -1) << "Failed to bind server socket";

    // Слушаем подключения
    ASSERT_NE(listen(server_socket, 1), -1) << "Failed to listen on server socket";

    // Выполняем логирование
    {
        server_logger_builder builder;
        builder.add_console_stream(logger::severity::information); // Добавляем консольный вывод
        logger *logger_instance = builder.build();
        ASSERT_TRUE(logger_instance) << "Failed to create logger instance";

        std::string message = "Test message";
        logger::severity severity = logger::severity::information;
        const logger *result = logger_instance->log(message, severity);
        ASSERT_EQ(result, logger_instance) << "Logging failed";
    }

    // Принимаем соединение
    int client_socket = accept(server_socket, nullptr, nullptr);
    ASSERT_NE(client_socket, -1) << "Failed to accept connection";

    // Закрываем серверный сокет, чтобы избежать зависания
    close(server_socket);

    // Читаем данные из сокета
    char buffer[MESSAGE_SIZE];
    ssize_t bytes_read = recv(client_socket, buffer, sizeof(buffer), 0);
    ASSERT_NE(bytes_read, -1) << "Failed to read data from socket";

    // Закрываем клиентский сокет
    close(client_socket);

    // Удаляем созданный сокет
    std::remove(socket_path.c_str());
}


// Другие тесты можно добавить по аналогии с вышеуказанными

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
