#include <gtest/gtest.h>

#include "helpers/function_queue.h"

TEST(FunctionQueueTest, TestMin) {
    Helpers::FunctionQueue<int> queue([](int a, int b) -> int {
        return a < b ? a : b;
    });

    queue.push(1);
    queue.push(2);
    queue.push(3);

    EXPECT_EQ(1, queue.getValue());
    EXPECT_EQ(1, queue.pop());
    EXPECT_EQ(2, queue.getValue());
    EXPECT_EQ(2, queue.pop());
    EXPECT_EQ(3, queue.getValue());
    EXPECT_EQ(3, queue.pop());
}


TEST(WeightedSumQueueTest, TestSimpleSum) {
    Helpers::WeightedSumQueue<int> queue(1);

    queue.push(1);
    queue.push(2);
    queue.push(3);

    EXPECT_EQ(6, queue.getValue());
    EXPECT_EQ(1, queue.pop());
    EXPECT_EQ(5, queue.getValue());
    EXPECT_EQ(2, queue.pop());
    EXPECT_EQ(3, queue.getValue());
    EXPECT_EQ(3, queue.pop());
}

