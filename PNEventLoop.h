#ifndef PNEVENTLOOP_H
#define PNEVENTLOOP_H

#include <boost/noncopyable.hpp>
#include <thread>
#include <atomic>
#include <mutex>
#include "PNtype.h"
#include <vector>
#include <functional>
#include "PNTimer.h"
#include "PNTimerId.h"

/*************************************************************************************
对于one loop one event 来说, 每个线程至多只有一个eventLoop, (否则在epoll过程中会出现问题)
对于该网络模型而言, 每个需要监听的Event都需要先在EventLoop(Rector)中注册相应的信息, 然后
eventloop调用相应IO多路复用操作得到就绪的Event

**************************************************************************************/

class PNEvent;
class PNEpoll;
class PNTimestamp;
class PNTimer;
class PNTimerQueue;

class PNEventLoop : boost::noncopyable{
    typedef std::function<void()> callback_;
public:
    PNEventLoop();
    ~PNEventLoop();

    void loop();
    inline PNTimestamp getEpollReturnTime() const;

    void addEvent(PNEvent*);
    void updateEvent(PNEvent* );
    void updateEvent(int );//fd重载版本
    void removeEvent(PNEvent* );
    void removeEvent(int );//fd重载版本
    bool hasEvent(PNEvent* );
    inline void quit();
    void wakeup();//唤醒某个线程
    void assertInLoopThread() const; // 这个函数设置成inline的话成本有多高?

    void runInLoop(const callback_ &cb);
    PNTimerID runAt(const PNTimestamp& time, const PNTimer::TimerCallback &cb);
    //在某个时刻执行回调函数 cb

    PNTimerID runAfter(double delay, const PNTimer::TimerCallback &cb);
    //在延迟delay秒后执行回调函数.

    PNTimerID runEvery(double interval, const PNTimer::TimerCallback &cb);
    //每次interval都执行回调

    PNEventLoop* getEventLoopOnThread() const;

    void queueInLoop(const std::function<void()> & cb); ///muduo这里是public, 我觉得private就可以了吧? 需要给外部调用吗?


private:
    //成员函数
    inline bool isInLoopThread() const;
    void handleRead();
    void doPendingFunctors();

private:
    //成员变量
    Tid thisThreadID_;
    static int activeEventLength;
    std::atomic<int> running_;
    std::atomic<bool> quit_;
    PNEvent* currentActiveEvent; //我觉得该变量定义在这里的目的是不需要多次在loop里面构造又析构, 即便只是一个指针
    std::unique_ptr<PNEpoll> epoller_;
    std::unique_ptr<PNTimerQueue> timerQueue_;
    std::vector<PNEvent*> activeEventList_; //用于epoll返回的activechannel;

    PNTimestamp epollReturnTime_;//用于epoll定时?
    std::atomic<bool> callingPendingFunctors_;
    int wakeupFd_; //监听唤醒pipe;
    std::unique_ptr<PNEvent> wakeupEvent_;//唤醒事件
    std::mutex mtx_; //muduo 中这里定义成了mutable
    std::vector<std::function<void()>> pendingFunctors_; //待处理回调, 需要用mutex_保证线程安全
};

PNTimestamp PNEventLoop::getEpollReturnTime() const{
        return epollReturnTime_;
}

bool PNEventLoop::isInLoopThread() const{
    return thisThreadID_ == std::this_thread::get_id();
}

void PNEventLoop::quit(){
    quit_ = true;
    if(!isInLoopThread()){
        wakeup(); //如果不是当前线程执行quit , 唤醒该线程
    }
}

#endif // EVENTLOOP_H
