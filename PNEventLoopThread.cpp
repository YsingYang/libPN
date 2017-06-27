#include "PNEventLoopThread.h"
#include "PNEventLoop.h"

PNEventLoopThread::PNEventLoopThread():
    loop_(nullptr),
    exiting_(false),
    thread_()//该线程一直在runthreadFunc函数
    {}

PNEventLoopThread::~PNEventLoopThread(){
    exiting_ = true;
    loop_->quit();
    thread_.join();///析构时不要忘记这一句
}


void PNEventLoopThread::threadFunc(){
    //printf("threadFunc %s \n", PNTimestamp(PNTimestamp::now()).toFormattedString().c_str());
    PNEventLoop loop;
    {//临界区
        std::lock_guard<std::mutex> lck(mutex_);
        loop_ = &loop;
        cond_.notify_one();
    }
    loop_->loop();
}

PNEventLoop* PNEventLoopThread::startLoop(){
    //printf("startLoop %s \n", PNTimestamp(PNTimestamp::now()).toFormattedString().c_str());
    {
        std::lock_guard<std::mutex> lck(mutex_); ///为了保证threadFunc在startLoop调用后执行, 构造一个领界区, 理由有thread temp变量, swapEventLoopThread中的thread_
        std::thread temp(std::bind(&PNEventLoopThread::threadFunc, this));
        swap(temp, thread_);
    }
    {
        std::unique_lock<std::mutex> lck(mutex_);
        while(loop_ == nullptr){
            cond_.wait(lck); //如果loop未创建, 调用条件变量
        }
    }
    return loop_;
}

