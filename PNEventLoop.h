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

/*************************************************************************************
对于one loop one event 来说, 每个线程至多只有一个eventLoop, (否则在epoll过程中会出现问题)
对于该网络模型而言, 每个需要监听的Event都需要先在EventLoop(Rector)中注册相应的信息, 然后
eventloop调用相应IO多路复用操作得到就绪的Event

**************************************************************************************/

class PNEvent;
class PNEpoll;
class PNTimerID;
class PNTimestamp;
class PNTimer;
class PNTimerQueue;

class PNEventLoop : boost::noncopyable{
    typedef std::function<void()> callback_;
public:
    PNEventLoop();
    ~PNEventLoop();

    void loop();

    void addEvent(PNEvent*);
    void updateEvent(PNEvent* );
    void updateEvent(int );//fd重载版本
    void removeEvent(PNEvent* );
    void removeEvent(int );//fd重载版本
    bool hasEvent(PNEvent* );
    inline void quit();
    void assertInLoopThread() const; // 这个函数设置成inline的话成本有多高?

    void runInLoop(const callback_ &cb);
    PNTimerID runAt(const PNTimestamp& time, const PNTimer::TimerCallback &cb);
    //在某个时刻执行回调函数 cb

    PNTimerID runAfter(double delay, const PNTimer::TimerCallback &cb);
    //在延迟delay秒后执行回调函数.

    PNTimerID runEvery(double interval, const PNTimer::TimerCallback &cb);
    //每次interval都执行回调

    PNEventLoop* getEventLoopOnThread() const;

private:
    //成员函数
    inline bool isInLoopThread() const;
    void abortNotInLoopThread();

private:
    //成员变量
    Tid thisThreadID_;
    static int activeEventLength;
    std::atomic<int> running_;
    std::atomic<bool> quit_;
    std::mutex mtx_; //muduo 中这里定义成了mutable
    PNEvent* currentActiveEvent; //我觉得该变量定义在这里的目的是不需要多次在loop里面构造又析构, 即便只是一个指针
    std::unique_ptr<PNEpoll> epoller_;
    std::unique_ptr<PNTimerQueue> timerQueue_;
    std::vector<PNEvent*> activeEventList_; //用于epoll返回的activechannel;

};

bool PNEventLoop::isInLoopThread() const{
    return thisThreadID_ == std::this_thread::get_id();
}

void PNEventLoop::quit(){
    quit_ = true;
}

#endif // EVENTLOOP_H
