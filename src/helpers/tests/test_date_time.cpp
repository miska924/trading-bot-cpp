#include <gtest/gtest.h>

#include "helpers/date_time.h"


TEST(DateTimeTest, TestDateTime) {
    std::string dateTimeStr = Helpers::DateTime(1716928388, Helpers::DATETIME_FORMAT_S);
    EXPECT_EQ(dateTimeStr, "2024-05-28T20:33:08Z");
}

TEST(DateTimeTest, TestParseDateTime) {
    std::string dateTimeStr = "2024-05-28T20:33:08Z";
    time_t time = Helpers::ParseDateTime(dateTimeStr.c_str(), Helpers::DATETIME_FORMAT_S);
    EXPECT_EQ(time, 1716928388);
}
