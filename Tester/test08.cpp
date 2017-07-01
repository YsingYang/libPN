#include "../PNTCPServer.h"
#include "../PNEventLoop.h"
#include "../PNTCPConnection.h"
#include "../InetAddress.h"
#include <stdio.h>

void onConnection(const TCPConnectionPtr& conn){
    if(conn->getConnected()){
        printf("onConnection(): new connection [%s] from %s\n",
           conn->getName().c_str(),
           conn->getPeerAddr().getFormatString().c_str());
    }
    else{
        printf("onConnection(): connection [%s] is down\n",
           conn->getName().c_str());
    }
}

void onMessage(const TCPConnectionPtr& conn, const char* data, ssize_t len){
    printf("onMessage(): received %zd bytes from connection [%s]\n",len, conn->getName().c_str());
}

int main(){
    InetAddress listenAddr(9982);
    PNEventLoop loop;
    PNTCPServer server(&loop, listenAddr);
    server.setConnectionCallback(onConnection);
    server.setMessageCallback(onMessage);
    server.start();

    loop.loop();
}
