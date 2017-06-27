#ifndef EVENTLOOPTHREAD
#define EVENTLOOPTHREAD

#include <condition_variable>
#include <mutex>
#include <thread>

class PNEventLoop;

class PNEventLoopThread{
public:
    PNEventLoopThread();
    ~PNEventLoopThread();

    PNEventLoop* startLoop();//开启loop函数

private:
    void threadFunc();

    PNEventLoop *loop_;
    bool exiting_;
    std::thread thread_;
    std::mutex mutex_;
    std::condition_variable cond_;
};


#endif // EVENTLOOPTHREAD
