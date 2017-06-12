#include "PNEvent.h"

#include "PNEventLoop.h"
//定义PNEvent中几个常量变量

const uint32_t PNEvent::NoneEventFlag = 0;
const uint32_t PNEvent::readEventFlag = EPOLLIN | EPOLLPRI; //EPOLLPRI is used to receive urgent data
const uint32_t PNEvent::writeEventFlag = EPOLLOUT;
const uint32_t PNEvent::errorEventFlag = EPOLLERR;

//constructor
PNEvent::PNEvent(PNEventLoop* loop, int fd) :eventLoop_(loop), eventFD_(fd), addinLoop(false), event_(0), revent_(0){

}

PNEvent::~PNEvent(){
    //::close(eventFD_);//Event对象析构时, 注意移除掉fd
}

void PNEvent::handleFunc(){ //处理当前事件, 调用注册的回调函数
    if(revent_ & (EPOLLERR)){
        if(errorCallback_)
            errorCallback_(); //如果有, 调用回调
    }

    if(revent_ & (EPOLLIN | EPOLLPRI | EPOLLHUP)){
        if(readCallback_)
            readCallback_();
    }

    if(revent_ & (EPOLLOUT)){
        if(writeCallback_)
            writeCallback_();
    }
}


void PNEvent::update(){
    if(!addinLoop) //如果尚未添加到eventloop中, 先添加
        eventLoop_->addEvent(this);
    eventLoop_->updateEvent(this); //update操作扔给Eventloop, 然后 eventloop又会扔给Epoll进行操作
}

void PNEvent::removeFromEventLoop(){
    eventLoop_->removeEvent(this);//移除该Event
}



