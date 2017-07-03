#include "PNEventLoop.h"
#include "PNEpoll.h"
#include "PNEvent.h"
#include "PNTimerId.h"
#include "PNTimerQueue.h"
#include <assert.h>
#include <sys/poll.h>
#include <sys/eventfd.h>

int PNEventLoop::activeEventLength = 16;
__thread PNEventLoop* loopInThisThread = nullptr;
const int kEpollTImeMs = 10000;

static int createEventFd(){
    int evtfd = ::eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
    if( evtfd < 0){
        perror("event fd create error");
        exit(1);
    }
    return evtfd;
}

//constructor
PNEventLoop::PNEventLoop():
    thisThreadID_(std::this_thread::get_id()),
    running_(false),
    quit_(true),
    currentActiveEvent(nullptr),
    epoller_(new PNEpoll(this)),
    timerQueue_(new PNTimerQueue(this)),
    callingPendingFunctors_(false),
    wakeupFd_(createEventFd()),
    wakeupEvent_(new PNEvent(this, wakeupFd_))
{
    if(loopInThisThread){
        printf("Error : existing a eventloop in this thread\n");
        exit(0);
    }
    else{
        loopInThisThread = this;
    }
    wakeupEvent_->setReadCallback(std::bind(&PNEventLoop::handleRead, this));
    wakeupEvent_->enableReading();
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
        epollReturnTime_ = epoller_->poll(kEpollTImeMs, activeEventList_);
        for(auto &it : activeEventList_){ ///这里是用右值引用还是左值引用来着?
            it->handleFunc(epollReturnTime_);
        }
        doPendingFunctors();//执行相应回调
    }

    running_ = false; //关闭loop;
}

void PNEventLoop::runInLoop(const callback_ &cb){
    if(isInLoopThread()){
        cb();//如果在当前线程
    }
    else{
            queueInLoop(cb);
            // 如果是其它线程调用runInLoop，则异步地将cb添加到队列,让IO线程处理
    }
}


void PNEventLoop::assertInLoopThread() const {
    if(!isInLoopThread()){
        printf("Error : EventLoop not in thread\n");
        exit(0); //应该调用abortNotinthread方法的
    }
}

PNTimerID PNEventLoop::runAt(const PNTimestamp& time, const PNTimer::TimerCallback &cb){
    return timerQueue_->addTimer(cb, time, 0.0);
}

PNTimerID PNEventLoop::runAfter(double delay, const PNTimer::TimerCallback &cb){
    PNTimestamp time(addTime(PNTimestamp::now(), delay)); //返回一个当前时间+delay时间
    return runAt(time, cb);
}

PNTimerID PNEventLoop::runEvery(double interval, const PNTimer::TimerCallback &cb){
    PNTimestamp time(addTime(PNTimestamp::now(), interval));
    return timerQueue_->addTimer(cb, time, interval);// 在timerfd_createfd中的interval 设置为!=0
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

//处理wakeupfd 的可读事件,
void PNEventLoop::handleRead(){
    uint64_t count = 1; //eventfd, 可读是, 会读取出一个uint64_t的返回值
    ssize_t n = ::read(wakeupFd_, &count, sizeof(count));
    assert(n == sizeof(count));
}
/****
    解释一下这里唤醒的意思, 刚才拍了下脑袋竟然想明白了
    我们知道如果对epoll进行定时,(或者说无限等待-1), 那么线程将会阻塞在epoll_wait()上
    这时候为了唤醒该线程, 我们需要人为地给该线程制造一个事件, 而这里用到的就是wakeupFD,
*****/
void PNEventLoop::wakeup(){
    uint64_t count = 1;
    ssize_t n = ::write(wakeupFd_, &count, sizeof(count));//写入wakeupfd, 希望唤醒该线程
    assert( n == sizeof(count));
}

void PNEventLoop::queueInLoop(const std::function<void()>& cb){
    {
        std::lock_guard<std::mutex> lck(mtx_); //在嵌套内自动锁/ 释放, 保证线程安全
        pendingFunctors_.push_back(cb);
    }
    if(!isInLoopThread() || callingPendingFunctors_){ //如果执行loop不是相应的io线程, or 线程在执行callingPendingFunctors
        wakeup();///wakeup也可以是私有吧?
    }
}

void PNEventLoop::doPendingFunctors(){
    std::vector<std::function<void()>> functors;
    callingPendingFunctors_ = true; //表示正在执行pengdingFunctor
    {
        std::lock_guard<std::mutex> lck(mtx_);
        swap(functors, pendingFunctors_); //交换到局部变量, 减少领界区的长度
    }
    for(size_t i = 0; i < functors.size(); ++i){
        functors[i]();//执行相应的回调函数
    }
    callingPendingFunctors_ = false;
}
