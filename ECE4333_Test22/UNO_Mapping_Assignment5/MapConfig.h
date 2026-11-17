#pragma once
#include <stdint.h>
#include <avr/pgmspace.h>
// ===== Test14 (Assignment 4 race build), based on Test13: every tunable value is collected here =====
// UNO<->ESP32 serial link. Must equal UNO_LINK_BAUD in ESP32_Assignment2/Config.h.
// If communication fails, set both to 9600 and raise SAMPLE_MS to 150.
static const long LINK_BAUD = 19200;
static const int  FIRMWARE_UV = 51;        // N221 "UV"; ESP32/web page require 51 (Test21)

// Data logging
static const uint16_t SAMPLE_MS = 100;      // N230 sample period while driving (10 Hz)
static const uint16_t IDLE_SAMPLE_MS = 200; // N230 period while stopped (live gyro readout on the page)
static const uint16_t STATUS_MS = 1000;     // N221 (web page status) period
static const uint16_t SENSOR_MS = 2000;     // N210 (battery/ultrasonic) period

// Driving (values not set by the web page)
static const int  PIVOT_PWM = 100;          // in-place turn at corners/search
static const uint16_t CROSS_HOLD_MS = 400;  // hold heading this long across the 60-degree crossing
static const int  MAX_CORNER_DEG = 200;     // corner pivot limit before falling back to search
static const int  WIDE_SEARCH_DEG = 170;    // last-resort sweep amplitude
static const int  FORWARD_GUARD_CM = 12;    // something this close straight ahead: the car waits (no E-stop latch)
static const int  CREEP_BLOCK_CM = 15;      // search will not creep forward if an obstacle is this close
// Fixed-route map (LineFollower route*). Set false for a different track; corners then use the
// sensors only and the STOP-sign / traffic-light zones (they come from the map) are not available.
static const bool ROUTE_HINT = true;

// ===== Test14 race map (Course project Figure 8) =====
// Corner names: C = lower right, D = upper left, E = upper right, A = kink below O, B = lower left, O = intersection.
// Driving direction (arrows in the figure): C -> O -> D -> E -> O -> A -> B -> C.
// START S: on O-D, in front of the traffic light, START_FROM_O_CM after O, facing D (yaw = 0 there).
// Route segments (index = follower.seg):
//   0 S-D   1 D-E   2 E-O (STOP sign 1 before O)   3 O-A   4 A-B   5 B-C (obstacle)   6 C-O (STOP sign 2 before O)   7 O-S (traffic light)
// Heading of each straight in degrees (+ = left, unwrapped from the start heading). From the drawing:
// 60 deg crossing, O-D 1.7 m, E-O 1.2 m -> corner D turns right 136.6 deg, E right 103.4 deg, A left 30, B left 90, C left 120.
static const int16_t ROUTE_HEADING[8] PROGMEM = {0,-137,-240,-240,-210,-120,0,0};
// Measured centre-line lengths (cm). Index 0 and 7 are derived from O-D and the start offset.
static const int16_t ROUTE_LEN[8] PROGMEM     = {0,148,118,99,47,125,150,0};
static const int16_t ROUTE_OD_CM = 167;       // O to D
static const int16_t START_FROM_O_CM = 83;    // default start point: middle of O-D (web page can change it)
// Overtaking side = OUTSIDE of the loop being driven (+1 left, -1 right). Upper loop (right turns D, E): left.
// Lower loop (left turns A, B, C): right. Never on the inside.
static inline int8_t routeOutside(uint8_t seg){return (seg>=3&&seg<=6)?-1:1;}

// Pose model. Measured on the real car (Test10 run 2): corner C to corner D = 3.2 m in 20 s
// at PWM 34-50 -> about 0.33 cm/s per PWM. The map still fine-tunes the scale from complete straights.
static const float TRACK_WIDTH_CM = 14.0f;
static const float LEFT_CM_S_PER_PWM = 0.33f, RIGHT_CM_S_PER_PWM = 0.33f;
// Line sensors: hysteresis around the black threshold (ADC counts). A sensor riding the tape
// edge reads in between (Test10 run 2: left sensor 550-730 on a 600 threshold) and must not flicker.
static const int  LINE_HYST = 40;

// Test22 build: figure-8 run (STOP sign, map, obstacle avoidance) + target / cone following, battery LED.
// 1 = the line search may creep forward a step between sweeps (LineFollower.h).
#define ASSIGNMENT5_BUILD 1

// Target following. Defaults for what the web page sends with its Follow button.
static const int FOLLOW_STOP_W_PM = 188;   // stop when the target is this wide: 75 px of a 400 px CIF image (per mille)
static const int FOLLOW_NEAR_PM = 110;     // a target this wide that then leaves the image counts as reached
static const int FOLLOW_FOV_DEG   = 62;    // camera horizontal field of view; negative = image mirrored
static const int FOLLOW_KP10      = 15;    // inner-loop PD: PWM per degree x10
static const int FOLLOW_KD100     = 10;    // inner-loop PD: PWM per (deg/s) x100
static const uint16_t FOLLOW_OUTER_MS = 500;       // camera outer loop period (assignment: 0.5-1 s)
static const uint16_t FOLLOW_LOST_HOLD_MS = 1500;  // target not seen this long: stop
static const uint16_t FOLLOW_SWEEP_AFTER_MS = 3000;// ... and after this long start looking around
static const int FOLLOW_MIN_PWM = 40;      // lowest forward PWM that still moves the car
static const int FOLLOW_PIVOT_MIN_PWM = 50;// lowest in-place turn PWM that still turns the car
static const int FOLLOW_SWEEP_DPS = 30;    // search: slow car turn after the camera search found nothing
// Camera pan servo (camera + ultrasonic on one bracket, D10). Servo 90 = straight ahead; PAN_DIR +1: a LARGER servo angle
// looks LEFT (checked on this car in Assignment 4, run 13). If the car turns AWAY from a target found with the camera turned,
// set PAN_DIR = -1.
static const int8_t PAN_DIR = 1;
static const int FOLLOW_PAN_STEP_DEG = 20;   // camera search: look straight, then 20, 40, 60 deg to each side
static const int FOLLOW_PAN_MAX_DEG = 60;
static const uint16_t FOLLOW_PAN_SETTLE_MS = 350; // frames / ultrasonic readings taken while the servo turns are ignored
static const uint16_t FOLLOW_PAN_DWELL_MS = 800;  // time at each camera position (needs at least one settled frame)
static const int FOLLOW_STOP_CM = 15;      // Test18: ultrasonic distance at which the car stops in front of a cone
static const int FOLLOW_SWEEP_MAX_DEG = 400;
