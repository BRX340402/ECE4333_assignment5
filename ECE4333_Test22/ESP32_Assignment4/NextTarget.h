#pragma once
#include <atomic>
#include "RedDetector.h"
// After a target is reached, a region of its colour may be the next target only after that colour has left the image once
// (the car turned away from the reached target), and only if it is small (far: under half the stop width) and does not touch
// the image side (a target sliding out of view looks narrow). Another colour may be chosen at once.
std::atomic<int> reachedColour{-1};std::atomic<bool> reachedGone{false};std::atomic<int> followStopPm{188};
static bool nextTargetOk(const RedRegion &g,bool seen,int c){
 if(c!=reachedColour.load())return true;
 if(!seen){reachedGone=true;return false;}
 bool edge=g.x<=2||g.x+g.w>=g.imageW-2;
 if(reachedGone.load()&&g.imageW>0&&!edge&&(long)g.w*2000/g.imageW<followStopPm.load()){reachedColour=-1;return true;}
 return false;
}
