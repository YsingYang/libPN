#ifndef "PNSOCKETSTRUCT_H"
#define "PNSOCKETSTRUCT_H"

#include <boost/noncopyable.hpp>

class PNSocketStruct : boost::noncopyable{
public:
    explicit PNSocketStruct(int sockfd);
    ~PNSocketStruct;

    inline int getSocketFD () const;

    void bindSockaddr(const InetAddress &);
    void listen();

    int accept(InetAddress*);
private:
    const int sockFD_;
};

int PNSocketStruct::getSocketFD() const{
    return sockFD_;
}




#endif
