#pragma once
#include <Arduino.h>
struct PoseEstimate{
 float x=0,y=0,heading=0,yawRate=0,yawDegrees=0,dist=0,gyroSign=1;
 bool calibrating=false,calDone=false,gyro=true;uint8_t imuType=1;
 void begin(){}
 void zero(uint32_t){x=y=heading=yawDegrees=dist=0;}
 void startRecal(uint32_t){calibrating=true;}
 void step(int,int,uint32_t,bool){}
};
