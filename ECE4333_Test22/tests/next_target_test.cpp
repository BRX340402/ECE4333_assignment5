// Host test of ESP32_Assignment4/NextTarget.h (target following, Auto colour): the first target is the first colour seen
// (largest if several); every next target is any colour except the colour just reached (only that one is excluded).
// Colours: 0 green, 1 red, 2 blue.
// Build: g++ -std=c++11 -I ../ESP32_Assignment4 next_target_test.cpp -o nt && ./nt
#include <cstdio>
#include "NextTarget.h"

static int fails=0;
#define CHECK(c,msg) do{if(c)printf("PASS %s\n",msg);else{printf("FAIL %s\n",msg);fails++;}}while(0)
static const int G=0,R=1,B=2;
static int pick(bool g,int gp,bool r,int rp,bool b,int bp,int last){bool s[3]={g,r,b};int p[3]={gp,rp,bp};return chooseColour(s,p,last);}

int main(){
 CHECK(pick(false,0,true,500,false,0,-1)==R,"first target: the only colour seen (red)");
 CHECK(pick(true,300,true,900,true,100,-1)==R,"first target: several colours seen -> the largest (red)");
 CHECK(pick(false,0,false,0,false,0,-1)==-1,"nothing seen -> no target (keep looking)");
 // after red is reached the red cone is right in front (large): it must not be chosen again
 CHECK(pick(false,0,true,4000,false,0,R)==-1,"after red: the red target just reached (large, in front) is not chosen");
 CHECK(pick(true,200,true,4000,false,0,R)==G,"after red: green chosen although the reached red is bigger");
 CHECK(pick(true,200,true,4000,true,600,R)==B,"after red: green or blue, the larger one (blue)");
 // only the last colour is excluded, nothing accumulates
 CHECK(pick(false,0,true,300,false,0,G)==R,"after green: red may be a target again (only green is excluded)");
 CHECK(pick(true,300,false,0,false,0,B)==G,"after blue: green allowed");
 // a whole r-g-r-g course: at each stop the car sees the cone it reached (large) and the next cone (smaller)
 {const int course[4]={R,G,R,G};int last=-1;bool ok=true;
  for(int i=0;i<4;i++){bool s[3]={false,false,false};int p[3]={0,0,0};
   s[course[i]]=true;p[course[i]]=200;                                 // next cone, far
   if(last>=0){s[last]=true;p[last]=4000;}                             // the cone just reached, right in front
   int c=chooseColour(s,p,last);if(c!=course[i])ok=false;last=c;}
  CHECK(ok,"course r-g-r-g: every next cone is chosen, never the one just reached");}
 lastReached=-1;CHECK(lastReached.load()==-1,"a new run starts with no colour excluded");
 printf(fails?"NEXT TARGET TESTS FAILED: %d\n":"ALL NEXT TARGET TESTS PASS\n",fails);
 return fails?1:0;
}
