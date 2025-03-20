
#include <gtest/gtest.h>
int add(int a, int b) { return a + b; };
int subtract(int a, int b) { return a - b; };
TEST(CalculatorTest, Addition)
{
    EXPECT_EQ(add(3, 4), 7);
    EXPECT_EQ(add(-1, 5), 4);
}

TEST(CalculatorTest, Subtraction)
{
    EXPECT_EQ(subtract(10, 3), 7);
    EXPECT_EQ(subtract(7, 7), 0);
}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
