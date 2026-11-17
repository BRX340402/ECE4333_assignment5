#pragma once
#include <stdint.h>
#include <math.h>
#include "HeadingPD.h"
// Obstacle avoidance without scanning (Test22). Every distance and angle is a web setting (defaults in brackets):
//   IDLE      line following. The first ultrasonic reading within detectCm [15] stops the car; a second one confirms
//             the obstacle (a single noisy echo only pauses the car for one reading)
//   OUT_TURN  pivot by angleDeg [90] to the first-turn side [right]
//   OUT_DRIVE drive outCm [15]
//   ALIGN     pivot back to the road heading H
//   PASS      drive passCm [10] (the obstacle length) parallel to the road
//   RET_TURN  pivot by angleDeg towards the road (the other side: left after a right first turn)
//   RET_DRIVE drive retCm [15] back to the road. Ends early once a line sensor sees the road after half of it; if the
//             road is not seen at retCm it keeps going up to RET_EXTRA_CM further. After the road is seen the car
//             drives on until the axle (not only the sensors) is over it.
//   JOIN_TURN pivot back to H, then the line follower resumes
// Odd states pivot, even states drive; the next state is always state + 1.
// A step that cannot finish (pivot stuck, blocked) ends the detour: the line follower takes over (its search finds the line).
// Pure logic (no Arduino calls) so tests/overtake_test.cpp can run it on a PC.
struct Overtake{
 enum State:uint8_t{IDLE=0,OUT_TURN=1,OUT_DRIVE=2,ALIGN=3,PASS=4,RET_TURN=5,RET_DRIVE=6,JOIN_TURN=7};
 static const int SENSOR_AHEAD_CM=7,RET_EXTRA_CM=15,DRIVE_PWM=60,PIVOT_MIN=55,BLOCK_CM=8;
 static const uint16_t TURN_MS=3500,MS_PER_CM=90,DRIVE_EXTRA_MS=3000; // drive time limit: about 1.8 x the time the distance needs
 // web settings
 bool enabled=false;int detectCm=15,outCm=15,passCm=10,retCm=15,angleDeg=90;int8_t firstRight=1;
 State state=IDLE;bool cool=false,seen=false,holding=false;int8_t side=-1;
 float H=0,tgt=0,d0=0,need=0,extraCm=0;uint32_t at=0;uint16_t ok=0,fails=0;
 HeadingPD pd;
 void reset(){state=IDLE;cool=false;holding=false;}
 bool active()const{return state!=IDLE;}
 static float norm(float a){while(a>180)a-=360;while(a<-180)a+=360;return a;}
 void finish(bool good){state=IDLE;cool=true;holding=false;if(good)ok++;else fails++;}
 // pivot states: heading to turn to; drive states: distance to drive
 void enter(uint8_t s,float dist,uint32_t now){
  state=(State)s;at=now;d0=dist;seen=false;
  if(s&1)tgt=H+side*angleDeg*((s==OUT_TURN)-(s==RET_TURN));
  else need=s==OUT_DRIVE?outCm:(s==PASS?passCm:retCm);
 }
 // Returns true while Overtake drives the wheels (l, r); false = the line follower drives.
 //  fresh: new ultrasonic reading this pass; following: the line follower is on the line (follow / crossing);
 //  lineYaw: its road heading; line: a line sensor sees black
 bool step(float yaw,float rate,float dist,int range,bool fresh,bool following,float lineYaw,bool line,uint32_t now,int &l,int &r){
  l=r=0;
  if(state==IDLE){
   bool hit=range>0&&range<=detectCm;
   if(fresh&&!hit){if(range<0||range>detectCm+10)cool=false;holding=false;}
   if(!enabled||cool||!following)return false;
   if(fresh&&hit){
    if(!holding){holding=true;return true;}                    // first reading: stop and look again
    holding=false;H=lineYaw;side=firstRight?-1:1;extraCm=0;enter(OUT_TURN,dist,now);  // confirmed: start the detour
   }
   else return holding;                                          // waiting for the next reading: stand still
  }
  float e=norm(tgt-yaw),ae=e<0?-e:e;uint32_t t=now-at;
  if(state&1){ // pivot
   bool aligned=ae<6&&rate<30&&rate>-30;
   if(state==JOIN_TURN&&(aligned||t>=TURN_MS)){finish(true);return false;} // a late join: the line follower's search finds the line
   if(aligned){enter(state+1,dist,now);return true;}
   if(t>=TURN_MS){finish(false);return false;}
   if(ae>=5)pd.steer(e,rate,0,PIVOT_MIN,l,r);   // inside 5 deg: wheels off, let the turn settle (no hunting)
   return true;
  }
  if(t>=DRIVE_EXTRA_MS+(uint32_t)need*MS_PER_CM){finish(false);return false;}
  if(range>=0&&range<BLOCK_CM)return true;   // something right ahead: wait (gives up when the time limit runs out)
  float run=dist-d0;
  if(state==RET_DRIVE){
   if(!seen&&line&&run>=retCm*0.5f){seen=true;need=run+SENSOR_AHEAD_CM*sinf(angleDeg*0.0174533f);} // road found: axle over it
   else if(!seen&&run>=need&&need<retCm+RET_EXTRA_CM)need=retCm+RET_EXTRA_CM;                       // not yet: a little further
  }
  if(run>=need){
   if(state!=PASS)extraCm+=run*(1-cosf(angleDeg*0.0174533f)); // side steps are not progress along the road
   enter(state+1,dist,now);return true;
  }
  pd.steer(e,rate,DRIVE_PWM,PIVOT_MIN,l,r);return true;
 }
};
