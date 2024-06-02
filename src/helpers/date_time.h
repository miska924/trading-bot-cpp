#pragma once

#include <string>
#include <time.h>


namespace Helpers {

    const std::string DATETIME_FORMAT_S = "%Y-%m-%dT%H:%M:%SZ";
    const std::string DATETIME_FORMAT_MS = "%Y-%m-%dT%H:%M:%S.%3NZ";

    time_t ParseDateTime(const char* datetimeString, const std::string& datetimeFormat = DATETIME_FORMAT_S);
    std::string DateTime(time_t time, const std::string& datetimeFormat = DATETIME_FORMAT_S);

} // namespace Helpers
