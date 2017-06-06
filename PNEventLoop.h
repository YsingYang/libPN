#ifndef PNEVENTLOOP_H

#include <boost/noncopyable.hpp>
#include <thread>
#include <atomic>
#include <mutex>

/*************************************************************************************
对于one loop one event 来说, 每个线程至多只有一个eventLoop, (否则在epoll过程中会出现问题)
对于该网络模型而言, 每个需要监听的Event都需要先在EventLoop(Rector)中注册相应的信息, 然后
eventloop调用相应IO多路复用操作得到就绪的Event

**************************************************************************************/

class PNEventLoop : boost::noncopyable{
public:
    PNEventLoop();
    ~PNEventLoop() = default;

    void loop();

    void addEvent(PNEvent*);
    void updateEvent(PNEvent* );
    void removeEvent(PNEvent* );
    bool hasEvent(PNEvent* );



private:
    //成员函数
    bool isInLoopThread() const{
        return thisThreadID_ == std::this_thread::get_id();  //获取当前线程的tid;
    }

private:
    //成员变量
    Tid thisThreadID_;
    static int activeEventLength;
    std::atomic<int> running_;
    std::mutex mtx_; //muduo 中这里定义成了mutable
    std::unique_ptr<PNEpoll> epoller_;
    vector<int>

};


#endif // EVENTLOOP_H
