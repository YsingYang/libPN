#ifndef PNINETADDRESS_H
#define PNINETADDRESS_H

#include <netinet/in.h>
#include <string>


class InetAddress { //copyable
public:
    explicit InetAddress(uint16_t port);
    InetAddress(const std::string& IP, uint16_t port);
    InetAddress(const struct sockaddr_in& addr);

    InetAddress(const InetAddress&) = default;
    InetAddress& operator= (const InetAddress&) = default;
    ~InetAddress() = default;

    std::string getFormatString() const;

    inline const struct sockaddr_in& getSockAddr() const;
    inline  void setSockaddr(const struct sockaddr_in& );
private:
    struct sockaddr_in addr_;
};

const struct sockaddr_in& InetAddress::getSockAddr() const{
    return addr_;
};

void InetAddress::setSockaddr(const struct sockaddr_in& addr){
    addr_ = addr;
}
void setNoBlockAndCloseOnExec(int fd);

int createSocketFD(bool noblock = false);

struct sockaddr_in getLocalAddr(int sockfd);

int getSocketError(int sockfd);
#endif
