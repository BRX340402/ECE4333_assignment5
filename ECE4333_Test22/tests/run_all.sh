#!/bin/bash
# Host tests for the Test22 UNO firmware (run on a PC with g++):
#   cone_test      cone arrival / next cone (TargetFollow.h)
#   overtake_test  no-scan obstacle avoidance, 2-D simulation: legs / turns = web values, page clearance check, +-15 % speed error
#   next_target_test  target following: which region may be the next target after one is reached (ESP32 NextTarget.h)
#   link_test      ESP32 -> UNO commands end to end through the real sketch (stubs for the hardware)
set -e
cd "$(dirname "$0")"
OUT=$(mktemp -d)
g++ -std=c++11 -O2 -I stubs -I ../UNO_Mapping_Assignment5 cone_test.cpp -o $OUT/cone && $OUT/cone | tail -1
g++ -std=c++11 -O2 -I ../UNO_Mapping_Assignment5 overtake_test.cpp -o $OUT/ovt && $OUT/ovt | tail -1
g++ -std=c++11 -O2 -I ../ESP32_Assignment4 next_target_test.cpp -o $OUT/nt && $OUT/nt | tail -1
# link test: the sketch with the gyro / LED drivers swapped for stubs
mkdir -p $OUT/uno && cp ../UNO_Mapping_Assignment5/* $OUT/uno/
sed -i 's/#include "PoseEstimate.h"/#include "PoseStub.h"/; s/#include "KeyIrLed.h"/#include "KeyStub.h"/' $OUT/uno/UNO_Mapping_Assignment5.ino
g++ -std=gnu++11 -O1 -w -I . -I stubs -I $OUT/uno link_test.cpp -o $OUT/link && $OUT/link
rm -rf $OUT
