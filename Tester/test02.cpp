//对实现好的PNEpoll, PNEvent ,PNEventLoop的测试程序

#include "../PNEventLoop.h"
#include "../PNEvent.h"
#include <sys/timerfd.h>
PNEventLoop *g_loop;

void timeout(){
    printf("timeout \n");
    g_loop->quit();
}

int main(){
    PNEventLoop loop;
    g_loop = &loop;
    int timefd = ::timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK | TFD_CLOEXEC);
    PNEvent event(&loop, timefd);
    event.setReadCallback(timeout);
    event.enableReading();

    struct itimerspec howlong;
    bzero(&howlong, sizeof(howlong));
    howlong.it_value.tv_sec = 5;
    ::timerfd_settime(timefd, 0, &howlong, NULL);
    loop.loop();

    ::close(timefd);
}


