#ifndef PNTIMER_H
#define PNTIMER_H

#include <sys/time.h>
#include <functional>
#include <PNTimeStamp.h>
#include <atomic>

class PNTimer{
public:
    PNTimer(const std::function<void()>& cb, PNTimestamp when, double interval):
        callback_(cb), expiration_(when), interval_(interval), repeat_(interval > 0.0 ), sequence_(TimerNumsCreate_++){}

    void run() const;

    inline PNTimestamp getExpiration() const;
    inline bool isRepeat() const;
    inline int64_t getSequence() const;

    void restart(Timestamp now);
    static inline int getTimerNumsCreate() const;

private:
    const std::function<void()> callback_;
    PNTimestamp expiration_;//到期时间戳
    const double interval_; //间隔
    const bool repeat_;//是否重复计时
    const int64_t sequence_; //sequence似乎是记录当前计时器的序号的

    static std::atomic<int>  TimerNumsCreate_; //当前创建了多少个timer, 对于这个变量来说, 需要线程安全, 如果我只把他定义为atomic<int>保证是线程安全吗
};

PNTimestamp PNTimer::getExpiratiob() const{
    return expiration_;
}

bool PNTimer::isRepeat() const{
    return repeat_;
}

int64_t PNTimer::getSequence() const{
    return sequence_;
}

int PNTimer::getTimerNumsCreate() const{
    return TimerNumsCreate_;//返回类型不是一个atomic<int >应该不影响本身的线程安全"?
}

/*************************************
注 : timespec , 与 timeval区别,
timespec : 精确到纳秒级别
timeval : 精确到微秒级别

秒->毫秒->微秒->纳秒
**************************************/

#endif // TIMER_H
