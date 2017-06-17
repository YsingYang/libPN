#ifndef PNTIMERID_H
#define PNTIMERID_H

class PNTimer;

class PNTimerID{
public:
        PNTimerID() : timer_(nullptr), sequence_(0){}
        PNTimerID(PNTimer* timer, int64_t seq): timer, sequence_(seq){}
        PNTimerID(const PNTimerID& rhs) = default;
        PNTimerID& operator(const PNTimerID& rhs) = default;

        friend class TimerQueue;//muduo中这里定义成了TImerQueue的友元,
private:
    PNTimer* timer_;
    int64_t sequence_;
};



#endif // PNTIMER_H
