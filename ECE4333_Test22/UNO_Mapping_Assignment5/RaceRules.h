#pragma once
#include <stdint.h>
// STOP-sign rule only (Assignment 5 UNO). Pure logic, no Arduino calls.
//
// The figure-8 has a STOP sign before O on segments E-O (2) and C-O (6). The ESP32 reports the sign's
// size in the camera image (N204 R level), and the sketch also knows where the map puts the stop line.
//   GO   -> SLOW : the sign is seen (level 1): drive at slowPct % of the speed
//   GO/SLOW -> HOLD : the sign is right in front (level 2), or the map says the stop line is reached
//   HOLD : stand still for STOP_HOLD_MS (3 s), then DONE. The STOP-sign AI check is requested at the same time.
//   DONE : this stop is finished; the car drives on until it leaves the segment.
// Nothing else (traffic light, intersection scan, overtaking) is handled here.
struct RaceRules{
 enum State:uint8_t{GO=0,SLOW=1,HOLD=2,DONE=3};
 static const uint32_t STOP_HOLD_MS=3000; // hold time after the stop starts
 static const uint8_t STOP_WIN=25;        // a big sign only stops the car within this distance of the map stop line
 State st=GO;uint8_t zone=0,segSeen=255;bool enabled=true,changed=false,seqSeen=false,aiAns=false;
 uint32_t at=0;uint16_t req=0,seq=0;uint8_t stops=0,stopsAI=0;
 float toO=999;uint8_t lineCm=0; // map position (cm before O) and map stop line, set every step
 void reset(){st=GO;seqSeen=false;segSeen=255;changed=true;}
 void enter(State s,uint32_t now){st=s;at=now;changed=true;}
 bool stopped()const{return st==HOLD;}
 bool slow()const{return st==SLOW;}
 // TS code for the ESP32 and the page: 2 track, 3 slowing for a STOP sign, 4 STOP hold
 uint8_t ts()const{return st==HOLD?4:(st==SLOW?3:2);}
 // the follower moved to another segment: a pending approach or a finished stop belongs to the old segment
 void segment(uint8_t s){if(s!=segSeen){segSeen=s;if(st==DONE||st==SLOW){st=GO;changed=true;}}}
 void setZone(uint8_t z){if(z!=zone){zone=z;changed=true;}}
 void hold(uint32_t now){enter(HOLD,now);req++;aiAns=false;if(stops<255)stops++;}
 // N204 colour frame: level 1 = STOP sign seen, 2 = STOP sign right in front. allow = line follower on a straight
 void colour(uint16_t s,bool valid,uint8_t level,bool green,bool allow,uint32_t now){
  (void)green;
  if(seqSeen&&s==seq)return;seq=s;seqSeen=true;
  if(!valid||!enabled||!allow||zone!=2)return;
  if(st==GO||st==SLOW){
   if(level>=2&&(!lineCm||toO<=lineCm+STOP_WIN))hold(now);
   else if(level&&st==GO){st=SLOW;changed=true;}
  }
 }
 // map fallback: never pass the stop line of a STOP-sign approach without stopping
 void stopLine(float o,uint8_t l,uint32_t now){toO=o;lineCm=l;if(enabled&&l&&zone==2&&(st==GO||st==SLOW)&&o<=l)hold(now);}
 void captured(uint16_t id,uint32_t now){(void)id;(void)now;} // the ESP32 took the AI frame: nothing to do here
 void ai(uint16_t id,bool ok,bool positive,uint32_t now){
  (void)now;
  if(id==req&&ok&&!aiAns){aiAns=true;if(positive&&stopsAI<255)stopsAI++;}
 }
 // called every step; the hold ends after STOP_HOLD_MS
 void tick(uint32_t now,int range,bool fresh){
  (void)range;(void)fresh;
  if(st==HOLD&&uint32_t(now-at)>=STOP_HOLD_MS)enter(DONE,now);
 }
};
