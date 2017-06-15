#include "PNTimeStamp.h"

PNTimestamp PNTimestamp::now(){
    struct timeval tv;
    gettimeofday(&tv, nullptr);
    int64_t seconds = tv.tv_sec;
    return PNTimestamp(seconds * kMicroSecondsPerSecond + tv.tv_usec);
}
