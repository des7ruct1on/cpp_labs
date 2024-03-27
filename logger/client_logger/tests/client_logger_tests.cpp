#include <gtest/gtest.h>
#include <client_logger.h>
#include <client_logger_builder.h>
#include <fstream>

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