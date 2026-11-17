#pragma once
#include <stdint.h>
#include <math.h>
#include <stdlib.h>
#include "MapConfig.h"
#include "HeadingPD.h"
// Vision-based target following (Assignment 3, part 5): nested control.
//
//  OUTER LOOP (camera, every FOLLOW_OUTER_MS = 0.5 s): the ESP32 reports where the colour
//    target is in the image (N215: centre x and width in per mille of the image, age of the
//    frame). The outer loop turns that into
//      bearing  = (0.5 - x/1000) * FOV           (deg, + = target is LEFT of the camera axis)
//      setYaw   = gyro heading when the frame was captured + bearing     (heading set point)
//    and into a forward speed from the apparent width (bigger = closer): full speed far
//    away, slowing down towards the stop width, stopped when it is reached.
//  INNER LOOP (every loop pass, >100 Hz): HeadingPD on the gyro-fused heading
//    (err = setYaw - yaw, rate = gyro rate) -> wheel differential u; left = v-u, right = v+u.
//
// Pure logic (no Arduino calls) so tests/follow_test.cpp can simulate it on a PC.
struct TargetFollow{
 enum State:uint8_t{IDLE=0,TRACK=1,ARRIVED=2,HOLD=3,SWEEP=4};
 // parameters (set by the sketch from the web page)
 int stopW=FOLLOW_STOP_W_PM,fov=FOLLOW_FOV_DEG,speed=60; // fov < 0 = camera image mirrored
 HeadingPD pd;
 // state
 State state=IDLE;
 float setYaw=0;int16_t err10=0,bear10=0;int widthPm=0;
 uint32_t outerAt=0,seenAt=0,lostAt=0;
 // latest camera frame (consumed by the outer loop)
 bool pending=false,pendSeen=false;int pendX=500,pendW=0;float pendYaw=0;uint16_t seq=0;bool seqSeen=false;
 int8_t side=1;float sweptDeg=0;uint32_t lastMs=0;
 // yaw history (0.1 deg, every 50 ms) to look up the heading at the moment a frame was captured
 int16_t hist[8];uint8_t hp=0;uint32_t histAt=0;

 void start(float yaw,uint32_t now){
  state=HOLD;setYaw=yaw;err10=bear10=0;widthPm=0;outerAt=seenAt=lostAt=lastMs=histAt=now; // stand still until the target is seen
  pending=pendSeen=false;seqSeen=false;side=1;sweptDeg=0;ignoreUntil=0;usStop=false;usArm=true;
  for(uint8_t i=0;i<8;i++)hist[i]=(int16_t)(yaw*10);hp=0;
 }
 void stop(){state=IDLE;}
 static float norm180(float a){a=fmodf(a+180.0f,360.0f);if(a<0)a+=360.0f;return a-180.0f;}
 void remember(float yaw,uint32_t now){
  if(uint32_t(now-histAt)<50)return;
  histAt=now;float v=yaw*10;hist[hp++&7]=(int16_t)(v>32000?32000:(v<-32000?-32000:v));
 }
 // N215 from the ESP32. ageMs = how old the frame was when it was sent.
 void measure(uint16_t s,bool valid,bool seen,int x,int w,int ageMs){
  if(seqSeen&&s==seq)return;seq=s;seqSeen=true;
  if(!valid||ageMs<0||ageMs>1200)return;
  int idx=(ageMs+25)/50;if(idx<0)idx=0;if(idx>7)idx=7;
  pendYaw=hist[(hp-1-idx)&7]*0.1f;
  pendSeen=seen;pendX=x;pendW=w;pending=true;
 }
 // Inner loop + outer loop scheduler. yaw/rate come from the gyro-fused pose.
 // Test18: next cone of the sequence (ESP32 N220): forget the old target and search right away, starting to the
 // side the last target was on; frames of the old colour that are still in flight are ignored for 400 ms.
 uint32_t ignoreUntil=0;bool usStop=false,usArm=true; // usArm: the ultrasonic saw open space since the last cone
 void next(float yaw,uint32_t now){if(state==IDLE)return;state=SWEEP;setYaw=yaw;sweptDeg=0;pending=false;usStop=false;usArm=false;seenAt=now-FOLLOW_LOST_HOLD_MS-1;ignoreUntil=now+400;widthPm=0;}
 void step(float yaw,float rate,uint32_t now,int range,int &left,int &right){
  left=right=0;
  if(state==IDLE||state==ARRIVED){lastMs=now;return;} // arrival is latched until next() / start()
  uint32_t dt=now-lastMs;lastMs=now;if(dt>200)dt=200;
  remember(yaw,now);
  // ---- outer loop ----
  if(pending&&int32_t(now-ignoreUntil)<0)pending=false;
  if(uint32_t(now-outerAt)>=FOLLOW_OUTER_MS&&pending){
   outerAt=now;pending=false;
   if(pendSeen){
    float b=(0.5f-pendX*0.001f)*fov;       // deg, + = left (camera not mirrored)
    setYaw=pendYaw+b;bear10=(int16_t)(b*10);widthPm=pendW;
    side=b>=0?1:-1;seenAt=now;sweptDeg=0;
    if(state==HOLD||state==SWEEP)state=TRACK;
   }
  }
  uint32_t lost=uint32_t(now-seenAt);
  // ---- state ----
  if(state==TRACK||state==ARRIVED){
   if(widthPm>=stopW&&abs(bear10)<=150)state=ARRIVED; // Test21: centred width arrival is latched until next target
   // Test18: stop BEFORE touching the cone: the ultrasonic (straight ahead) ends the approach at FOLLOW_STOP_CM,
   // ARRIVED stays latched; the next cone must see a valid far echo before ultrasonic arrival is re-armed
   if(range>FOLLOW_STOP_CM+10){usStop=false;usArm=true;}   // the cone just reached no longer in front
   else if(usArm&&range>0&&range<=FOLLOW_STOP_CM&&lost<=700&&abs(bear10)<=150)usStop=true;
   if(usStop)state=ARRIVED;
   // Test22: a cone close enough to fill the image drops out of the colour detector (clipped / too large) before
   // the width or ultrasonic test passes: that is the arrival, not a loss. Latched like the width arrival.
   if(state!=ARRIVED&&lost>FOLLOW_LOST_HOLD_MS&&!usStop){
    if(widthPm>=FOLLOW_NEAR_PM)state=ARRIVED;
    else{state=HOLD;lostAt=now;}
   }
  }else if(state==HOLD&&lost>FOLLOW_SWEEP_AFTER_MS&&sweptDeg<FOLLOW_SWEEP_MAX_DEG)state=SWEEP;
  if(state==SWEEP){ // look for the target: the set point slides towards the side it was last seen on
   float d=FOLLOW_SWEEP_DPS*dt*0.001f;setYaw+=side*d;sweptDeg+=d;
   if(sweptDeg>=FOLLOW_SWEEP_MAX_DEG)state=HOLD;
  }
  // ---- inner loop ----
  float err=norm180(setYaw-yaw);err10=(int16_t)(err*10);
  float v=0;
  float ae=err<0?-err:err;
  if(state==TRACK&&ae<20){                   // larger heading errors: turn on the spot first
   float f=(stopW-widthPm)/(stopW*0.5f);if(f>1)f=1;if(f<0)f=0;
   float lo=speed<FOLLOW_MIN_PWM?speed:FOLLOW_MIN_PWM;
   v=lo+(speed-lo)*f;                       // far: full speed, near: slow
   if(lost>300)v=lo;                        // target momentarily missing: creep on the old heading
   else v*=1.0f-0.5f*ae/20;                 // slow down while the heading error is large
   if(v<lo)v=lo;                            // below this the motors stall
  }
  if(state==HOLD||state==ARRIVED)left=right=0;else pd.steer(err,rate,v,FOLLOW_PIVOT_MIN_PWM,left,right);
 }
};
