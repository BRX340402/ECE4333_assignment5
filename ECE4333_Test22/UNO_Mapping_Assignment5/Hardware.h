#pragma once
// User's ex 2 is TB6612: right AIN=D7, left BIN=D8, STBY=D3.
// Do NOT substitute the official older DRV8835 pin mapping.
#define MOTOR_RIGHT_PWM 5
#define MOTOR_LEFT_PWM 6
#define MOTOR_RIGHT_DIR 7
#define MOTOR_LEFT_DIR 8
#define MOTOR_STBY 3
#define RGB_PIN 4
#define US_TRIG_PIN 13
#define US_ECHO_PIN 12
#define LINE_LEFT_PIN A2
#define LINE_MIDDLE_PIN A1
#define LINE_RIGHT_PIN A0
#define BATTERY_PIN A3
static const float BATTERY_SCALE=0.0375f; // official divider coefficient; verify with a meter
static const int DRIVE_LIMIT=180;
