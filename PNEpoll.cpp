#include "PNEpoll.h"
#include <iostream>
#include <assert.h>
#include "PNEvent.h"

/******************
    Timerfd Test;
*******************/
PNEpoll::PNEpoll(PNEventLoop* loop): epollFD_(::epoll_create(EPOLL_CLOEXEC)), eventList_(InitEventListSize), ownerLoop_(loop) {
    if(epollFD_ <0){
        perror("Create epollFD error ");
        exit(0);
    }
}

PNEpoll::~PNEpoll(){
    ::close(epollFD_);//关闭fd;
}

void PNEpoll::poll(std::vector<PNEvent* > &activeEventsList){
    //poll前不需要对eventList进行memset?

    int activeNum = epoll_wait(epollFD_, &eventList_[0], static_cast<int>(eventList_.size()), -1);///最后的 timestamp之后再加上
    if(activeNum < 0 && errno != EINTR){
        perror("epoll_wait get error : " );
        exit(0);
    }
    if(activeNum == 0){
        printf("Nothing happen\n");
    }
    else{
        fillActivEventsList(activeNum, activeEventsList);
        if(static_cast<size_t>(activeNum) == eventList_.size()){
            eventList_.resize(activeNum * 2);//当监听事件总量到达vector上限时, 类似vector一样扩展上限
        }
    }
}

void PNEpoll::fillActivEventsList(int numsActiveEvents, std::vector<PNEvent*>& activeEventList){
    assert(static_cast<size_t>(numsActiveEvents) <= eventList_.size());

    for(int i = 0; i < numsActiveEvents; ++i){
        PNEvent* event = static_cast<PNEvent*>(eventList_[i].data.ptr); // 将eventlist中记录的data转为event指针

        int fd = event->getEventFD();
        if(eventMap_.find(fd) == eventMap_.end()){
            printf("Can find eventfd in eventMap error \n");
            exit(0); //暂时定为退出
        }
        assert(eventMap_[fd] == event);

        event->setRevent(eventList_[i].events);//设置返回到的事件
        activeEventList.push_back(event);
    }
}

void PNEpoll::addEvent(PNEvent* event){
    assertInLoopThread();
    const int efd = event->getEventFD();
    assert(eventMap_.find(efd) == eventMap_.end());
    update(EPOLL_CTL_ADD, event);
    eventMap_[efd] = event;
}

void PNEpoll::updateEvent(PNEvent* event){
    assertInLoopThread();
    const int efd = event->getEventFD();
    assert(eventMap_.find(efd) != eventMap_.end());
    update(EPOLL_CTL_MOD, event);
}

void PNEpoll::removeEvent(PNEvent* event){
    assertInLoopThread();
    const int efd = event->getEventFD();
    assert(eventMap_.find(efd) != eventMap_.end());
    update(EPOLL_CTL_DEL, event);
    eventMap_.erase(efd);
}


void PNEpoll::update(int operation, PNEvent* event){
    struct epoll_event epollEvent;
    memset((void*)&epollEvent, 0, sizeof(epoll_event));

    epollEvent.events = event->getEvent(); //获取需要监听的事件
    epollEvent.data.ptr = event;
    int fd = event->getEventFD();

    if(epoll_ctl(epollFD_, operation, fd, &epollEvent) < 0 ){
        perror("epoll_ctl error");
        exit(0);
    }
}
