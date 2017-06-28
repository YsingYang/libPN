#include "InetAddress.h"

#include <string.h>
#include <netinet/in.h>
#include <sys/socket.h>


void initSocketAddr(const char* ip, uint16_t port, struct sockaddr_in* addr){
    addr->sin_family = AF_INET;
    addr->sin_port  = htons(port);
    if(inet_pton(AF_INET, ip, &addr->sin_addr) <=0 ){
        perror("inet_pton error");
    }
}

inline void getFormatIP(char* ipBuff, size_t buffSize, struct sockaddr_in& addr){
    inet_ntop(AF_INET, &addr.sin_addr, ipBuff, sizeof(ipBuff));
}

inline uint16_t getFormatPort(struct sockaddr_in& addr){
    return ntohs(addr.sin_port);
}

void returnFormatString(char* Buff, size_t buffSize, struct sockaddr_in& addr){
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
    initSocketAddr(ip.c_str(), port, addr_);
}

InetAddress::InetAddress(const struct sockaddr_in& addr): addr_(addr){

}

std::string InetAddress::getFormatString() const{
    char buff[32];
    returnFormatString(buff, 32, addr_);
    return buff;//隐式转换

}
