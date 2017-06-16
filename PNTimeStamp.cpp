#include "PNTimeStamp.h"
#include <boost/static_assert.hpp>
#include <boost/format.hpp>
#include <string>
#include <iostream>
#include <sstream>
#include <sys/time.h>
#include <inttypes.h>


BOOST_STATIC_ASSERT(sizeof(PNTimestamp) == sizeof(int64_t));

std::string PNTimestamp::toString() const{
    int64_t seconds = microSeconds_ / kMicroSecondsPerSecond;
    int64_t microSeconds = microSeconds_ % kMicroSecondsPerSecond;
    //char buf[32] = {0};
    //snprintf(buf, sizeof(buf)-1, "%" PRId64 ".%06" PRId64 "", seconds, microSeconds);
    std::stringstream ss; //用boost的话,效率应该会慢一点, 但安全性应该会好点
    ss<<boost::format("%ds:%06ldms" )%seconds %microSeconds<<std::endl;
    return ss.str();
}

std::string PNTimestamp::toFormattedString(bool showMicroseconds) const{
    time_t seconds = static_cast<time_t>(microSeconds_ / kMicroSecondsPerSecond);
    struct tm tm_time;
    gmtime_r(&seconds, &tm_time);//转换成年月日表示 此函数返回的时间日期未经时区转换，而是UTC时间。需要 + 8小时才是北京时间
    std::stringstream ss;
    if(showMicroseconds){
        int microSeconds = static_cast<int>(microSeconds_ % kMicroSecondsPerSecond);
        ss << boost::format("%4d%02d%02d %02d:%02d:%02d.%06d") %(tm_time.tm_year + 1900)%(tm_time.tm_mon + 1 )%tm_time.tm_mday
        %tm_time.tm_hour %tm_time.tm_min % tm_time.tm_sec%microSeconds<<std::endl;
    }
    else{
        ss<< boost::format("%4d%02d%02d %02d:%02d:%02d")%(tm_time.tm_year + 1900)%(tm_time.tm_mon + 1)%(tm_time.tm_mday)
             %tm_time.tm_hour %tm_time.tm_min  %tm_time.tm_sec<<std::endl;
    }
    return ss.str();
}


PNTimestamp PNTimestamp::now(){
    struct timeval tv;
    gettimeofday(&tv, nullptr);
    int64_t seconds = tv.tv_sec;
    return PNTimestamp(seconds * kMicroSecondsPerSecond + tv.tv_usec);
}

