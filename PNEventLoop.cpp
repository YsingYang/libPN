#include "PNEventLoop.h"

PNEventLoop::activeEventLength = 16;

PNEventLoop::PNEventLoop():
    thisThreadID_(std::this_thread::get_id()),
    running_(false),
    epoller_(std::shared_ptr(new std::shared_ptr<PNEpoll>())),
    {

}


void PNEventLoop::addEvent(PNEvent * event){

}

