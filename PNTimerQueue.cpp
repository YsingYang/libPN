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
        printf("excuted handleFunctor\n");
}

PNTimerID PNTimerQueue::addTimer(const std::function<void()>& cb, PNTimestamp when, double interval){
    std::shared_ptr<PNTimer>timer(new PNTimer(cb, when, interval));
    loop_->runInLoop(std::bind(&PNTimerQueue::addTimerInLoop, this, timer));
    return PNTimerID(timer.get(), timer->getSequence());//注意这里把指针给了runInLoop, 但runInLoop不管理该timer的删除,
}

void PNTimerQueue::addTimerInLoop(std::shared_ptr<PNTimer> timer){
    loop_->assertInLoopThread();
    bool earliestChanged = insert(timer);//该timer, s是否最早到时
    if(earliestChanged ){//为什么最早到时是需要reset
        resetTimerFd(timerFd_, timer->getExpiration());
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










