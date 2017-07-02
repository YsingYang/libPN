#include "PNTCPConnection.h"
#include "PNEventLoop.h"
#include "PNSocketStruct.h"
#include "PNEvent.h"
#include <iostream>


//这里有一点要注意， 确保这里loop不能为空
PNTCPConnection::PNTCPConnection(PNEventLoop* loop, const std::string& name, int sockfd, const InetAddress& localAddr, const InetAddress& peerAddr):
    loop_(loop), name_(name), state_(kConnecting),  socket_(new PNSocketStruct(sockfd)), event_(new PNEvent(loop, sockfd)), localAddr_(localAddr), peerAddr_(peerAddr){
        std::cout<<"TcpConnection::ctor [" << name_ <<" ] and fd : [" <<sockfd<<" ] "<<std::endl;
        event_->setReadCallback(std::bind(&PNTCPConnection::handleRead, this)); //事件触发时， 回调handleRead
        event_->setWriteCallback(std::bind(&PNTCPConnection::handleWrite, this));
        event_->setCloseCallback(std::bind(&PNTCPConnection::handleClose, this));
        event_->setErrorCallback(std::bind(&PNTCPConnection::handleError, this));
}
///connection的sockfd会在自己内部socketStruct析构的时候close掉， 所以connection并不用管理sockfd

PNTCPConnection::~PNTCPConnection(){
    std::cout<<"TcpConnection::dtor [ "<<name_<<"] and fd : [" <<event_->getEventFD()<<std::endl;
}

void PNTCPConnection::connectEstablished(){
    loop_->assertInLoopThread();
    assert(state_ == kConnecting);
    setState(kConnected); //状态改为已连接
    event_->enableReading();///注意这里， 直到这里， 才开启该Connection的enableReading
    connectCallback_(shared_from_this()); //回调connectionCallback
}

void PNTCPConnection::connectDestroyed(){ //最后 Connection生命的最后一步（如果没有Bug）
    loop_->assertInLoopThread();
    assert(state_ == kConnected);
    setState(kDisconnected);
    event_->disableAll();
    connectCallback_(shared_from_this());
    loop_->removeEvent(event_.get());
}

void PNTCPConnection::handleRead(){
    char buf[65535];
    bzero(buf, 65535);
    ssize_t n = ::read(event_->getEventFD(), buf, 65535);
    if(n > 0){
       messageCallback_(shared_from_this(), buf, n);
    }
    else if(n == 0){
        handleClose();
    }
    else{
        handleError();
    }
}

void PNTCPConnection::handleWrite(){}

void PNTCPConnection::handleClose(){ //被动断开
    loop_->assertInLoopThread();
    assert(state_ == kConnected);
    event_->disableAll();
    closeCallback_(shared_from_this()); //这里的closeCallback是给Server用的， 绑定于Server的removeConnection
}


void PNTCPConnection::handleError(){
    //int err = getSocketError(event_->getEventFD());
    perror("Handle Error err");

}


