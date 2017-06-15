#ifndef PNTIMESTAMP_H
#define PNTIMESTAMP_H


#include <sys/time.h>
#include <stdio.h>

#include <boost/operators.hpp>

//封装了一个记录微妙的时间戳
class PNTimestamp : public boost::less_than_comparable<PNTimestamp>{
public:
    PNTimestamp() : microSeconds_(0){}
    explicit PNTimestamp(int64_t microSeconds) : microSeconds_(microSeconds){}

public:
    static const int kMicroSecondsPerSecond = 1000 * 1000;

    static PNTimestamp now();//返回当前时间
    static inline PNTimestamp fromUnixTime(time_t t); //将time_t类型时间转化为内部是unix时间
    static inline PNTimestamp fromUnixTime(time_t t, int microSeconds);

public:

    inline void swap(PNTimestamp& rhs);
    inline bool valid() const;
    inline int64_t getMicroSeconds() const;
    //time_t 定义在private中
    inline time_t getSecond() const;

private:

    int64_t microSeconds_;
};
 //比较函数
 inline bool operator< (const PNTimestamp& lhs,const  PNTimestamp& rhs){//为什么源码中这里不使用引用
    return lhs.getMicroSeconds() <rhs.getMicroSeconds();
 }

 inline bool operator== (const PNTimestamp& lhs,const  PNTimestamp& rhs){
    return  lhs.getMicroSeconds() == rhs.getMicroSeconds();
 }

 inline double timeDifference(const PNTimestamp &high,const PNTimestamp &low){ //返回一个以秒为单位的时间差
    int64_t diff = high.getMicroSeconds() - low.getMicroSeconds();
    return static_cast<double>(diff) / PNTimestamp::kMicroSecondsPerSecond;
 }

inline PNTimestamp addTime(const PNTimestamp &ts, const double &second){
        //先将秒转为微妙
        int64_t delta = static_cast<int64_t>(second * PNTimestamp::kMicroSecondsPerSecond);
        return PNTimestamp(ts.getMicroSeconds() + delta);
}

PNTimestamp PNTimestamp::fromUnixTime(time_t t){
    return fromUnixTime(t, 0);
 }

PNTimestamp PNTimestamp::fromUnixTime(time_t t, int microSeconds){
    return PNTimestamp(static_cast<int64_t>(t) * kMicroSecondsPerSecond);
 }

 void PNTimestamp::swap(PNTimestamp& rhs){
    std::swap(microSeconds_, rhs.microSeconds_);
 }

 bool PNTimestamp::valid() const{
    return microSeconds_ > 0;
 }

 int64_t PNTimestamp::getMicroSeconds() const{
    return microSeconds_;
 }

 time_t PNTimestamp::getSecond() const{
    return static_cast<time_t>(microSeconds_ / kMicroSecondsPerSecond);
 }

///注意muduo之中用到一个BOOST_STATIC_ASSERT(sizeof(Timestamp) == sizeof(int64_t)) 这是一个编译时断言, 我们一般使用的assert是编译时断言

#endif // TIMESTAMP_H
