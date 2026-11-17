#include <cstdio>
#include <cstdint>
#include <cassert>
#include "TargetFollow.h"
int main(){
 // Case 1: cone approaches, width grows past 110 pm, then leaves the image (lost, no ultrasonic) -> ARRIVED
 {TargetFollow f;f.state=TargetFollow::IDLE;f.start(0,0);f.fov=62;uint32_t t=0;int L,R;
  f.measure(1,true,true,500,120,0);
  for(;t<2000;t+=10){f.step(0,0,t,-1,L,R);} // seen at 120 pm
  // no more frames: lost after 1.5 s
  for(;t<4000;t+=10){f.step(0,0,t,-1,L,R);}
  printf("near-loss state=%d (expect 2 ARRIVED)\n",f.state); assert(f.state==TargetFollow::ARRIVED);}
 // Case 2: far target lost -> HOLD, not ARRIVED
 {TargetFollow f;f.start(0,0);f.fov=62;uint32_t t=0;int L,R;
  f.measure(1,true,true,500,60,0);
  for(;t<2000;t+=10)f.step(0,0,t,-1,L,R);
  for(;t<2500;t+=10)f.step(0,0,t,-1,L,R);
  printf("far-loss state=%d (expect 3 HOLD)\n",f.state); assert(f.state==TargetFollow::HOLD);}
 // Case 3: width 188 pm arrival (centred) -> ARRIVED and next() re-arms search
 {TargetFollow f;f.start(0,0);f.fov=62;uint32_t t=0;int L,R;
  f.measure(1,true,true,500,190,0);
  for(;t<1000;t+=10)f.step(0,0,t,-1,L,R);
  printf("width-arrival state=%d (expect 2)\n",f.state); assert(f.state==TargetFollow::ARRIVED);
  f.next(0,t); printf("after next state=%d (expect 4 SWEEP)\n",f.state); assert(f.state==TargetFollow::SWEEP);}
 puts("PASS");
}
