#ifndef PNTIMERQUEUE_H
#define PNTIMERQUEUE_H

#include <boost/noncopyable.hpp>
#include <functional>
#include <set>
#include <vector>

class PNEventLoop;
class PNTimer;
class PNTimerId;


class PNTimerQueue : boost::noncopyable{
public:
    PNTimerQueue(PNEventLoop* loop);
    ~PNTimerQueue();

    PNTimerID addTimer(std::function<void()> cb, PNTimestamp when, double interval);//添加一个定时器进队列中
    void cancel(PNTimerID timerid);

private:
    typedef std::pair<PNTimestamp, std::unique_ptr<PNTimer>> TimerKey; //这里是用shared_ptr好还是unique_ptr好
    typedef std::set<TimerKey> TimerSet;

private:
    void addTimerInLoop(PNTimer* timer);
    void cancelTimerInloop(TimerID, timerid);
    void handleRead();//处理读事件
    std::vector<TimerKey> getExpired(PNTimestamp now);//获取当前超时时间

private:
    PNEventLoop* loop_;
    const int timerFd_;
    PNEvent eventForTimerFd_;
    TimerSet timers_;



};




#endif // PNTIMERQUEUE_H
