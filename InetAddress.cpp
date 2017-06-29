#include "InetAddress.h"

#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <stdio.h>


void setNoBlockAndCloseOnExec(int fd){
    int flags = ::fcntl(fd, F_GETFL, 0);
    flags |=  (O_NONBLOCK | FD_CLOEXEC);
    int ret = ::fcntl(fd, F_SETFL, flags);
    if(ret < 0){
        perror("fcntl error");
    }
}

int createSocketFD(bool noblock ){
    int sockfd = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if(sockfd < 0 ){
        perror("socket fd create error ");
    }
    if(noblock){
        setNoBlockAndCloseOnExec(sockfd);
    }
    return sockfd;
}

void initSocketAddr(const char* ip, uint16_t port, struct sockaddr_in* addr){
    addr->sin_family = AF_INET;
    addr->sin_port  = htons(port);
    if(inet_pton(AF_INET, ip, &addr->sin_addr) <=0 ){
        perror("inet_pton error");
    }
}

//返回string格式的ip, 注意这里用了引用传参
inline void getFormatIP(char* ipBuff, size_t buffSize,const  struct sockaddr_in& addr){
    inet_ntop(AF_INET, &addr.sin_addr, ipBuff, 32 ); ///注意这里不能用sizeof(ipBuff, 传入的是一个指针， 这里sizeof会返回8(64位)
}

//返回host格式的端口
inline uint16_t getFormatPort(const struct sockaddr_in& addr){
    return ntohs(addr.sin_port);
}


//转换sockaddr_in的ip, 与port
void returnFormatString(char* Buff, size_t buffSize, const struct sockaddr_in& addr){
    char host[INET_ADDRSTRLEN]  = "INVALID";
    getFormatIP(host, INET_ADDRSTRLEN, addr);
    uint16_t port = getFormatPort(addr);
    snprintf(Buff, buffSize, "%s :%u", host, port);
}

//传port构造函数
InetAddress::InetAddress(uint16_t port){
    bzero(&addr_, sizeof(addr_));
    addr_.sin_family =AF_INET;
    addr_.sin_port = htons(port);
    addr_.sin_addr.s_addr = htonl(INADDR_ANY);
}

//传ip与port构造函数
InetAddress::InetAddress(const std::string& ip, uint16_t port){
    bzero(&addr_, sizeof(addr_));
    initSocketAddr(ip.c_str(), port, &addr_);
}

//sockaddr_in参数初始化
InetAddress::InetAddress(const struct sockaddr_in& addr): addr_(addr){

}

//返回Ip, port string的形式
std::string InetAddress::getFormatString() const{
    char buff[32];
    returnFormatString(buff, 32,  addr_);
    return buff;//隐式转换

}
