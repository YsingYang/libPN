#include "PNTCPServer.h"

#include "PNEventLoop.h"
#include "PNAccetpor.h"
#include "PNTCPConnection.h"
#include "callbackType.h"
#include <functional>

PNTCPServer::PNTCPServer(PNEventLoop* loop, const InetAddress& localAddr):
    loop_(loop), name_(localAddr.getFormatString()), acceptor_(new PNAcceptor(loop, localAddr)), started_(false), nextConnId_(1){
        acceptor_->setNewConnectionCallback(std::bind(&PNTCPServer::newConnection, this, std::placeholders::_1, std::placeholders::_2));
}

PNTCPServer::~PNTCPServer(){
}

void PNTCPServer::start(){
    if (!started_){
        started_ = true;
    }
    if(!acceptor_->isListenning()){
        loop_->runInLoop(std::bind(&PNAcceptor::listen, acceptor_.get()));
    }
}


void PNTCPServer::newConnection(int sockfd, const InetAddress& peerAddr){
    loop_->assertInLoopThread();
    char buf[32];
    snprintf(buf, sizeof(buf), "#%d", nextConnId_);
    ++nextConnId_;//这里除掉一个连接应该有个--吧
    std::string connName  = name_ + buf;
    InetAddress localAddr(getLocalAddr(sockfd));
    TCPConnectionPtr conn(new PNTCPConnection(loop_, connName, sockfd, localAddr, peerAddr));
    connectedRecord_[connName] = conn;
    conn->setConnectionCallback(connctionCallback_);
    conn->setMessageCallback(messageCallback_);
    conn->setCloseCallback(std::bind(&PNTCPServer::removeConnection, this, std::placeholders::_1));
    conn->connectEstablished();
}

void PNTCPServer::removeConnection(const TCPConnectionPtr& conn){
    loop_->assertInLoopThread();
    size_t n  = connectedRecord_.erase(conn->getName()); //the function returns the number of elements erased. 我似乎现在才知道erase的返回值是删除个数...
    assert(n == 1);
    loop_->queueInLoop(std::bind(&PNTCPConnection::connectDestroyed, conn));
}
//这个fd到底是哪个fd
