#pragma once
#include <stdint.h>
#include <cstdlib>
#include <cmath>
#include <cstring>
#include <string>
#define HIGH 1
#define LOW 0
#define OUTPUT 1
#define INPUT 0
#define INPUT_PULLUP 2
#define A0 14
#define A1 15
#define A2 16
#define A3 17
#define PI 3.14159265358979323846
#define constrain(x,a,b) ((x)<(a)?(a):((x)>(b)?(b):(x)))
// Timer1 registers (camera pan servo, PanServo.h)
#define _BV(b) (1u<<(b))
#define COM1B1 5
#define WGM11 1
#define WGM12 3
#define WGM13 4
#define CS11 1
static uint8_t TCCR1A,TCCR1B;static uint16_t ICR1,OCR1B;
extern uint32_t fakeNow;
extern int fakeRange,fakeFloor[3],fakePins[32];
inline uint32_t millis(){return fakeNow;}
inline void delay(unsigned long ms){fakeNow+=ms;}
inline void delayMicroseconds(unsigned){}
inline void pinMode(int,int){}
inline void digitalWrite(int p,int v){fakePins[p]=v;}
inline void analogWrite(int p,int v){fakePins[p]=v;}
inline int analogRead(int p){return p==A2?fakeFloor[0]:p==A1?fakeFloor[1]:p==A0?fakeFloor[2]:210;}
inline unsigned long pulseIn(int,int,unsigned long){return fakeRange>0?fakeRange*58UL:0;}
struct FakeSerial{
 std::string out,in;size_t ip=0;   // in: bytes the ESP32 sends to the UNO (link test)
 void begin(long){}
 int available(){return (int)(in.size()-ip);}
 int read(){return ip<in.size()?(unsigned char)in[ip++]:-1;}
 int availableForWrite(){return 1024;}
 void write(char c){out+=c;}
};
extern FakeSerial Serial;
