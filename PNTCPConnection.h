#ifndef PNTCPCONNECTION_H
#define PNTCPCONNECTION_H


#include  <boost/noncopyable.hpp>
#include <boost/enable_shared_from_this.hpp>
#include "callbackType.h"
#include "InetAddress.h"
#include "Buffer.h"
#include <memory>
#include <atomic>

class PNEventLoop;
class PNSocketStruct;
class PNEvent;

class PNTCPConnection : boost::noncopyable, public std::enable_shared_from_this<PNTCPConnection>{
public:
    typedef std::shared_ptr<PNTCPConnection> TCPConnection;

    PNTCPConnection(PNEventLoop* loop, const std::string& name, int sockfd, const InetAddress& localAddr, const InetAddress& peerAddress);
    ~PNTCPConnection();

    //对外访问接口
    inline PNEventLoop* getLoop() const;
    inline const std::string& getName() const;
    inline const InetAddress& getLocalAddr() const;
    inline const InetAddress& getPeerAddr() const;
    inline bool getConnected() const;

    inline void setConnectionCallback(const ConnectionCallback& cb);
    inline void setMessageCallback(const MessageCallback& cb);
    inline void setCloseCallback(const CloseCallback& cb){
        closeCallback_ = cb;
    }

    void connectEstablished();//muduo  里  // called when TcpServer accepts a new connection// should be called only once
    void connectDestroyed(); // should be called only once too

private:
    enum State{kConnecting, kConnected, kDisconnected};//muduo用了这个记录该COnnection的状态

    void setState(State s){
        state_ = s;
    }
    void handleRead(PNTimestamp recviveTimestamp);
    void handleWrite();
    void handleError();
    void handleClose();

    PNEventLoop* loop_;
    std::string name_;

    std::atomic<State> state_;
    std::unique_ptr<PNSocketStruct> socket_;
    std::unique_ptr<PNEvent> event_;
    InetAddress localAddr_;
    InetAddress peerAddr_;
    ConnectionCallback connectCallback_;
    MessageCallback messageCallback_;
    CloseCallback closeCallback_; //用于析构时给TCPServer的mp去除掉相应的Connection
    Buffer inputBuffer_; //添加read需要使用到的Buffer类
};

PNEventLoop* PNTCPConnection::getLoop() const{
    return loop_;
}

const std::string& PNTCPConnection::getName() const{
    return name_;
}

const InetAddress& PNTCPConnection::getLocalAddr() const{
    return localAddr_;
}

const InetAddress& PNTCPConnection::getPeerAddr() const{
    return peerAddr_;
}

bool PNTCPConnection::getConnected() const{
    return state_ == kConnected;
}

void PNTCPConnection::setConnectionCallback(const ConnectionCallback& cb){
    connectCallback_ = cb;
}

void PNTCPConnection::setMessageCallback(const MessageCallback& cb){
    messageCallback_ = cb;
}


/*void PNTCPConnection::setCloseCallback(const CloseCallback& cb){
    closeCallback_ = cb;
}*/



#endif // PNTCPCONNECTION_H
