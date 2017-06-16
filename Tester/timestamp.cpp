#include "../PNTimeStamp.h"
#include <vector>
#include <stdio.h>
#include <iostream>

using namespace std;

void passByConstReference(const PNTimestamp& x)
{
  printf("%s\n", x.toString().c_str());
}

void passByValue(PNTimestamp x)
{
  printf("%s\n", x.toString().c_str());
}

void benchmark()
{
  const int kNumber = 1000*1000;

  std::vector<PNTimestamp> stamps;
  stamps.reserve(kNumber);
  for (int i = 0; i < kNumber; ++i)
  {
    stamps.push_back(PNTimestamp::now());
  }
  printf("%s\n", stamps.front().toString().c_str());
  printf("%s\n", stamps.back().toString().c_str());
  printf("%f\n", timeDifference(stamps.back(), stamps.front()));

  int increments[100] = { 0 };
  int64_t start = stamps.front().getMicroSeconds();
  for (int i = 1; i < kNumber; ++i)
  {
    int64_t next = stamps[i].getMicroSeconds();
    int64_t inc = next - start;
    start = next;
    if (inc < 0){ //如果next时间 > start时间, 错误
      printf("reverse!\n");
    }
    else if (inc < 100) {  //记录相应的时间差
      ++increments[inc];
    }
    else{
      printf("big gap %d\n", static_cast<int>(inc)); //如果差距>100毫秒, 输出
    }
  }

  for (int i = 0; i < 100; ++i){
    printf("%2d: %d\n", i, increments[i]);
  }
}

int main(){
  PNTimestamp now(PNTimestamp::now());
  printf("%s\n", now.toString().c_str());
  passByValue(now);
  passByConstReference(now);
  std::cout<< now.toFormattedString()<<endl;
  //benchmark();
  return 0;
}
