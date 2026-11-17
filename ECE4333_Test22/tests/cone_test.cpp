// Host test of TargetFollow.h arrival: near loss, far loss, width arrival, and the search after the next target (N220).
// Frames are given 400 ms after start: the camera (pan servo) must have settled straight ahead first.
#include <cstdio>
#include <cstdint>
#include <cassert>
#include "TargetFollow.h"
int main(){
 // Case 1: cone approaches, width grows past 110 pm, then leaves the image (lost, no ultrasonic) -> ARRIVED
 {TargetFollow f;f.state=TargetFollow::IDLE;f.start(0,0);f.fov=62;uint32_t t=400;int L,R;
  f.measure(1,true,true,500,120,0,t);
  for(;t<2400;t+=10){f.step(0,0,t,-1,L,R);} // seen at 120 pm
  // no more frames: lost after 1.5 s
  for(;t<4400;t+=10){f.step(0,0,t,-1,L,R);}
  printf("near-loss state=%d (expect 2 ARRIVED)\n",f.state); assert(f.state==TargetFollow::ARRIVED);}
 // Case 2: far target lost -> HOLD, not ARRIVED
 {TargetFollow f;f.start(0,0);f.fov=62;uint32_t t=400;int L,R;
  f.measure(1,true,true,500,60,0,t);
  for(;t<2400;t+=10)f.step(0,0,t,-1,L,R);
  for(;t<2900;t+=10)f.step(0,0,t,-1,L,R);
  printf("far-loss state=%d (expect 3 HOLD)\n",f.state); assert(f.state==TargetFollow::HOLD);}
 // Case 3: width 188 pm arrival (centred) -> ARRIVED and next() starts the camera search (car still)
 {TargetFollow f;f.start(0,0);f.fov=62;uint32_t t=400;int L,R;
  f.measure(1,true,true,500,190,0,t);
  for(;t<1400;t+=10)f.step(0,0,t,-1,L,R);
  printf("width-arrival state=%d (expect 2)\n",f.state); assert(f.state==TargetFollow::ARRIVED);
  f.next(0,t); printf("after next state=%d (expect 5 LOOK)\n",f.state); assert(f.state==TargetFollow::LOOK&&f.pan==0);
  f.step(0,0,t+10,-1,L,R);assert(L==0&&R==0);}
 // Case 4: a frame captured before the camera settled after start is ignored
 {TargetFollow f;f.start(0,0);f.fov=62;int L,R;
  f.measure(1,true,true,500,100,0,100);f.step(0,0,110,-1,L,R);
  printf("unsettled frame state=%d (expect 5 LOOK)\n",f.state); assert(f.state==TargetFollow::LOOK);}
 puts("PASS");
}
