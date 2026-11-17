#include <Arduino.h>
// ECE4333 Test22, UV=51. Assignment 5 UNO firmware:
//   initialization (battery LED; the web page Start is the only start), figure-8 line following with the STOP-sign rule (3 s hold),
//   cone / target following (gyro inner loop, camera outer loop, cone sequence).
#include <avr/pgmspace.h>
#include "MapConfig.h"
#include "Hardware.h"
#include "IntegerCommand.h"
#include "PoseEstimate.h"
#include "LineFollower.h"
#include "RoutePolicy.h"
#include "RaceRules.h"
#include "KeyIrLed.h"
#include "LinkTx.h"
#include "TargetFollow.h"
#include "Overtake.h"
#include "PanServo.h"

PoseEstimate pose;LineFollower follower;RaceRules traffic;TargetFollow follow;Overtake ovt;
// mode: 0 standby, 6 figure-8 line following, 7 cone / target following
uint8_t mode=0,blockReason=1;
bool settingsReady=false;int configReply=-1;int8_t pendingReply=-1;
int blackLow=600,blackHigh=1023,cruise=60,echoRS=0,echoDS=40;
uint8_t zeroRevision=0;uint16_t runNumber=0;
// STOP-sign parameters from the start command: slowPct = speed while a STOP sign is ahead, stopLine = map stop line before O (0 off)
uint8_t prevSg=0,slowPct=50,stopLine=20;
uint8_t pingSeq=0,pingSeen=0;
int lineL=0,lineM=0,lineR=0,rangeCm=-1,leftPWM=0,rightPWM=0;
uint32_t motorsOffSince=0;

// ---------- N230 sample ring (sampled on time, sent when the UART is free) ----------
struct Sample{uint32_t t;int32_t yaw10,distMm;uint16_t q;int16_t l,c,r,pl,pr,g10,u,x,y;uint8_t st,k,sm;};
static const uint8_t RING=5;Sample ring[RING];uint8_t ringHead=0,ringCount=0;
uint16_t sampleSeq=0,droppedSamples=0;uint8_t eventMask=0;uint32_t lastSample=0;

bool black(int v){return v>=blackLow&&v<=blackHigh;}
// Hysteresis: become black only clearly inside the black range, become white only clearly outside.
bool blackL=false,blackM=false,blackR=false;
bool hyst(bool was,int v){
 if(blackLow>0){int t=blackLow+(was?-LINE_HYST:LINE_HYST);return v>=t&&v<=blackHigh;} // black = high readings
 int t=blackHigh+(was?LINE_HYST:-LINE_HYST);return v<=t;                              // black = low readings
}
int8_t gyroSignLearned=0;uint8_t gyroSignFlips=0; // auto-corrected gyro direction (kept until power-off)
int usRange(){return panAhead()?rangeCm:-1;} // ultrasonic distance straight ahead (-1 = no echo, or the camera / sensor is turned)

// battery LED state
uint8_t battV10=0;uint32_t ledAt=0;

void motorOutput(int left,int right){
 left=constrain(left,-DRIVE_LIMIT,DRIVE_LIMIT);right=constrain(right,-DRIVE_LIMIT,DRIVE_LIMIT);
 digitalWrite(MOTOR_STBY,(left||right)?HIGH:LOW);
 digitalWrite(MOTOR_LEFT_DIR,left>=0?HIGH:LOW);digitalWrite(MOTOR_RIGHT_DIR,right>=0?HIGH:LOW);
 analogWrite(MOTOR_LEFT_PWM,abs(left));analogWrite(MOTOR_RIGHT_PWM,abs(right));
 if(!left&&!right&&(leftPWM||rightPWM))motorsOffSince=millis();
 leftPWM=left;rightPWM=right;
}
uint8_t motionBlockReason(int l,int r){
 if(l||r)return 0;
 if(!mode)return 1;
 if(!pose.gyro)return 10;
 if(pose.calibrating)return 2;
 if(mode==6){if(traffic.stopped())return 5;if(follower.state==LineFollower::FAIL)return 3;return 9;}
 if(mode==7){if(follow.state==TargetFollow::ARRIVED)return 7;if(follow.state==TargetFollow::LOOK)return 11;if(follow.state==TargetFollow::HOLD)return follow.sweptDeg>=FOLLOW_SWEEP_MAX_DEG?8:6;}
 return 9;
}
void standby(){follower.stop();follow.stop();traffic.reset();ovt.reset();mode=0;motorOutput(0,0);}
void startRun(){
 runNumber++;mode=6;prevSg=0;pose.zero(millis());ovt.reset();
 follower.cruise=cruise; // pivot, crossHold, maxTurn, wideSweep, blockCm, route: LineFollower defaults = MapConfig.h values
 follower.start(pose.yawDegrees,millis());
 traffic.reset();traffic.enabled=true;traffic.stops=traffic.stopsAI=0;
 sampleSeq=0;droppedSamples=0;ringCount=0;lastSample=0;
}
void startFollow(int w,int fov,int kp,int kd){
 runNumber++;mode=7;pose.zero(millis());
 follow.stopW=w;follow.fov=fov;follow.speed=cruise;follow.pd.kp10=kp;follow.pd.kd100=kd;
 follow.start(pose.yawDegrees,millis());traffic.reset();
 sampleSeq=0;droppedSamples=0;ringCount=0;lastSample=0;
}

// ---------- serial protocol ----------
void reply(bool ok){pendingReply=ok?1:0;}
void command(char *text){
 IntegerCommand doc;
 if(!doc.begin(text)||!doc[PSTR("N")].isInt16())return;
 int n=doc[PSTR("N")],d=doc[PSTR("D1")].orDefault(0);
 if(n==204){ // colour frame from the ESP32: S frame id, V valid, R STOP-sign level (1 seen, 2 right in front), G green seen
  if(mode==6&&doc[PSTR("S")].isUInt16()&&(long)doc[PSTR("Z")]==traffic.zone)
   traffic.colour(doc[PSTR("S")],(long)doc[PSTR("V")]==1,(uint8_t)constrain((long)doc[PSTR("R")],0,2),(long)doc[PSTR("G")]==1,
    follower.state==LineFollower::FOLLOW||follower.state==LineFollower::CROSS,millis());
  return;
 }
 if(n==205){ // STOP-sign AI result for our request ID
  if(mode==6&&doc[PSTR("ID")].isUInt16())traffic.ai(doc[PSTR("ID")],(long)doc[PSTR("V")]==1,(long)doc[PSTR("P")]>0,millis());
  return;
 }
 if(n==219){if(mode==6&&doc[PSTR("ID")].isUInt16())traffic.captured(doc[PSTR("ID")],millis());return;}
 if(n==217){ // live speed from the web page (allowed while driving)
  if(d<40||d>DRIVE_LIMIT){reply(false);return;}
  cruise=d;follower.cruise=d;reply(true);return;
 }
 if(n==220){if(mode==7)follow.next(pose.yawDegrees,millis());return;} // next cone of the sequence: look around for it now
 if(n==215){ // colour target frame: S seq, V valid, T seen, X centre x and W width (per mille of the image), A frame age ms
  static const char K215[] PROGMEM="S\0V\0T\0X\0W\0A";
  if(mode==7&&doc[K215].isUInt16()){long a[6];for(uint8_t i=0;i<6;i++)a[i]=doc[K215+2*i];follow.measure((uint16_t)a[0],a[1]==1,a[2]==1,(int)a[3],(int)a[4],(int)a[5],millis());}
  return;
 }
 if(n==218)return; // track localizer (removed in Test22)
 if((n>=200&&n<=203)||n==214)return; // other camera messages are not needed
 if(n==213){ // gyro navigation 0/1, 1 also zeroes; 2 = zero only
  if(mode!=0||d<1||d>2){reply(false);return;}
  if(d){if(d==2)pose.startRecal(millis());else{pose.zero(millis());zeroRevision++;}} // 2: re-measure bias (1 s) then zero; ZR increments when done
  reply(true);return;
 }
 if(n==208){reply(false);return;}
 if(n==206){
  const char *keys[]={PSTR("ID"),PSTR("SC"),PSTR("BL"),PSTR("BH"),PSTR("RA"),PSTR("SA"),PSTR("CT"),PSTR("GS"),PSTR("CS"),PSTR("RS"),PSTR("DS")};
  for(uint8_t i=0;i<11;i++)if(!doc[keys[i]].isInt16())return;
  int id=doc[PSTR("ID")],bl=doc[PSTR("BL")],bh=doc[PSTR("BH")],gs=doc[PSTR("GS")],cs=doc[PSTR("CS")],rs=doc[PSTR("RS")],ds=doc[PSTR("DS")];
  if(mode!=0||id<1||id>30000||bl<0||bh>1023||bl>=bh||gs<0||gs>1||cs<50||cs>150||rs<0||rs>1||ds<0||ds>cs)return;
  // Used here: BL/BH line threshold, CS cruise PWM, GS gyro sign. RS/DS are echoed only.
  blackLow=bl;blackHigh=bh;cruise=cs;pose.gyroSign=gyroSignLearned?(float)gyroSignLearned:(gs?1.0f:-1.0f);echoRS=rs;echoDS=ds;settingsReady=true;configReply=id;return;
 }
 if(n==100||n==110){standby();reply(true);return;}
 if(n==101){
  if(d==0){standby();reply(true);return;}
  // A line run needs a working gyro: without it the old track cannot be rejected at corners.
  bool can=mode==0&&settingsReady&&!pose.calibrating;
  if(d==6&&can&&pose.gyro){
   // D4 start offset after O (cm), D5 speed % while a STOP sign is ahead, D6 map stop line before O (cm, 0 off). D2 is ignored.
   static const char K101[] PROGMEM="D4\0D5\0D6";
   static const uint8_t DEF[3] PROGMEM={83,50,20},LO[3] PROGMEM={20,20,0},HI[3] PROGMEM={150,100,60};
   uint8_t v[3];const char *k=K101;
   for(uint8_t i=0;i<3;i++){long x=doc[k].orDefault(pgm_read_byte(&DEF[i]));if(x<pgm_read_byte(&LO[i])||x>pgm_read_byte(&HI[i])){reply(false);return;}v[i]=x;k+=strlen_P(k)+1;}
   follower.startOff=v[0];slowPct=v[1];stopLine=v[2];
   // obstacle avoidance from the web page (no scanning): D7 on/off, D8 detect cm, D9 first leg cm, D10 straight (obstacle length) cm,
   // D11 return leg cm, D12 turn angle deg, D13 first turn 1 = right / 0 = left. Echoed in N221 (OT OS OL OR OA OD) for the page.
   ovt.enabled=doc[PSTR("D7")].orDefault(0)==1;ovt.detectCm=constrain(doc[PSTR("D8")].orDefault(15),15,120);
   ovt.outCm=constrain(doc[PSTR("D9")].orDefault(15),5,60);ovt.passCm=constrain(doc[PSTR("D10")].orDefault(10),5,150);
   ovt.retCm=constrain(doc[PSTR("D11")].orDefault(15),5,60);ovt.angleDeg=constrain(doc[PSTR("D12")].orDefault(90),30,90);
   ovt.firstRight=doc[PSTR("D13")].orDefault(1)!=0;
   startRun();reply(true);return;
  }
  // cone / target following: D2 stop width (per mille), D3 camera FOV deg (negative = mirrored), D4 Kp x10, D5 Kd x100. Needs the gyro.
  if(d==7&&can&&pose.gyro){
   int w=doc[PSTR("D2")].orDefault(FOLLOW_STOP_W_PM),f=doc[PSTR("D3")].orDefault(FOLLOW_FOV_DEG),kp=doc[PSTR("D4")].orDefault(FOLLOW_KP10),kd=doc[PSTR("D5")].orDefault(FOLLOW_KD100);
   int af=f<0?-f:f;
   if(w>=40&&w<=900&&af>=20&&af<=140&&kp>=1&&kp<=80&&kd>=0&&kd<=50){startFollow(w,f,kp,kd);reply(true);return;}
  }
  reply(false);return;
 }
 if(n==21||n==22||n==210)return; // status is sent periodically anyway
 reply(false);
}
char input[176];uint8_t inputPos=0;bool receiving=false;
void serialInput(){
 uint8_t budget=64;
 while(budget--&&Serial.available()){
  char c=Serial.read();if(c=='{'){inputPos=0;receiving=true;}
  if(!receiving)continue;
  if(inputPos>=sizeof(input)-1){receiving=false;continue;}
  input[inputPos++]=c;
  if(c=='}'){input[inputPos]=0;receiving=false;command(input);}
 }
}

// ---------- sensors ----------
void sensors(){
 lineL=analogRead(LINE_LEFT_PIN);lineM=analogRead(LINE_MIDDLE_PIN);lineR=analogRead(LINE_RIGHT_PIN);
 static uint32_t lastPing=0;
 if(millis()-lastPing>=100){
  lastPing=millis();digitalWrite(US_TRIG_PIN,LOW);delayMicroseconds(2);
  digitalWrite(US_TRIG_PIN,HIGH);delayMicroseconds(10);digitalWrite(US_TRIG_PIN,LOW);
  unsigned long duration=pulseIn(US_ECHO_PIN,HIGH,12000);rangeCm=duration?(int)(duration/58):-1;pingSeq++;
 }
 blackL=hyst(blackL,lineL);blackM=hyst(blackM,lineM);blackR=hyst(blackR,lineR);
 bool l=blackL,m=blackM,r=blackR;
 eventMask|=(l?1:0)|(m?2:0)|(r?4:0)|((l&&m&&r)?8:0);
}

// ---------- telemetry ----------
char tx[340];uint16_t txPos=0,txLen=0; // longest message (N221 with every field at its widest, 330 bytes) is checked by tests/link_test.cpp; the ESP32 accepts 352
uint8_t trafficState(){
 // TS shown by the web page: 0 OFF, 2 TRACK, 3 STOP sign ahead (slow), 4 STOP hold, 7 CROSS, 8 FAULT, 9-12 target following
 if(mode==7)return 8+follow.state; // 9 TRACK, 10 ARRIVED, 11 target lost (stopped), 12 car turning to search, 13 camera search
 if(mode!=6)return 0;
 if(traffic.st!=RaceRules::GO&&traffic.st!=RaceRules::DONE)return traffic.ts();
 switch(follower.state){case LineFollower::CROSS:return 7;case LineFollower::FAIL:return 8;default:return 2;}
}
// state-machine code logged with every sample (page: names and state-transition log)
// 0 standby, 1 track, 2 crossing, 3 corner turn, 4 line search, 5 STOP sign ahead: slow, 6 STOP hold,
// 9-11 obstacle avoidance (out, pass, return + join), 12 fault, 15 target following
uint8_t smState(){
 if(mode==0)return 0;
 if(mode!=6)return 15;
 if(ovt.active())return ovt.state<=2?9:(ovt.state<=4?10:11); // obstacle avoidance: out, pass, return + join
 if(traffic.st==RaceRules::HOLD)return 6;
 if(traffic.slow())return 5;
 if(follower.state==LineFollower::FAIL)return 12;
 switch(follower.state){case LineFollower::CROSS:return 2;case LineFollower::TURN:return 3;case LineFollower::SEARCH:return 4;default:return 1;}
}
void takeSample(uint32_t now){
 if(ringCount==RING){droppedSamples++;ringHead=(ringHead+1)%RING;ringCount--;} // overwrite oldest
 Sample &s=ring[(ringHead+ringCount)%RING];ringCount++;
 s.q=sampleSeq++;s.t=now;s.l=lineL;s.c=lineM;s.r=lineR;s.pl=leftPWM;s.pr=rightPWM;
 s.g10=(int16_t)(pose.yawRate*10);s.yaw10=(int32_t)(pose.yawDegrees*10);s.distMm=(int32_t)(pose.dist*10);
 s.u=rangeCm;s.x=(int16_t)pose.x;s.y=(int16_t)pose.y;
 uint8_t fs=mode==6?(traffic.stopped()?6:follower.state):0;
 if(mode==7){fs=follow.state;s.l=follow.err10;s.c=follow.widthPm;s.r=follow.bear10;} // slots L,C,R carry heading error x10, target width (per mille), bearing x10
 s.sm=smState()|(mode==6?(follower.seg<<4):0); // low nibble: state-machine code, high nibble: map segment
 s.st=(uint8_t)((mode<<4)|(pose.gyro?8:0)|fs); // bit3 = gyro working
 s.k=eventMask|follower.events;
 eventMask=0;follower.events=0;
}
bool startTx(int n){if(n>0&&n<(int)sizeof(tx)){txPos=0;txLen=n;return true;}return false;}
void scheduleTx(uint32_t now){
 static uint32_t lastStatus=0;
 if(txPos<txLen)return;
 if(configReply>=0){
  startTx(linkFmt(tx,sizeof(tx),PSTR("{\"N\":222,\"ID\":%d,\"OK\":1,\"CS\":%d,\"RS\":%d,\"DS\":%d}\n"),configReply,cruise,echoRS,echoDS));configReply=-1;return;
 }
 if(pendingReply>=0){startTx(linkFmt(tx,sizeof(tx),PSTR("{\"N\":211,\"OK\":%d}\n"),(int)pendingReply));pendingReply=-1;return;}
 bool statusDue=uint32_t(now-lastStatus)>=STATUS_MS||traffic.changed; // a traffic state change is reported at once (ESP32 starts the STOP AI)
 if(ringCount&&!traffic.changed&&!(statusDue&&uint32_t(now-lastStatus)>=STATUS_MS+250)){
  Sample &s=ring[ringHead];ringHead=(ringHead+1)%RING;ringCount--;
  // d=[q,t,L,C,R,PL,PR,gyro_rate*10,yaw*10,dist_mm,x_cm,y_cm,ultrasonic,state(mode*16+8*gyroOK+follow),mask, predicted: heading*10,x_cm,y_cm, state code]
  startTx(linkFmt(tx,sizeof(tx),PSTR("{\"N\":230,\"d\":[%u,%lu,%d,%d,%d,%d,%d,%d,%ld,%ld,%d,%d,%d,%u,%u,0,0,0,%u]}\n"),
   s.q,(unsigned long)s.t,s.l,s.c,s.r,s.pl,s.pr,s.g10,(long)s.yaw10,(long)s.distMm,s.x,s.y,s.u,(unsigned)s.st,(unsigned)s.k,(unsigned)s.sm)); // slots 15-17 (legacy prediction) are 0: the page rebuilds it from PWM
  return;
 }
 if(statusDue){
  lastStatus=now;traffic.changed=false;
  startTx(linkFmt(tx,sizeof(tx),PSTR("{\"N\":221,\"M\":%u,\"TS\":%u,\"ID\":%u,\"L\":%d,\"C\":%d,\"R\":%d,\"GY\":%u,\"UV\":%d,\"UT\":%lu,\"FC\":%u,\"PL\":%d,\"PR\":%d,\"ES\":%u,\"RN\":%u,\"GN\":%u,\"ZR\":%u,\"IM\":%u,\"CB\":%u,\"GL\":%d,\"GF\":%u,\"SG\":%u,\"LP\":%u,\"OV\":%u,\"OT\":%d,\"OS\":%d,\"OL\":%d,\"OR\":%d,\"OA\":%d,\"OD\":%u,\"Z\":%u,\"CV\":%d,\"SN\":%u,\"SA\":%u,\"BV\":%u,\"A5\":%u,\"US\":%d,\"BR\":%u}\n"),
   mode,trafficState(),traffic.req,lineL,lineM,lineR,(unsigned)pose.gyro,FIRMWARE_UV,(unsigned long)now,
   (unsigned)(mode==6?follower.fault:0),leftPWM,rightPWM,0u,runNumber,1u,(unsigned)zeroRevision,(unsigned)pose.imuType,(unsigned)pose.calibrating,(int)gyroSignLearned,(unsigned)gyroSignFlips,(unsigned)follower.seg,(unsigned)follower.lapsDone,(unsigned)(ovt.enabled?ovt.state+1:0),ovt.detectCm,ovt.outCm,ovt.passCm,ovt.retCm,ovt.angleDeg,(unsigned)ovt.firstRight,(unsigned)traffic.zone,cruise,(unsigned)traffic.stops,(unsigned)traffic.stopsAI,(unsigned)battV10,1u,rangeCm,(unsigned)blockReason));
 }
}
void pumpTx(){int a=Serial.availableForWrite();while(txPos<txLen&&a-->0)Serial.write(tx[txPos++]);}

void setup(){
 Serial.begin(LINK_BAUD);
 ledBegin();
 panBegin();   // camera straight ahead
 pinMode(MOTOR_STBY,OUTPUT);pinMode(MOTOR_LEFT_PWM,OUTPUT);pinMode(MOTOR_RIGHT_PWM,OUTPUT);
 pinMode(MOTOR_LEFT_DIR,OUTPUT);pinMode(MOTOR_RIGHT_DIR,OUTPUT);motorOutput(0,0);
 pinMode(US_TRIG_PIN,OUTPUT);pinMode(US_ECHO_PIN,INPUT);
 pose.begin(); // keep the car still for ~1 s after power-on (gyro bias)
 motorsOffSince=millis();
}
void loop(){
 uint32_t now=millis();
 serialInput();pumpTx();
 bool parked=mode==0&&!leftPWM&&!rightPWM&&uint32_t(now-motorsOffSince)>1000;
 pose.step(leftPWM,rightPWM,now,parked);
 sensors();serialInput();
 if(uint32_t(now-ledAt)>=1000){ledAt=now;battV10=batteryTenths();batteryLed(battV10);}

 int left=0,right=0;now=millis();
 if((mode==6||mode==7)&&!pose.gyro){ /* feedback lost: do not advance either controller or command wheels */ }
 else if(mode==6){
  // ---- STOP-sign zone from the map: 2 on E-O and C-O until that stop is done, otherwise 0 ----
  uint8_t sg=follower.seg;
  { // the crossing O was reached on a STOP segment without a stop (sign not seen, map stop line not reached): stop now
   if(sg!=prevSg&&(prevSg==2||prevSg==6)&&traffic.enabled&&(traffic.st==RaceRules::GO||traffic.st==RaceRules::SLOW))traffic.hold(now);
   prevSg=sg;}
  traffic.segment(sg);
  traffic.setZone(routeFeature(sg,traffic.st==RaceRules::DONE));
  bool fresh=pingSeq!=pingSeen;pingSeen=pingSeq;
  traffic.tick(now,rangeCm,fresh);
  if(traffic.zone==2)traffic.stopLine(follower.toO(),stopLine,now);
  if(traffic.stopped()){follower.lastMs=now;} // stand still at the STOP sign; the line follower resumes where it was
  else{
   follower.odo(pose.dist);            // odometer (true cm) once per step
   // obstacle ahead: drive round it (no scan, web obstacle size), then the line follower resumes on the line
   bool detour=ovt.active();
   if(ovt.step(pose.yawDegrees,pose.yawRate,pose.dist,rangeCm,fresh,follower.state==LineFollower::FOLLOW||follower.state==LineFollower::CROSS,
              follower.refYaw,blackL||blackM||blackR,now,left,right))follower.lastMs=now;
   else{
    if(detour){follower.distCm-=ovt.extraCm*follower.odoK;follower.resume(pose.yawDegrees,ovt.H,now);} // side steps are not track progress
    follower.step(blackL,blackM,blackR,now,pose.yawDegrees,pose.gyro,usRange(),left,right);
    uint8_t pct=traffic.slow()?slowPct:100; // slower towards a STOP sign
    if(pct<100&&left>=0&&right>=0){left=left*pct/100;right=right*pct/100;}
   }
  }
  if(follower.requestSignFlip){ // commanded a left pivot but the gyro said right: the gyro sign (GS) is reversed
   follower.requestSignFlip=false;pose.gyroSign=-pose.gyroSign;pose.yawDegrees=-pose.yawDegrees;pose.heading=-pose.heading;
   follower.negateAngles();gyroSignLearned=pose.gyroSign>0?1:-1;gyroSignFlips++;
  }
 }
 else if(mode==7){
  follow.step(pose.yawDegrees,pose.yawRate,now,usRange(),left,right);
 }
 panSet(mode==7?follow.pan:0);   // camera search turns the servo; otherwise straight ahead
 blockReason=motionBlockReason(left,right);motorOutput(left,right);
 if(pose.calDone){pose.calDone=false;zeroRevision++;}
 uint32_t period=mode?SAMPLE_MS:IDLE_SAMPLE_MS;
 if(uint32_t(now-lastSample)>=period){lastSample=now;takeSample(now);}
 scheduleTx(now);pumpTx();
}
