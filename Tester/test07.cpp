#include "../InetAddress.h"
#include "../PNAccetpor.h"
#include "../PNEventLoop.h"
#include "../PNTimeStamp.h"

#include <stdio.h>

void newConnection(int sockfd, const InetAddress& addr){
    printf("newConnection(): accepted a new connection from %s\n", addr.getFormatString().c_str());
    ::write(sockfd, "How are you \n", 13);
    close(sockfd);
}


//将服务器改为daytime服务器
void timeConncetion(int sockfd, const InetAddress& addr){
    printf("newConnection(): accepted a new connection from %s\n", addr.getFormatString().c_str());
    const char* timeStame = PNTimestamp(PNTimestamp::now()).toFormattedString().c_str();
    ::write(sockfd,  timeStame , strlen(timeStame) + 1);
    close(sockfd);
}

//同时监听2个port
void newConnection_2(int sockfd, const InetAddress& addr){
     printf("newConnection(): accepted a new connection from %s\n", addr.getFormatString().c_str());
    ::write(sockfd, "Fuck you \n", 13);
    close(sockfd);
}

int main(){
    InetAddress listenAddr(9981);
    InetAddress listenAddr_2(9982);
    PNEventLoop loop;
    PNAcceptor acceptor(&loop, listenAddr);
    PNAcceptor acceptor_2(&loop, listenAddr_2);
    acceptor.setNewConnectionCallback(timeConncetion);
    acceptor_2.setNewConnectionCallback(newConnection_2);
    acceptor_2.listen();
    acceptor.listen();
    loop.loop();
}
