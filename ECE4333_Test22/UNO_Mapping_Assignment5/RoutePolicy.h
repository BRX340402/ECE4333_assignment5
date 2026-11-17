#pragma once
#include <stdint.h>
// STOP-sign zone: segments E-O (2) and C-O (6) have a STOP sign before O, until that stop is done.
// Returns 2 (STOP sign zone) or 0 (no vision rule active).
static inline uint8_t routeFeature(uint8_t seg,bool stopDone){
 return ((seg==2||seg==6)&&!stopDone)?2:0;
}
