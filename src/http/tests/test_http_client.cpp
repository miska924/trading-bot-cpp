#include <gtest/gtest.h>

#include "http/client.h"


TEST(HttpClientTest, TestGet) {
    TradingBot::HttpClient client("https://jsonplaceholder.typicode.com");

    Json::Value response = client.get("/posts");
    ASSERT_TRUE(response.isArray());  // Ожидаем, что ответ будет массивом JSON-объектов
    ASSERT_FALSE(response.empty());  // Убедимся, что массив не пуст
    std::cerr << response.toStyledString() << std::endl;

    // Проверка первого элемента массива, если массив не пуст
    if (!response.empty()) {
        const auto& firstPost = response[0];
        ASSERT_TRUE(firstPost.isObject());
        EXPECT_TRUE(firstPost.isMember("userId"));
        EXPECT_TRUE(firstPost.isMember("id"));
        EXPECT_TRUE(firstPost.isMember("title"));
        EXPECT_TRUE(firstPost.isMember("body"));
    }
}
