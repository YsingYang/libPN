#include "../PNEventLoop.h"
#include "../PNEventLoopThread.h"
#include <stdio.h>

void runInThread(){
   printf("runInThread(): pid = %d, tid = %ld\n",getpid(), std::this_thread::get_id());
}

int main(){
    printf("mainThread(): pid = %d, tid = %ld\n",getpid(), std::this_thread::get_id());
    PNEventLoopThread  loopThread;
    PNEventLoop *loop = loopThread.startLoop();
    loop->runInLoop(runInThread);
    sleep(1);
    loop->runAfter(2, runInThread);
    sleep(3);
    loop->quit();
}

