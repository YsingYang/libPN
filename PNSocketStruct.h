#ifndef PNSOCKETSTRUCT_H
#define PNSOCKETSTRUCT_H

#include <boost/noncopyable.hpp>


class InetAddress;
class PNSocketStruct : boost::noncopyable{
public:
    explicit PNSocketStruct(int sockfd) : sockFD_(sockfd){};
    ~PNSocketStruct();

    inline int getSocketFD () const;

    void bindSockaddr(const InetAddress &);
    void listenForConnect();

    int accept(InetAddress*);
    void setReuseAddr(bool on);//这个用途可以参考下文档
private:
    const int sockFD_;
};

int PNSocketStruct::getSocketFD() const{
    return sockFD_;
}



#endif
