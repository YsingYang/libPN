#ifndef PNTCPSERVER_H
#define PNTCPSERVER_H

#include <boost/noncopyable.hpp>

class PNTCPServer : boost::noncopyable{
public:

private:
    void newConnection(int sockfd, const InetAddress& peerAddr);

};



#endif // PNTCPSERVER_H
