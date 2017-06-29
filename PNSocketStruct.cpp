#include "PNSocketStruct.h"
#include "InetAddress.h"

#include <netinet/in.h>
#include <netinet/tcp.h>
#include <boost/implicit_cast.hpp>
#include <string.h>

PNSocketStruct::~PNSocketStruct(){
    if(::close(sockFD_) < 0){
        perror("SockFD close error");
    }
}

void PNSocketStruct::bindSockaddr(const InetAddress& addr){
    if(::bind(sockFD_, static_cast<const sockaddr*>(boost::implicit_cast<const void*>(&addr.getSockAddr())), sizeof(sockaddr)) < 0){
        perror("Binding FD, error");
    }
}

void PNSocketStruct::listenForConnect(){
    int ret = ::listen(sockFD_, SOMAXCONN);
    if(ret < 0){
        perror("listen error ");
    }
}

int PNSocketStruct::accept(InetAddress* peeraddr ){ //传入的一般是peerSockaddr_in
    struct sockaddr_in addr;
    bzero(&addr, sizeof(addr));
    socklen_t addrLength = sizeof(sockaddr_in);
    printf("listening\n");
    int connfd = ::accept(sockFD_, static_cast<sockaddr*> (boost::implicit_cast<void*>(&addr)), &addrLength);


    setNoBlockAndCloseOnExec(connfd);
    if ( connfd < 0){
        perror("connfd error ");
    }
    if(connfd >= 0){
        peeraddr->setSockaddr(addr);//根据收到的addr_in结构， 记录进peeraddr
    }
    return connfd;//返回连接的fd
}

void PNSocketStruct::setReuseAddr(bool on = true){
    int optval  = on ? 1 : 0;
    ::setsockopt(sockFD_, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));
}


