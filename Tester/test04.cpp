#include "../PNEventLoop.h"
#include <stdio.h>
#include <thread>

int cnt = 0;
PNEventLoop* g_loop;

void printTid(){
    printf("pid = %d, tid = %ld \n", getpid(), std::this_thread::get_id());
    printf("now %s\n", PNTimestamp::now().toFormattedString().c_str());
}

void print(const char* msg){
    printf("msg : %s %s \n", PNTimestamp::now().toString().c_str(), msg);
    if(++cnt == 10){
        g_loop->quit();//当cnt == 20时,停止循环
    }
}

int main(){
    printTid();
    PNEventLoop loop;
    g_loop = &loop;

    loop.runAfter(1, std::bind(print, "once1"));
    loop.runAfter(1.5, std::bind(print, "once1.5"));
    loop.runAfter(2.5, std::bind(print, "once2.5"));
    loop.runAfter(3.5, std::bind(print, "once3.5"));
    loop.runEvery(2, std::bind(print, "every2"));
    loop.runEvery(3, std::bind(print, "every3"));

    loop.loop();
    printf("finish \n");
    sleep(1);
    return 0;
}
