#ifndef PNTIMER_H
#define PNTIMER_H

#include <sys/time.h>
#include <functional>


class PNTimer{
public:
    PNTimer(uint32_t interval, std::function<void()> &cbFunctor);

    struct timeval getTimeOut(); //获取timeout时间]
    uint64_t getTimeOutMicroSecond();

    struct timespec getTimeInterval();

    void update();
    inline void setCallbackFunctor(const std::function<void()> &cbFunctor);

    static uint64_t getNowTimeMicroSecond();
    static std::string getNowTimeDate();

    void handler();

private:
    uint32_t intervalMs_;
    struct timeval now_;
    std::function<void()> callback_;
};


PNTimer::setCallbackFunctor(const std::function<void()>& cbFunctor){
    callback_ = cbFunctor;
}

/*************************************
注 : timespec , 与 timeval区别,
timespec : 精确到纳秒级别
timeval : 精确到微秒级别

秒->毫秒->微秒->纳秒
**************************************/

#endif // TIMER_H
