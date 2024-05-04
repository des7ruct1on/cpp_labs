#include <gtest/gtest.h>
#include <client_logger.h>
#include <client_logger_builder.h>
#include <fstream>


TEST(ClientLoggerTest, LogToFileAndConsole) {
    logger_builder* builder = new client_logger_builder();

    logger* logger_tmp = builder
            ->add_file_stream("file_1.txt", logger::severity::information)
            ->add_file_stream("file_2.txt", logger::severity::debug)
            ->add_console_stream(logger::severity::debug)
            ->build();

    logger_tmp->log("Test message 1", logger::severity::debug);
    logger_tmp->log("Test message 2", logger::severity::information);
    logger_tmp->log("Test message 3", logger::severity::warning);
    logger_tmp->log("Test message 4", logger::severity::error);

    delete builder;
    delete logger_tmp;

    std::ifstream file1("file_1.txt");
    std::ifstream file2("file_2.txt");
    std::string line;
    int count = 0;
    while (std::getline(file1, line)) {
        count++;
    }
    while (std::getline(file2, line)) {
        count++;
    }
    file1.close();
    file2.close();
    ASSERT_EQ(count, 4); 

    std::cout << "Check console output for logged messages\n";
}


int main(
    int argc,
    char *argv[])
{
    testing::InitGoogleTest(&argc, argv);
    logger_builder* builder = new client_logger_builder();

    logger* logger_tmp = builder
            ->add_file_stream("file_1.txt", logger::severity::information)
            ->add_file_stream("file_2.txt", logger::severity::debug)
            ->add_console_stream(logger::severity::debug)
            ->build();
    logger_tmp->log("my_test_first", logger::severity::debug);
    logger_tmp->log("my_test_second", logger::severity::information);

    delete builder;
    delete logger_tmp;

    return RUN_ALL_TESTS();
}