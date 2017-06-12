#include "PNEventLoop.h"
#include "PNEpoll.h"
#include "PNEvent.h"
#include <assert.h>
#include <sys/poll.h>

int PNEventLoop::activeEventLength = 16;
__thread PNEventLoop* loopInThisThread = nullptr;


//constructor
PNEventLoop::PNEventLoop():
    thisThreadID_(std::this_thread::get_id()),
    running_(false),
    quit_(true),
    currentActiveEvent(nullptr),
    epoller_(new PNEpoll(this))
{
    if(loopInThisThread){
        printf("Error : existing a eventloop in this thread\n");
        exit(0);
    }
    else{
        loopInThisThread = this;
    }
}

//destructor
PNEventLoop::~PNEventLoop(){
    assert(!running_);//析构前loop必须停止
    loopInThisThread = nullptr;
}

PNEventLoop* PNEventLoop::getEventLoopOnThread() const { ///返回当前线程的eventloop.感觉有点奇怪?
    return loopInThisThread;
}

void PNEventLoop::loop(){ //主要执行函数
    assert(!running_);
    assertInLoopThread();
    running_ = true;//开启loop
    quit_ = false;

    while(!quit_){
        activeEventList_.clear();
        epoller_->poll(activeEventList_);
        for(auto &it : activeEventList_){ ///这里是用右值引用还是左值引用来着?
            it->handleFunc();
        }
    }

    running_ = false; //关闭loop;
}


void PNEventLoop::assertInLoopThread() const {
    if(!isInLoopThread()){
        printf("Error : EventLoop not in thread\n");
        exit(0); //应该调用abortNotinthread方法的
    }
}


void PNEventLoop::addEvent(PNEvent* event){
    epoller_->addEvent(event);
}

void PNEventLoop::updateEvent(PNEvent* event){
    //这里的assert可以放在epoll类中进行assert(fd)
    epoller_->updateEvent(event);
}

void PNEventLoop::removeEvent(PNEvent* event){
    epoller_-> removeEvent(event);
}

