#include "PNTimer.h"

PNTimer::PNTimer(uint32_t interval, std::function<void()>& cbFunctor):intervalMs_(interval), callback_(cbFunctor){
    update();
}

struct timeval PNTimer::getTimeOut(){ //返回一个timeval类型
    struct timeval timeOut = now_;
    timeOut.tv_sec += intervalMs / 1000;
    timeOut.tv_usec += (intervalMs % 1000) * 1000;
    return timeOut;//获取timeout时间
};

struct timespec PNTimer::getTimeInterval(){//返回当前离timeout的interval
    int64_t interval = getTimeOutMicroSecond() - getNowTimeMicroSecond();
    if(interval <= 10){
        interval = 10;
    }
    struct timespec timerInterval;
    timerInterval.tv_sec = interval / 1000;
    timerInterval.tv_nsec = (interval % 1000) * 1000000;
    return timerInterval;
};

PNTimer::update(){
    struct timezone tz;
    gettimeofday(&now_, &tz);
}

uint64_t PNTimer::getTimeOutMicroSecond(){
    struct timeval timeOut = getTimeOut(); //获取结束时间
    int mSecond = timeOut.tv_sec * 1000 + timeOut.tv_usec / 1000;
    return mSecond;
}

uint64_t PNTimer::getNowTimeMicroSecond(){ //返回当前微妙时间
    struct timezone tz;
    struct timeval nowDate;
    gettimeofday(&nowDate, &tz);
    uint64_t ms = (nowDate.tv_sec) * 1000 + nowDate.tv_usec / 1000;
    return ms;
}

void PNTimer::handler(){
    if(callback_){
        callback_();
    }
}
