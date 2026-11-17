// Host simulation of TargetFollow.h search: camera pan first (car still), then a slow car turn.
// Car: differential drive, PWM * 0.33 cm/s, track 14 cm, motor lag 0.12 s, gyro = true rate. Camera: 62 deg field of view
// on the pan servo (servo angle follows the command after 250 ms), a frame every 200 ms delivered 150 ms after capture.
// Targets: 8 cm wide cones. Ultrasonic straight ahead (+-12 deg) only when the camera looks straight ahead.
// Build: g++ -std=c++11 -I stubs -I ../UNO_Mapping_Assignment5 follow_search_test.cpp -o fs && ./fs
#include <cstdio>
#include <cmath>
#include <vector>
#include "TargetFollow.h"

static const float K=0.33f,TRACK=14,TAU=0.12f,DT=0.01f,RAD=3.14159265f/180,FOV=62,CONE=8;
static int fails=0;
#define CHECK(c,...) do{if(c){printf("PASS ");printf(__VA_ARGS__);printf("\n");}else{printf("FAIL ");printf(__VA_ARGS__);printf("\n");fails++;}}while(0)
struct Cone{float x,y;};
struct Frame{uint32_t deliver;uint16_t seq;bool seen;int x,w;uint32_t captured;};
struct Res{bool arrived;int reached;float yaw;std::vector<int> pans;int firstSeenPan;float sweptAtFind;uint32_t t;}; // sweptAtFind: car search turn before the target was found

// targets in degrees / cm from the car (car at the origin facing +x, + = left)
static Res run(std::vector<Cone> cones,uint32_t limitMs=60000){
 float x=0,y=0,th=0,vl=0,vr=0;TargetFollow f;f.speed=60;f.stopW=188;f.fov=62;
 uint32_t now=0;f.start(0,now);
 std::vector<Frame> q;uint16_t seq=0;float servo=0;int8_t servoCmd=0;uint32_t servoCmdAt=0;
 Res R{};R.firstSeenPan=999;int lastPan=-999;
 for(;now<limitMs;now+=10){
  // servo: reaches the commanded angle 250 ms after the command
  if(f.pan!=servoCmd){servoCmd=f.pan;servoCmdAt=now;}
  if(now-servoCmdAt>=250)servo=servoCmd;
  if(f.pan!=lastPan){R.pans.push_back(f.pan);lastPan=f.pan;}
  // camera frame every 200 ms: the largest cone in view
  if(now%200==0){
   Frame fr{now+150,++seq,false,500,0,now};float best=0;
   for(auto&c:cones){float dx=c.x-x,dy=c.y-y,d=sqrtf(dx*dx+dy*dy);float a=atan2f(dy,dx)/RAD-th/RAD-servo;
    while(a>180)a-=360;while(a<-180)a+=360;float wdeg=2*atanf(CONE/2/d)/RAD;
    if(fabsf(a)<FOV/2&&d>5&&wdeg>best){best=wdeg;fr.seen=true;fr.x=(int)((0.5f-a/FOV)*1000);fr.w=(int)(wdeg/FOV*1000);}}
   q.push_back(fr);}
  for(size_t i=0;i<q.size();)if(q[i].deliver<=now){f.measure(q[i].seq,true,q[i].seen,q[i].x,q[i].w,(int)(now-q[i].captured),now);
   if(q[i].seen&&R.firstSeenPan==999&&f.state==TargetFollow::TRACK)R.firstSeenPan=f.pendPan;q.erase(q.begin()+i);}else i++;
  int range=-1;
  if(f.pan==0&&servo==0&&now-servoCmdAt>=350)for(auto&c:cones){float dx=c.x-x,dy=c.y-y,d=sqrtf(dx*dx+dy*dy),a=atan2f(dy,dx)-th;
   while(a>3.14159f)a-=6.2832f;while(a<-3.14159f)a+=6.2832f;if(fabsf(a)<12*RAD){int r=(int)(d-10);if(range<0||r<range)range=r;}}
  if(f.state==TargetFollow::SWEEP&&f.sweptDeg>R.sweptAtFind)R.sweptAtFind=f.sweptDeg;
  int l,r;f.step(th/RAD,(vr-vl)/TRACK/RAD,now,range,l,r);
  vl+=(l*K-vl)*DT/TAU;vr+=(r*K-vr)*DT/TAU;float v=(vl+vr)/2,w=(vr-vl)/TRACK;th+=w*DT;x+=v*cosf(th)*DT;y+=v*sinf(th)*DT;
  if(f.state==TargetFollow::ARRIVED){R.arrived=true;break;}
 }
 R.yaw=th/RAD;R.t=now;
 // which cone was reached: the nearest one
 float bd=1e9;for(size_t i=0;i<cones.size();i++){float d=hypotf(cones[i].x-x,cones[i].y-y);if(d<bd){bd=d;R.reached=(int)i;}}
 return R;
}
static Cone at(float deg,float cm){return Cone{cm*cosf(deg*RAD),cm*sinf(deg*RAD)};}
static void pans(const Res&R){printf("    camera positions:");for(int p:R.pans)printf(" %d",p);printf("\n");}

int main(){
 TargetFollow t;t.side=1;
 CHECK(t.LOOK_POSITIONS==7&&t.lookAngle(0)==0&&t.lookAngle(1)==20&&t.lookAngle(2)==-20&&t.lookAngle(3)==40&&t.lookAngle(4)==-40&&t.lookAngle(5)==60&&t.lookAngle(6)==-60,
       "camera search order: 0, +20, -20, +40, -40, +60, -60 deg (straight ahead first)");
 {Res R=run({at(0,120)});pans(R);
  CHECK(R.arrived&&R.firstSeenPan==0&&R.pans.size()==1,"target straight ahead: found with the camera straight, no panning, reached (%.1f s)",R.t/1000.0);}
 {Res R=run({at(45,120)});pans(R);
  CHECK(R.arrived&&R.firstSeenPan!=0&&R.firstSeenPan>0&&R.sweptAtFind<1,"target 45 deg left: found by turning the CAMERA (pan %+d), the car did not search-turn",R.firstSeenPan);
  CHECK(R.arrived&&fabsf(R.yaw-45)<15,"... then the car turned to it and reached it (car heading %.0f deg, target at 45)",R.yaw);}
 {Res R=run({at(-70,100)});pans(R);
  CHECK(R.arrived&&R.firstSeenPan<0&&R.sweptAtFind<1,"target 70 deg right: found with the camera at %+d, reached (heading %.0f)",R.firstSeenPan,R.yaw);}
 {Res R=run({at(10,200),at(55,60)});pans(R);
  CHECK(R.arrived&&R.reached==0,"a far target in front and a nearer, bigger one at 55 deg: the one in FRONT is chosen (reached cone %d)",R.reached);}
 {Res R=run({at(170,100)},90000);pans(R);
  CHECK(R.arrived&&R.pans.size()>=7&&R.sweptAtFind>1,"target behind the car: the camera finds nothing, then the car turns slowly (%.0f deg) and finds it, heading %.0f",R.sweptAtFind,R.yaw);}
 {Res R=run({},90000);
  TargetFollow g;g.start(0,0);
  CHECK(!R.arrived&&R.pans.back()==0,"no target at all: the search ends with the camera straight ahead (positions tried %d)",(int)R.pans.size());}
 // frames taken while the servo turns are ignored
 {TargetFollow g;g.start(0,0);g.side=1;uint32_t now=0;int l,r;
  for(;now<5000&&g.pan==0;now+=10)g.step(0,0,now,-1,l,r); // no frames at 0: moves on to +20 after 3 dwell times
  int8_t p=g.pan;uint32_t moved=g.panAt;
  g.measure(500,true,true,500,100,0,moved+100);         // captured 100 ms after the move: servo still turning
  g.step(0,0,moved+110,-1,l,r);
  CHECK(p!=0&&g.state==TargetFollow::LOOK,"a frame captured while the camera turns (pan %+d) is ignored",p);
  g.measure(501,true,true,500,100,0,moved+400);g.step(0,0,moved+410,-1,l,r);
  CHECK(g.state==TargetFollow::TRACK&&fabsf(g.setYaw-p)<1&&g.pan==0,"a settled frame there: target heading = camera angle (%.0f deg), camera back to straight",g.setYaw);}
 printf(fails?"FOLLOW SEARCH TESTS FAILED: %d\n":"ALL FOLLOW SEARCH TESTS PASS\n",fails);
 return fails?1:0;
}
