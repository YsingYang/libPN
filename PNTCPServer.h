#ifndef PNTCPSERVER_H
#define PNTCPSERVER_H

#include <boost/noncopyable.hpp>
#include <memory>
#include "callbackType.h"
#include <map>

class PNAcceptor;
class PNEventLoop;
class InetAddress;

class PNTCPServer : boost::noncopyable{
public:
    PNTCPServer(PNEventLoop* loop, const InetAddress& listenAddr);
    ~PNTCPServer();

    void start();
    inline void setConnectionCallback(const ConnectionCallback& cb);
    inline void setMessageCallback(const MessageCallback&cb);

private:
    void newConnection(int sockfd, const InetAddress& peerAddr);
    void removeConnection(const TCPConnectionPtr& conn);

    typedef std::map<std::string, TCPConnectionPtr> ConnectionMap;

private:
    PNEventLoop *loop_;
    const std::string name_; //ip->端口的format String
    std::unique_ptr<PNAcceptor> acceptor_;
    ConnectionCallback connctionCallback_;
    MessageCallback messageCallback_;
    bool started_;
    int nextConnId_; //好像是用于下一个连接的id , 用于标记连接id吗
    ConnectionMap connectedRecord_;
};

void PNTCPServer::setConnectionCallback(const ConnectionCallback& cb){
    connctionCallback_ = cb;
}

void PNTCPServer::setMessageCallback(const MessageCallback& cb){
    messageCallback_ = cb;
}



#endif // PNTCPSERVER_H
