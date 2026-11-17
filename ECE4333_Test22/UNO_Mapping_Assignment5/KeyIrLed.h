#pragma once
#include <Arduino.h>
#include "Hardware.h"
// Test22: battery LED only (WS2812 on RGB_PIN). Start / stop come from the web page; no KEY or IR remote.
void ledBegin(){pinMode(RGB_PIN,OUTPUT);}
// one WS2812 pixel, 16 MHz: 20 cycles per bit, T0H 5 cycles (0.31 us), T1H 13 cycles (0.81 us)
void ledSend(uint8_t r,uint8_t g,uint8_t b){
 uint8_t v[3]={g,r,b},hi=PORTD|_BV(4),lo=PORTD&~_BV(4);uint8_t s=SREG;cli();
 for(uint8_t i=0;i<3;i++){uint8_t x=v[i],n=8;
  asm volatile("1: out %[p],%[hi]\n nop\n nop\n nop\n sbrs %[x],7\n out %[p],%[lo]\n lsl %[x]\n nop\n nop\n nop\n nop\n nop\n nop\n out %[p],%[lo]\n nop\n nop\n nop\n dec %[n]\n brne 1b\n"
   :[x]"+r"(x),[n]"+r"(n):[p]"I"(_SFR_IO_ADDR(PORTD)),[hi]"r"(hi),[lo]"r"(lo));}
 SREG=s;
}
uint8_t batteryTenths(){return (uint8_t)(analogRead(BATTERY_PIN)*BATTERY_SCALE*1.08f*10);} // ELEGOO divider + 8 % correction
void batteryLed(uint8_t v10){if(v10>=76)ledSend(0,24,0);else if(v10>=70)ledSend(24,14,0);else ledSend(30,0,0);}
