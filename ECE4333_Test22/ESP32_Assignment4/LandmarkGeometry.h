#pragma once
#include <math.h>
#include <cmath>
// Calibration gives optical depth from apparent width. Translate that point
// from the forward camera to the robot centre before the EKF measurement.
static bool landmarkRB(int imageW,int x,int width,int referencePm,int calibrationCm,
                       int fovDeg,float cameraAheadCm,float &range,float &bearing){
 if(imageW<=0||width<=0||referencePm<=0||calibrationCm<=0)return false;
 float wp=1000.0f*width/imageW;
 float depth=calibrationCm*referencePm/wp;
 float u=(x+0.5f*width)/imageW;
 float y=depth*(0.5f-u)*2*tanf(fovDeg*0.00872664626f);
 float bx=cameraAheadCm+depth;
 range=sqrtf(bx*bx+y*y);bearing=atan2f(y,bx);
 return std::isfinite(range)&&std::isfinite(bearing)&&range>=10&&range<=250;
}
