#include "PNTimerQueue.h"

#include <sys/timerfd.h>
#include <sys/time.h>

//先封装相应的timefd的操作

int createTimerFd(){
    int timerfd = timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK |TFD_CLOEXEC);
    if(timerfd < 0){
        printf("Failed to create timerfd");
    }
    retur  timerfd;
}

//返回一个精确到纳秒的时间
struct timespec howMuchTimeFromNow(PNTimestamp when){
    int64_t microseconds = when.getMicroSeconds() - PNTimestamp::now().getMicroSeconds();
    if(microseconds < 100){
        microseconds = 100;
    }
    struct timespec ts;
    t
};
