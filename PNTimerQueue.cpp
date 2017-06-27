#include "PNTimerQueue.h"
#include "PNEventLoop.h"
#include "PNTimerId.h"
#include <sys/timerfd.h>
#include <time.h>
#include <assert.h>

//先封装相应的timefd的操作

int createTimerFd(){
    int timerfd = timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK |TFD_CLOEXEC);
    if(timerfd < 0){
        printf("Failed to create timerfd");
    }
    return  timerfd;
}


//返回一个精确到纳秒的时间
struct timespec howMuchTimeFromNow(PNTimestamp when){
    int64_t microseconds = when.getMicroSeconds() - PNTimestamp::now().getMicroSeconds();
    if(microseconds < 100){
        microseconds = 100;
    }
    struct timespec ts;
    ts.tv_sec = static_cast<time_t> (microseconds / PNTimestamp::kMicroSecondsPerSecond);
    ts.tv_nsec = static_cast<long>(microseconds % PNTimestamp::kMicroSecondsPerSecond) * 1000;
    return ts;
};

/**
自manpage
read(2) If the timer has already expired one or more times since its settings were last modified using timerfd_settime(), or since the last successful read(2),
 then the buffer given to read(2) returns an unsigned 8-byte integer (uint64_t) containing the number of expirations that have occurred
**/
void readTimerfd(int timerfd, PNTimestamp now){
    uint64_t times;
    ssize_t n = ::read(timerfd, &times, sizeof(times));
    printf("this time expired times : %ld \n",times );
    assert( n == sizeof(times)); //只支持64位机器下的e

}

void resetTimerFd(int timerFd, PNTimestamp expiration){ //提供对timersetting的接口
    struct itimerspec newTime;
    struct itimerspec oldTime;
    memset((void*)&newTime, 0, sizeof(itimerspec));
    memset((void*)&oldTime, 0, sizeof(itimerspec));
    newTime.it_value = howMuchTimeFromNow(expiration);//计算超时时间到当前时间的差值
    int ret = ::timerfd_settime(timerFd, 0, &newTime, &oldTime);
    if(ret){//成功返回0
        perror("timerfd setting error");
    }
}

PNTimerQueue::PNTimerQueue(PNEventLoop* loop) :
    loop_(loop),
    timerFd_(createTimerFd()),
    eventForTimerFd_(loop, timerFd_), //注意这里要用到timerFd, 所以位置在放在timerFd后面
    timers_(){

    eventForTimerFd_.setReadCallback(std::bind(&PNTimerQueue::handleRead, this));
    //将当前实例的handler, read绑定为该timerqueue的回调函数
    eventForTimerFd_.enableReading();
}

PNTimerQueue::~PNTimerQueue(){
    eventForTimerFd_.disableAll();//取消所有事件触发
    eventForTimerFd_.removeFromEventLoop();
    ::close(timerFd_);
    for(TimerSet::iterator it  = timers_.begin(); it != timers_.end(); ++it){
        //delete it->second;, muduo 中采用了 delete, 但我的timer用的是unique存的, 应该是不需要的吧
    }
}

#ifdef __GXX_EXPERIMENTAL_CXX0X__
//c++11 constructor
#endif // __GXX_EXPERIMENTAL_CXX0X__

void PNTimerQueue::handleRead(){
        loop_->assertInLoopThread();
        PNTimestamp now(PNTimestamp::now());
        readTimerfd(timerFd_, now);//读取当前超时出现次数

        std::vector<TimerKey> expired = getExpired(now); //超时timer队列

        for(std::vector<TimerKey>::iterator it = expired.begin(); it != expired.end(); ++it){
            it->second->run(); //遍历每一个超时timer, 并执行相应的run 方法, 其实就是执行timer的回调
        }
        reset(expired, now);
}

PNTimerID PNTimerQueue::addTimer(const std::function<void()>& cb, PNTimestamp when, double interval){
    std::shared_ptr<PNTimer>timer(new PNTimer(cb, when, interval));
    loop_->runInLoop(std::bind(&PNTimerQueue::addTimerInLoop, this, timer));

    return PNTimerID(timer.get(), timer->getSequence());//注意这里把指针给了runInLoop, 但runInLoop不管理该timer的删除,
}

void PNTimerQueue::addTimerInLoop(std::shared_ptr<PNTimer> timer){
    loop_->assertInLoopThread();
    bool earliestChanged = insert(timer);//该timer, s是否最早到时
    if(earliestChanged ){// 如果是最早到时, 调用reset
        resetTimerFd(timerFd_, timer->getExpiration());
    }
}

//重置expired数组
void PNTimerQueue::reset(const std::vector<TimerKey>& expired, PNTimestamp now){
    PNTimestamp nextExpire; //下一次超时时间
    for(std::vector<TimerKey>::const_iterator it = expired.cbegin(); it != expired.cend(); ++it){
        if(it->second->isRepeat()){ //如果该定时器是
            it->second->restart(now);
            insert(it->second); //重新插入到timer集合中
        }
    }
    if(!timers_.empty()){
        nextExpire = timers_.begin()->second->getExpiration();
    }
    if(nextExpire.valid()){
        resetTimerFd(timerFd_, nextExpire);//重新开始监听超时事件
    }
}

bool PNTimerQueue::insert(std::shared_ptr<PNTimer> timer){
    loop_->assertInLoopThread();
    //assert(timers_.size() == activeTImers.size());muduo多线程中加入, 暂时不理解用意
    bool earlestChanged = false;
    PNTimestamp expiration = timer->getExpiration();

    TimerSet::iterator it = timers_.begin();
    if(it == timers_.end() || expiration < it->first){ //如果timers为空, 或者新加入的 timer超时为最小
        earlestChanged = true;
    }
    std::pair<TimerSet::iterator, bool> insertResult = timers_.insert(TimerKey(expiration, timer));
    assert(insertResult.second);
    return earlestChanged;
}

std::vector<PNTimerQueue::TimerKey> PNTimerQueue::getExpired(PNTimestamp now){ //给handleread调用
    std::vector<TimerKey> expiredTimers;
    TimerKey sentry(now, nullptr);//哨兵
    auto bound = timers_.lower_bound(sentry);
    std::copy(timers_.begin(), bound, back_inserter(expiredTimers));//将超时的timer都插入到expiredTimer中

    timers_.erase(timers_.begin(), bound);

    return expiredTimers;
    /**********
    这里还要补充多线程的删除操作
    ***********/

}










