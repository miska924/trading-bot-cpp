#include "helpers/date_time.h"


namespace Helpers {

    time_t ParseDateTime(const char* datetimeString, const std::string& datetimeFormat)
    {
        struct tm tmStruct;
        strptime(datetimeString, datetimeFormat.c_str(), &tmStruct);
        tmStruct.tm_isdst = 0;
        time_t time = timegm(&tmStruct);
        return time;
    }

    std::string DateTime(time_t time, const std::string& datetimeFormat)
    {
        char buffer[90];
        struct tm* timeinfo = gmtime(&time);
        strftime(buffer, sizeof(buffer), datetimeFormat.c_str(), timeinfo);
        return buffer;
    }

} // namespace Helpers
