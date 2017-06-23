#ifndef PNTIMERQUEUE_H
#define PNTIMERQUEUE_H

#include <boost/noncopyable.hpp>
#include <functional>
#include <set>
#include <vector>
#include <memory>
#include "PNEvent.h"

class PNEventLoop;
class PNTimer;
class PNTimerId;
class PNTimerID;
class PNTimestamp;

class PNTimerQueue : boost::noncopyable{
public:
    PNTimerQueue(PNEventLoop* loop);
    ~PNTimerQueue();

    PNTimerID addTimer(const std::function<void()> &cb, PNTimestamp when, double interval);//添加一个定时器进队列中
    void cancel(PNTimerID timerid);


private:
    typedef std::pair<PNTimestamp, std::shared_ptr<PNTimer>> TimerKey; //这里是用shared_ptr好还是unique_ptr好
    typedef std::set<TimerKey> TimerSet;

private:
    void addTimerInLoop(std::shared_ptr<PNTimer> timer);
    void cancelTimerInloop(PNTimerID timerid);
    void handleRead();//处理读事件
    void reset(const std::vector<TimerKey>& expired, PNTimestamp now);
    bool insert(std::shared_ptr<PNTimer> timer);
    std::vector<TimerKey> getExpired(PNTimestamp now);//获取当前超时时间

private:
    PNEventLoop* loop_;
    const int timerFd_;
    PNEvent eventForTimerFd_; // 这里没用指针为什么?
    TimerSet timers_;

};




#endif // PNTIMERQUEUE_H
