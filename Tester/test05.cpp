#include "../PNEventLoop.h"
#include <iostream>
#include <stdio.h>
#include <thread>

PNEventLoop* g_loop;
int flag = 0;

using namespace std;
void run4(){
    printf("run4(): tid = %ld, flag = %d\n", std::this_thread::get_id(), flag);
    g_loop->quit();
}

void run3(){
    printf("run3(): tid = %ld, flag = %d\n", std::this_thread::get_id(), flag);
    g_loop->runAfter(3, run4);
    flag = 3;
}

void run2(){
    printf("run2(): pid = %ld, flag = %d\n", std::this_thread::get_id(), flag);
    g_loop->queueInLoop(run3);
}

void run1(){
    flag = 1;
    printf("run1(): pid = %ld, flag = %d\n", std::this_thread::get_id(), flag);
    g_loop->runInLoop(run2);
    flag = 2;
}

void multiRun1(){
    flag = 1;
    printf("run1.1(): pid = %ld, flag = %d\n", std::this_thread::get_id(), flag);
}

int main(){
    printf("main(): pid = %ld, flag = %d\n",std::this_thread::get_id() , flag);
    PNEventLoop loop;
    g_loop = &loop;
    loop.runAfter(2, run1);
    std::thread t(run1); //调用这个后, 该线程不会自己执行runInLoop里的回调, 它会放入pendingFunctor中, 给主线程回调
    loop.loop();
    printf("main(): pid = %ld, flag = %d\n", std::this_thread::get_id, flag);
    t.join();
}

