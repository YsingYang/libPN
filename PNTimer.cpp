#include "PNTimer.h"

void PNTimer::run() const{
    callback_();
}

void PNTimer::restart(PNTimestamp now){
    if(!repeat_)
        return ;
    expiration_ = addTime(now, interval_); //继续加入定时器

}
