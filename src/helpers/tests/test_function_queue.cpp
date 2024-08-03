#include <gtest/gtest.h>

#include "helpers/function_queue.h"

TEST(FunctionQueueTest, TestMin) {
    Helpers::FunctionQueue<int> queue([](int a, int b) -> int {
        return a < b ? a : b;
    });

    queue.push(1);
    queue.push(2);
    queue.push(3);

    EXPECT_EQ(1, queue.functionValue());
    EXPECT_EQ(1, queue.pop());
    EXPECT_EQ(2, queue.functionValue());
    EXPECT_EQ(2, queue.pop());
    EXPECT_EQ(3, queue.functionValue());
    EXPECT_EQ(3, queue.pop());
}


