#pragma once
#include <stdint.h>
#define PROGMEM
#define PSTR(x) (x)
#define pgm_read_byte(p) (*(const uint8_t*)(p))
#define pgm_read_word(p) (*(const uint16_t*)(p))
#define strlen_P strlen
