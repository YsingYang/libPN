#include "PNAccetpor.h"
#include "InetAddress.h"
#include "PNEventLoop.h"
#include "PNSocketStruct.h"


PNAcceptor::PNAcceptor(PNEventLoop* loop, const InetAddress& addr) :
    loop_(loop),
    acceptSocket_(createSocketFD()),
    acceptEvent_(loop, acceptSocket_.getSocketFD()),
    listenning_(false){

    acceptSocket_.setReuseAddr(true);
    acceptSocket_.bindSockaddr(addr);
    acceptEvent_.setReadCallback(std::bind(&PNAcceptor::handleRead, this));
}

void PNAcceptor::listen(){
    loop_->assertInLoopThread();
    listenning_ = true;
    acceptSocket_.listenForConnect();
    acceptEvent_.enableReading();
}


void PNAcceptor::handleRead(){
    loop_->assertInLoopThread();//避免跨线程调用
    InetAddress peerAddr(0);//
    int connfd = acceptSocket_.accept(&peerAddr);
    struct sockaddr_in temp;

    if(connfd >= 0){
        if(callback_){
            callback_(connfd, peerAddr);
        }
        else{
            ::close(connfd);//如果没有设置NewConnectionCallback, 解除连接
        }
    }

}
