#pragma once
#include <Arduino.h>
#include "MapConfig.h"
// Camera pan servo on D10 (OC1B), driven by Timer1 directly: 50 Hz, 0.5 us steps. No Servo library (saves ~0.9 KB flash).
// Timer1 is free: the motor PWM pins D5 / D6 use Timer0. panDeg: + = look left of the car axis, 0 = straight ahead.
#define SERVO_PAN_PIN 10
int8_t panNow=127;uint32_t panMoved=0;
void panSet(int8_t deg){
 if(deg==panNow)return;
 panNow=deg;panMoved=millis();
 int a=constrain(90+PAN_DIR*deg,0,180);
 OCR1B=(uint16_t)(1088+(long)a*3712/180);          // 544-2400 us pulse (the Servo library's range), 2 ticks per us
}
void panBegin(){
 pinMode(SERVO_PAN_PIN,OUTPUT);
 TCCR1A=_BV(COM1B1)|_BV(WGM11);TCCR1B=_BV(WGM13)|_BV(WGM12)|_BV(CS11);ICR1=39999; // fast PWM, TOP = ICR1: 20 ms
 panSet(0);
}
// ultrasonic (on the same bracket) counts only when it looks straight ahead and the servo has settled
bool panAhead(){return panNow==0&&uint32_t(millis()-panMoved)>FOLLOW_PAN_SETTLE_MS;}
