#include "../PNEventLoop.h"
#include <thread>

PNEventLoop* g_loop;

using namespace std;

void print(){
    g_loop->quit();
}

void threadFunc(){
    printf("thread tid %ld\n", std::this_thread::get_id());
    g_loop->runAfter(1.0, print);
}

int main(){
    PNEventLoop loop;
    g_loop = &loop;
    printf("main thread tid %ld\n", std::this_thread::get_id());
    std::thread t(threadFunc);
    loop.loop();
    t.join();

}
