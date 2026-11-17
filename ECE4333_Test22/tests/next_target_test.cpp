// Host test of ESP32_Assignment4/NextTarget.h: after a target is reached, which region may be the next target.
// Image 400 px wide (CIF), stop width 188 per mille (75 px).
// Build: g++ -std=c++11 -I ../ESP32_Assignment4 next_target_test.cpp -o nt && ./nt
#include <cstdio>
#include "NextTarget.h"

static int fails=0;
#define CHECK(c,msg) do{if(c)printf("PASS %s\n",msg);else{printf("FAIL %s\n",msg);fails++;}}while(0)
static RedRegion reg(int x,int w){RedRegion g;g.x=x;g.w=w;g.h=w;g.imageW=400;g.imageH=296;g.pixels=w*w;return g;}
static void reached(int c){reachedGone=false;reachedColour=c;}   // as coneStep() does

int main(){
 const int RED=1,GREEN=0;
 reached(RED);
 CHECK(!nextTargetOk(reg(160,80),true,RED),"the red target just reached (80 px, centre) is not the next target");
 CHECK(nextTargetOk(reg(100,40),true,GREEN),"another colour may be chosen at once");
 CHECK(!nextTargetOk(reg(0,20),true,RED),"red sliding out at the left side (narrow) is still the reached target");
 CHECK(!nextTargetOk(reg(380,20),true,RED),"red sliding out at the right side (narrow) is still the reached target");
 CHECK(!nextTargetOk(reg(190,20),true,RED),"small red before red has left the image once: not yet");
 CHECK(!nextTargetOk(reg(0,0),false,RED),"red no longer seen: the car has turned away");
 CHECK(!nextTargetOk(reg(150,80),true,RED),"red seen large again (the same target after a full turn): not the next one");
 CHECK(!nextTargetOk(reg(0,25),true,RED),"red entering at the image side: wait until it is fully in view");
 CHECK(nextTargetOk(reg(180,30),true,RED),"a far red (30 px, fully in view) after red left the image: next target");
 CHECK(reachedColour.load()==-1,"once the next target is chosen the rule is cleared");
 CHECK(nextTargetOk(reg(160,90),true,RED),"approaching it (it grows) is allowed");
 reachedColour=-1;
 CHECK(nextTargetOk(reg(160,80),true,RED),"no target reached yet: anything may be chosen");
 printf(fails?"NEXT TARGET TESTS FAILED: %d\n":"ALL NEXT TARGET TESTS PASS\n",fails);
 return fails?1:0;
}
