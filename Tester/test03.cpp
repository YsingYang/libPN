#include "../PNEvent.h"
#include "../PNEventLoop.h"

#include <stdio.h>
#include <sys/timerfd.h>
#include <../PNTimeStamp.h>

PNEventLoop *loop = nullptr;

void timeout(PNTimestamp receiveTime){
    printf("Timeout! %s \n", receiveTime.toFormattedString().c_str());
    loop->quit();
}

int main(){
    PNEventLoop loop_;
    loop = &loop_;

    int timerfd = ::timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK | TFD_CLOEXEC);
    PNEvent event(&loop_, timerfd);//创建一个event加入到loop中
    event.setReadCallback(timeout); //设置超时函数
    event.enableReading();

    struct itimerspec howlong;
    bzero(&howlong, sizeof(howlong));
    howlong.it_value.tv_sec = 5; //5秒超时
    ::timerfd_settime(timerfd, 0, &howlong, NULL);
    loop_.loop();
    close(timerfd);
}
