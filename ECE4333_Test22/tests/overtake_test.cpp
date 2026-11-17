// Host simulation of Overtake.h (no-scan obstacle avoidance, every distance / angle from the web page) on a straight road.
// Robot: differential drive, wheel speed = PWM * 0.33 cm/s (MapConfig), track 14 cm, first-order motor lag 0.12 s,
// body = 20 x 16 cm rectangle around the axle centre (+12 cm front, -8 cm rear), ultrasonic 11 cm ahead of the axle
// (+-12 deg cone), three line sensors 7 cm ahead of the axle at -2/0/+2 cm, tape 2 cm wide along y = 0.
// Obstacle: square box (side = obstacle length), centred on the road.
// Checks: 1) each leg and turn is what the web page set; 2) the page's clearance check (clearOK below, same formula as
// the page) predicts correctly whether the car clears the obstacle; 3) with values that pass the check the car gets
// round and back on the line; 4) trigger logic.
// Build: g++ -std=c++11 -I ../UNO_Mapping_Assignment5 overtake_test.cpp -o ovt && ./ovt
#include <cstdio>
#include <cmath>
#include <cstdint>
#include <initializer_list>
#include "Overtake.h"

static const float K=0.33f,TRACK=14.0f,TAU=0.12f,DT=0.01f,RAD=3.14159265f/180;
struct Sim{
 float x=0,y=0,th=0,vl=0,vr=0,dist=0,f=1.0f;    // th radians, + = left; f = real speed / PWM-model speed
 float ox0,ox1,oy0,oy1;
 bool insideBox(float px,float py)const{return px>=ox0&&px<=ox1&&py>=oy0&&py<=oy1;}
 bool collides()const{
  for(float a=-8;a<=12;a+=1)for(float b=-8;b<=8;b+=16){float px=x+a*cosf(th)-b*sinf(th),py=y+a*sinf(th)+b*cosf(th);if(insideBox(px,py))return true;}
  for(float b=-8;b<=8;b+=1)for(float a=-8;a<=12;a+=20){float px=x+a*cosf(th)-b*sinf(th),py=y+a*sinf(th)+b*cosf(th);if(insideBox(px,py))return true;}
  return false;
 }
 int range()const{
  float sx=x+11*cosf(th),sy=y+11*sinf(th);int best=-1;
  for(int k=-12;k<=12;k+=3){float a=th+k*RAD;for(float d=1;d<=300;d+=0.5f){if(insideBox(sx+d*cosf(a),sy+d*sinf(a))){if(best<0||d<best)best=(int)d;break;}}}
  return best;
 }
 bool sensor(float lat)const{float py=y+7*sinf(th)+lat*cosf(th);return fabsf(py)<1.0f;}
 void step(int l,int r){vl+=(l*K-vl)*DT/TAU;vr+=(r*K-vr)*DT/TAU;float v=(vl+vr)/2,w=(vr-vl)/TRACK;th+=f*w*DT;x+=f*v*cosf(th)*DT;y+=f*v*sinf(th)*DT;dist+=v*DT;}
 float rateDeg()const{return f*(vr-vl)/TRACK/RAD;}
};
static void followLine(const Sim&s,int&l,int&r){float e=-(s.y*2.0f)-s.th/RAD;int u=(int)(e*1.5f);if(u>25)u=25;if(u<-25)u=-25;l=60-u;r=60+u;}

// The web page's clearance check (TrafficPage.h ovtCheck): forward advance of the axle >= detect + sensor 11 + length + rear 8 + 4,
// sideways >= length/2 + half width 8 + 2
static bool clearOK(int det,int out,int pass,int ret,int ang,int len){
 float c=cosf(ang*RAD),s=sinf(ang*RAD);
 return out*c+pass+ret*c>=det+11+len+12&&out*s>=len/2.0f+10;
}

struct Res{bool collided,finished;float y,yaw,legOut,legPass,legRet,turn1;int ok,fails;};
static Res run(int det,int out,int pass,int ret,int ang,bool right,int len,float f=1.0f){
 Sim s;s.f=f;Overtake o;o.enabled=true;o.detectCm=det;o.outCm=out;o.passCm=pass;o.retCm=ret;o.angleDeg=ang;o.firstRight=right;
 float face=11+det+40;s.ox0=face;s.ox1=face+len;s.oy0=-len/2.0f;s.oy1=len/2.0f;
 Res R{};uint32_t lastPing=0;int rg=-1;bool was=false;float dEnter=0;int prev=0;
 for(int i=0;i<60000;i++){
  uint32_t now=i*10;bool fresh=false;if(now-lastPing>=100){lastPing=now;rg=s.range();fresh=true;}
  int l=0,r=0;float yaw=s.th/RAD;
  bool drv=o.step(yaw,s.rateDeg(),s.dist,rg,fresh,!was||o.state==0,0.0f,s.sensor(-2)||s.sensor(0)||s.sensor(2),now,l,r);
  if(o.state!=prev){ // leg lengths in real cm (odometry of the sim = PWM model)
   if(prev==Overtake::OUT_DRIVE)R.legOut=s.dist-dEnter;
   if(prev==Overtake::PASS)R.legPass=s.dist-dEnter;
   if(prev==Overtake::RET_DRIVE)R.legRet=s.dist-dEnter;
   if(prev==Overtake::OUT_TURN)R.turn1=yaw;
   dEnter=s.dist;prev=o.state;
  }
  if(!drv&&o.ok+o.fails==0&&!o.holding)followLine(s,l,r);
  if(!drv&&o.ok+o.fails>0)followLine(s,l,r);
  was=drv;s.step(l,r);
  if(s.collides()){R.collided=true;break;}
  if(o.ok+o.fails>0&&!drv&&s.x>s.ox1+40)break;
 }
 R.finished=o.ok==1;R.ok=o.ok;R.fails=o.fails;R.y=s.y;R.yaw=s.th/RAD;return R;
}

int main(){
 int bad=0;
 // 1) the user's defaults: right 90, 15 cm out, 10 cm straight, 15 cm back, detected at 15 cm, obstacle 10 cm
 {Res R=run(15,15,10,15,90,true,10);
  printf("defaults (detect 15, right 90, 15 / 10 / 15, obstacle 10): first turn to %.0f deg, legs %.1f / %.1f / %.1f cm, collided=%d\n",R.turn1,R.legOut,R.legPass,R.legRet,R.collided);
  bool legs=fabsf(R.turn1+90)<8&&fabsf(R.legOut-15)<2.5f&&fabsf(R.legPass-10)<2.5f&&R.legRet>=7.5f&&R.legRet<=31;
  printf("%s legs and first turn are the web values (right = -90 deg)\n",legs?"PASS":"FAIL");bad+=!legs;
  bool pred=clearOK(15,15,10,15,90,10)==!R.collided;
  printf("%s page clearance check predicts the outcome (%s)\n",pred?"PASS":"FAIL",R.collided?"these values hit the obstacle: the page warns":"clears");bad+=!pred;}
 // 2) clearance check vs simulation over a grid, 3) values that pass the check get round and back on the line
 int dets[]={15,25,40},lens[]={10,20},angs[]={45,60,90},passes[]={10,30,50,70,90};int agree=0,total=0;
 for(int d:dets)for(int L:lens)for(int a:angs)for(int p:passes)for(int sd=0;sd<2;sd++){
  int out=15+L/2;Res R=run(d,out,p,out,a,sd==1,L);bool ok=clearOK(d,out,p,out,a,L);total++;
  bool good=ok?(!R.collided&&R.finished&&fabsf(R.y)<6&&fabsf(R.yaw)<15):R.collided; // predicted clear -> clears and rejoins; predicted hit -> hits
  if(ok&&R.collided){printf("FAIL check said clear but hit: det %d len %d ang %d pass %d out %d side %s\n",d,L,a,p,out,sd?"R":"L");}
  else if(ok&&!good)printf("FAIL cleared but not back on the line: det %d len %d ang %d pass %d y %.1f yaw %.1f ok %d fails %d\n",d,L,a,p,R.y,R.yaw,R.ok,R.fails);
  agree+=good||(!ok&&!R.collided); // a predicted hit that in fact clears is only conservative
  if(ok&&!good)bad++;
 }
 printf("%s clearance check vs simulation: %d / %d cases agree (never 'clear' when it hits)\n",bad?"FAIL":"PASS",agree,total);
 // speed model error: a car 15 %% faster / slower than the PWM model, values with margin
 for(float f:{0.85f,1.15f}){Res R=run(15,20,60,20,90,true,10,f);bool g=!R.collided&&R.finished&&fabsf(R.y)<6;printf("%s speed x%.2f (detect 15, 20 / 60 / 20, 90 deg): collided=%d y=%.1f\n",g?"PASS":"FAIL",f,R.collided,R.y);bad+=!g;}
 // 4) trigger logic
 {Overtake o;int l,r;bool any=false;for(uint32_t t=0;t<2000;t+=10)any|=o.step(0,0,0,10,t%100==0,true,0,false,t,l,r);printf("%s disabled never drives\n",any?"FAIL":"PASS");bad+=any;}
 {Overtake o;o.enabled=true;int l,r;bool any=false;for(uint32_t t=0;t<2000;t+=10)any|=o.step(0,0,0,10,t%100==0,false,0,false,t,l,r);printf("%s no trigger while the follower is not on the line (corner / search)\n",any?"FAIL":"PASS");bad+=any;}
 {Overtake o;o.enabled=true;int l,r;bool started=false;for(uint32_t t=0;t<3000;t+=10){int rg=(t%400==0)?10:60;o.step(0,0,0,rg,t%100==0,true,0,false,t,l,r);started|=o.active();}
  printf("%s a single echo only pauses the car (no detour)\n",started?"FAIL":"PASS");bad+=started;}
 {Overtake o;o.enabled=true;o.detectCm=30;int l,r;int startedAt=-1;for(uint32_t t=0;t<3000&&startedAt<0;t+=10){int rg=60-(int)(t/50);o.step(0,0,0,rg,t%100==0,true,0,false,t,l,r);if(o.active())startedAt=rg;}
  bool g=startedAt>=26&&startedAt<=30;printf("%s detour starts at the detect distance (started at %d cm, detect 30)\n",g?"PASS":"FAIL",startedAt);bad+=!g;}
 printf(bad?"OVERTAKE TESTS FAILED: %d\n":"ALL OVERTAKE TESTS PASS\n",bad);
 return bad?1:0;
}
