#ifndef PNACCEPTOR_H
#define PNACCEPTOR_H

#include <boost/noncopyable.hpp>
#include <functional>

#include "PNEvent.h"
#include "PNSocketStruct.h"

class PNEventLoop;
class InetAddress;

class PNAcceptor : boost::noncopyable{
public:
    typedef std::function<void(int sockfd, const InetAddress&)> NewConnectionCallback;
    PNAcceptor(PNEventLoop* loop, const InetAddress& listenAddr);

    inline void setNewConnectionCallback(const NewConnectionCallback& cb);
    inline bool isListenning() const;
    void listen();

private:
    void handleRead();

    PNEventLoop* loop_;
    PNSocketStruct acceptSocket_;
    PNEvent acceptEvent_;
    NewConnectionCallback callback_;
    bool listenning_;
};


void PNAcceptor::setNewConnectionCallback(const NewConnectionCallback& cb){
    callback_ = cb;
}

bool PNAcceptor::isListenning() const{
    return listenning_;
}


#endif // PNACCEPTOR_H
