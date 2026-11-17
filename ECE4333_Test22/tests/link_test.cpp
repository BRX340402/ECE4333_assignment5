// End-to-end serial link test: the real UNO sketch (built on a PC with Arduino stubs) receives the exact
// commands the ESP32 builds (/traffic-start, /follow-start, settings N206, STOP N100) and its replies are checked.
// Build/run with run_all.sh (it swaps PoseEstimate.h / KeyIrLed.h for the stubs).
#include <cstdio>
#include <cassert>
#include <string>
#include <vector>
#include "Arduino.h"
uint32_t fakeNow=0;int fakeRange=-1,fakeFloor[3]={100,900,100},fakePins[32];FakeSerial Serial;
#include "UNO_Mapping_Assignment5.ino"

static int fails=0;
#define CHECK(c,msg) do{if(c)printf("PASS %s\n",msg);else{printf("FAIL %s\n",msg);fails++;}}while(0)

static void run(uint32_t ms){for(uint32_t t=0;t<ms;t+=5){fakeNow+=5;loop();}}
static void send(const std::string&j){Serial.in+=j;run(60);}
static bool replied(const char*s){return Serial.out.find(s)!=std::string::npos;}

// --- the ESP32 builders, copied from ESP32_Assignment4.ino ---
struct Opt{const char*n;int lo,hi,v;};
static std::string trafficStart(int so,int slow,int line,int ovt,int trig,int out=15,int pass=10,int ret=15,int ang=90,int right=1){
 Opt o[]={{"so",20,150,so},{"slow",20,100,slow},{"line",0,60,line},{"ovt",0,1,ovt},{"trig",15,120,trig},
  {"out",5,60,out},{"pass",5,150,pass},{"ret",5,60,ret},{"ang",30,90,ang},{"right",0,1,right}};
 const int green=0;
 std::string j=std::string("{\"N\":101,\"D1\":6,\"D2\":")+std::to_string(green)+",\"D3\":0";
 for(int i=0;i<10;i++)j+=",\"D"+std::to_string(i+4)+"\":"+std::to_string(o[i].v);
 return j+"}";
}
static std::string followStart(int w,int fov,int kp,int kd){
 return std::string("{\"N\":101,\"D1\":7,\"D2\":")+std::to_string(w)+",\"D3\":"+std::to_string(fov)+",\"D4\":"+std::to_string(kp)+",\"D5\":"+std::to_string(kd)+"}";
}
static std::string settings(int id){
 char c[150];snprintf(c,sizeof(c),"{\"N\":206,\"ID\":%d,\"SC\":%d,\"BL\":%d,\"BH\":%d,\"RA\":%d,\"SA\":%d,\"CT\":%d,\"GS\":%d,\"CS\":%d,\"RS\":%d,\"DS\":%d}",id,25,600,1023,180,360,1500,0,60,0,40);
 return c;
}

int main(){
 setup();run(500);
 CHECK(mode==0,"power-on: standby (only the web Start starts a run)");
 Serial.out.clear();send(settings(7));run(100);
 CHECK(settingsReady&&replied("\"N\":222,\"ID\":7,\"OK\":1"),"settings N206 accepted, N222 acknowledgement sent");

 std::string s=trafficStart(150,100,60,1,120,60,150,60,90,1); // widest values: buffer check
 CHECK(s.size()<sizeof(input)-1,"longest N101 fits the UNO input buffer");
 s=trafficStart(83,50,20,1,15);
 printf("  /traffic-start -> %s (%u chars, buffer %u)\n",s.c_str(),(unsigned)s.size(),(unsigned)sizeof(input));
 CHECK(s.size()<sizeof(input)-1,"N101 fits the UNO input buffer");
 Serial.out.clear();send(s);
 CHECK(mode==6&&replied("\"N\":211,\"OK\":1"),"figure-8 start accepted (mode 6, N211 OK)");
 CHECK(ovt.enabled&&ovt.detectCm==15&&ovt.outCm==15&&ovt.passCm==10&&ovt.retCm==15&&ovt.angleDeg==90&&ovt.firstRight==1,
       "web defaults reach the UNO (detect 15, right 90, legs 15 / 10 / 15)");
 CHECK(follower.startOff==83&&slowPct==50&&stopLine==20,"start offset / STOP slow % / stop line reach the UNO");
 run(1500);
 CHECK(replied("\"N\":221,")&&replied("\"UV\":51"),"N221 status telemetry while driving (UV=51)");
 CHECK(replied("\"OV\":1,\"OT\":15,\"OS\":15,\"OL\":10,\"OR\":15,\"OA\":90,\"OD\":1,"),"N221 echoes the avoidance settings the car uses (page: Car is using)");
 {size_t p=Serial.out.rfind("{\"N\":230,\"d\":[");CHECK(p!=std::string::npos,"N230 samples while driving");
  if(p!=std::string::npos){size_t e=Serial.out.find(']',p);std::string d=Serial.out.substr(p+14,e-p-14);int n=1;for(char c:d)if(c==',')n++;
   printf("  N230 = %s\n",d.c_str());CHECK(n==19,"N230 still has 19 fields (web page format unchanged)");}}
 Serial.out.clear();send("{\"N\":100}");
 CHECK(mode==0&&leftPWM==0&&rightPWM==0&&!ovt.active(),"STOP CAR (N100): standby, motors off");

 Serial.out.clear();send(trafficStart(83,50,20,0,70,35));
 CHECK(mode==6&&!ovt.enabled,"avoidance OFF from the web check box");
 send("{\"N\":100}");
 Serial.out.clear();send(trafficStart(83,50,20,1,40,30,80,25,45,0));
 CHECK(mode==6&&ovt.detectCm==40&&ovt.outCm==30&&ovt.passCm==80&&ovt.retCm==25&&ovt.angleDeg==45&&ovt.firstRight==0,"changed web values reach the UNO (40 cm, left 45, 30 / 80 / 25)");
 send("{\"N\":100}");
 // widest N221 (every field at its widest) must fit the 300-byte transmit buffer
 Serial.out.clear();send(trafficStart(150,100,60,1,120,60,150,60,90,1));
 {mode=6;leftPWM=-180;rightPWM=-180;rangeCm=-1;runNumber=65535;zeroRevision=255;cruise=180;battV10=255;traffic.req=65535;traffic.stops=255;traffic.stopsAI=255;
  lineL=lineM=lineR=1023;ovt.state=Overtake::JOIN_TURN;uint32_t keepNow=fakeNow;
  int n=linkFmt(tx,sizeof(tx),PSTR("{\"N\":221,\"M\":%u,\"TS\":%u,\"ID\":%u,\"L\":%d,\"C\":%d,\"R\":%d,\"GY\":%u,\"UV\":%d,\"UT\":%lu,\"FC\":%u,\"PL\":%d,\"PR\":%d,\"ES\":%u,\"RN\":%u,\"GN\":%u,\"ZR\":%u,\"IM\":%u,\"CB\":%u,\"GL\":%d,\"GF\":%u,\"SG\":%u,\"LP\":%u,\"OV\":%u,\"OT\":%d,\"OS\":%d,\"OL\":%d,\"OR\":%d,\"OA\":%d,\"OD\":%u,\"Z\":%u,\"CV\":%d,\"SN\":%u,\"SA\":%u,\"BV\":%u,\"A5\":%u,\"US\":%d,\"BR\":%u}\n"),
   15u,15u,65535u,1023,1023,1023,1u,51,4000000000UL,255u,-180,-180,255u,65535u,1u,255u,255u,1u,-1,255u,7u,255u,8u,120,60,150,60,90,1u,3u,180,255u,255u,255u,1u,-1,255u);
  printf("  widest N221: %d bytes (buffer %u)\n",n,(unsigned)sizeof(tx));CHECK(n>0&&n<(int)sizeof(tx)&&n<352,"widest N221 fits the UNO transmit buffer and the ESP32 receive buffer (352)");
  mode=0;leftPWM=rightPWM=0;ovt.reset();fakeNow=keepNow;runNumber=1;}
 send("{\"N\":100}");

 Serial.out.clear();send(followStart(188,62,15,10));
 CHECK(mode==7&&follow.stopW==188&&replied("\"N\":211,\"OK\":1"),"target following start (mode 7, stop width 188 = 75 px)");
 send("{\"N\":215,\"S\":1,\"V\":1,\"T\":1,\"X\":500,\"W\":120,\"A\":40}");run(700);
 CHECK(follow.state==TargetFollow::TRACK,"colour frame N215 drives the follower (TRACK)");
 send("{\"N\":100}");
 CHECK(mode==0,"STOP CAR ends target following");

 Serial.out.clear();send("{\"N\":102,\"D1\":1,\"D2\":100}");
 CHECK(mode==0&&leftPWM==0&&replied("\"N\":211,\"OK\":0"),"old manual drive command N102 is refused (manual driving removed)");
 Serial.out.clear();send("{\"N\":101,\"D1\":6,\"D4\":10}");
 CHECK(mode==0&&replied("\"N\":211,\"OK\":0"),"out-of-range start offset refused");

 printf(fails?"LINK TESTS FAILED: %d\n":"ALL LINK TESTS PASS\n",fails);
 return fails?1:0;
}
