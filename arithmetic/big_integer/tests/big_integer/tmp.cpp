#include <gtest/gtest.h>



TEST(positive_tests, test1)
{
    int a = 5;
    int b = 5
    EXPECT_TRUE(a == b);
    
}

int main(
    int argc,
    char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    
    return RUN_ALL_TESTS();
}