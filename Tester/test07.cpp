#include "../InetAddress.h"
#include "../PNAccetpor.h"
#include "../PNEventLoop.h"

#include <stdio.h>

void newConnection(int sockfd, const InetAddress& addr){
    printf("newConnection(): accepted a new connection from %s\n", addr.getFormatString().c_str());
    ::write(sockfd, "How are you \n", 13);
    close(sockfd);
}

int main(){
    InetAddress listenAddr(9981);
    PNEventLoop loop;
    PNAcceptor acceptor(&loop, listenAddr);
    acceptor.setNewConnectionCallback(newConnection);
    acceptor.listen();
    loop.loop();
}
