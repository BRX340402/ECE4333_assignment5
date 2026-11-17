#pragma once
#include <stdint.h>
// One completion per ARRIVED episode, timed using UNO samples, not serial drain time.
struct ConeProgress{
 bool dwelling=false,awaitDeparture=false;uint32_t since=0,last=0;bool haveTime=false;
 void reset(){dwelling=awaitDeparture=haveTime=false;since=last=0;}
 bool update(int fs,uint32_t t){
  if(haveTime&&int32_t(t-last)<=0)return false;
  haveTime=true;last=t;
  if(fs!=2){dwelling=false;awaitDeparture=false;return false;}
  if(awaitDeparture)return false;
  if(!dwelling){dwelling=true;since=t;return false;}
  if(uint32_t(t-since)<600)return false;
  dwelling=false;awaitDeparture=true;return true;
 }
};
