#include <gtest/gtest.h>

#include <big_integer.h>
#include <client_logger.h>
#include <operation_not_supported.h>
#include <fraction.h>

logger *create_logger(
    std::vector<std::pair<std::string, logger::severity>> const &output_file_streams_setup,
    bool use_console_stream = true,
    logger::severity console_stream_severity = logger::severity::debug)
{
    logger_builder *builder = new client_logger_builder();
    
    if (use_console_stream)
    {
        builder->add_console_stream(console_stream_severity);
    }
    
    for (auto &output_file_stream_setup: output_file_streams_setup)
    {
        builder->add_file_stream(output_file_stream_setup.first, output_file_stream_setup.second);
    }
    
    logger *built_logger = builder->build();
    
    delete builder;
    
    return built_logger;
}

TEST(positive_tests, test1)
{
    logger *logger = create_logger(std::vector<std::pair<std::string, logger::severity>>
        {
            {
                "bigint_logs.txt",
                logger::severity::information
            },
        });
    
    big_integer bigint_1("18");
    big_integer bigint_2("3");
    fraction frct(std::move(bigint_1), std::move(bigint_2));
    
    std::stringstream ss;
    ss << frct;
    std::string result_string = ss.str();
    
    EXPECT_TRUE(result_string == "6/1");

    delete logger;
}

TEST(positive_tests, test2)
{
    logger *logger = create_logger(std::vector<std::pair<std::string, logger::severity>>
        {
            {
                "bigint_logs.txt",
                logger::severity::information
            },
        });
    
    big_integer bigint_1("18");
    big_integer bigint_2("3");
    
    fraction frct_1(std::move(bigint_1), std::move(bigint_2));

    big_integer bigint_3("18");
    big_integer bigint_4("2");
    fraction frct_2(std::move(bigint_3), std::move(bigint_4));

    fraction result_of_sum = frct_1 + frct_2;
    
    std::stringstream ss;
    ss << result_of_sum;
    std::string result_string = ss.str();
    std::cout<<result_string<<std::endl;
    EXPECT_TRUE(result_string == "15/1");
    
    delete logger;
}

TEST(positive_tests, test3)
{
    logger *logger = create_logger(std::vector<std::pair<std::string, logger::severity>>
        {
            {
                "bigint_logs.txt",
                logger::severity::information
            },
        });

    big_integer bigint_1("18");
    big_integer bigint_2("3");
    
    fraction frct_1(std::move(bigint_1), std::move(bigint_2));

    big_integer bigint_3("18");
    big_integer bigint_4("2");
    fraction frct_2(std::move(bigint_3), std::move(bigint_4));

    fraction result_of_sub = frct_1 - frct_2;
    
    std::stringstream ss;
    ss << result_of_sub;
    std::string result_string = ss.str();
    
    EXPECT_TRUE(result_string == "-3/1");

    delete logger;
}

TEST(positive_tests, test4)
{
    logger *logger = create_logger(std::vector<std::pair<std::string, logger::severity>>
        {
            {
                "bigint_logs.txt",
                logger::severity::information
            },
        });
    
    big_integer bigint_1("18");
    big_integer bigint_2("3");
    
    fraction frct_1(std::move(bigint_1), std::move(bigint_2));

    big_integer bigint_3("18");
    big_integer bigint_4("2");
    fraction frct_2(std::move(bigint_3), std::move(bigint_4));

    fraction result_of_mult = frct_1 * frct_2;
    
    std::stringstream ss;
    ss << result_of_mult;
    std::string result_string = ss.str();
    
    EXPECT_TRUE(result_string == "54/1");
    
    delete logger;
}

TEST(positive_tests, test5)
{
    logger *logger = create_logger(std::vector<std::pair<std::string, logger::severity>>
        {
            {
                "bigint_logs.txt",
                logger::severity::information
            },
        });
    
    big_integer bigint_1("18");
    big_integer bigint_2("3");
    
    fraction frct_1(std::move(bigint_1), std::move(bigint_2));

    big_integer bigint_3("18");
    big_integer bigint_4("2");
    fraction frct_2(std::move(bigint_3), std::move(bigint_4));

    fraction result_of_div = frct_1 / frct_2;
    
    std::stringstream ss;
    ss << result_of_div;
    std::string result_string = ss.str();
    
    EXPECT_TRUE(result_string == "2/3");
    
    delete logger;
}

TEST(positive_tests, test6)
{
    logger *logger = create_logger(std::vector<std::pair<std::string, logger::severity>>
        {
            {
                "bigint_logs.txt",
                logger::severity::information
            },
        });
    
    big_integer bigint_1("18");
    big_integer bigint_2("3");
    
    fraction frct_1(std::move(bigint_1), std::move(bigint_2));

    big_integer bigint_3("18");
    big_integer bigint_4("2");
    fraction frct_2(std::move(bigint_3), std::move(bigint_4));

    EXPECT_TRUE(frct_1 <= frct_2);
    
    delete logger;
}

TEST(positive_tests, test7)
{
    logger *logger = create_logger(std::vector<std::pair<std::string, logger::severity>>
        {
            {
                "bigint_logs.txt",
                logger::severity::information
            },
        });
    
    big_integer bigint_1("187");
    big_integer bigint_2("3");
    
    fraction frct_1(std::move(bigint_1), std::move(bigint_2));

    big_integer bigint_3("18");
    big_integer bigint_4("2");
    fraction frct_2(std::move(bigint_3), std::move(bigint_4));

    EXPECT_TRUE(frct_1 >= frct_2);
    
    delete logger;
}

TEST(positive_tests, test8)
{
    logger *logger = create_logger(std::vector<std::pair<std::string, logger::severity>>
        {
            {
                "bigint_logs.txt",
                logger::severity::information
            },
        });
    
    big_integer bigint_1("187");
    big_integer bigint_2("31");
    
    fraction frct_1(std::move(bigint_1), std::move(bigint_2));

    big_integer bigint_3("18");
    big_integer bigint_4("2");
    fraction frct_2(std::move(bigint_3), std::move(bigint_4));

    EXPECT_TRUE(frct_1 != frct_2);
    
    delete logger;
}

TEST(positive_tests, test9)
{
    logger *logger = create_logger(std::vector<std::pair<std::string, logger::severity>>
        {
            {
                "bigint_logs.txt",
                logger::severity::information
            },
        });
    
    big_integer bigint_1("18");
    big_integer bigint_2("3");
    
    fraction frct_1(std::move(bigint_1), std::move(bigint_2));

    big_integer bigint_3("36");
    big_integer bigint_4("6");
    fraction frct_2(std::move(bigint_3), std::move(bigint_4));

    EXPECT_TRUE(frct_1 == frct_2);
    
    delete logger;
}

int main(
    int argc,
    char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    
    return RUN_ALL_TESTS();
}