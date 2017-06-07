#include "../PNEventLoop.h"
#include <iostream>
#include <thread>

using namespace std;

PNEventLoop *loopPtr;

void threadFunc(){
    //printf("threadFunc run id pid %d , tid = %ld; \n", getpid(), this_thread::get_id());
    cout<<this_thread::get_id()<<endl;
    PNEventLoop loop;
    loop.loop();
}

void threadFunc2(){ //反例
    printf("threadFunc run id pid %d , tid = %ld; \n", getpid(), this_thread::get_id());
    loopPtr->loop();
}

int main(){
    printf("main() : pid = %d, tid = %ld \n", getpid(), this_thread::get_id());

    PNEventLoop loop;
    loopPtr = &loop;
    thread t(threadFunc);
    //loop.loop();
    t.join();
    return 0;
}
