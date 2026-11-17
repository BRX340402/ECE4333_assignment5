#pragma once
#include <atomic>
#include "RedDetector.h"
// Target following, Auto colour (Test22). Colours: 0 green, 1 red, 2 blue.
// The first target is the first colour seen (the largest region if several colours are seen at once). Every next target
// may be any colour EXCEPT the colour just reached (only that one colour is excluded, nothing accumulates): red -> green or
// blue -> ... so the target just reached is never chosen again at once, and courses such as r-g-r-g work.
std::atomic<int> lastReached{-1};   // colour of the target reached last, -1 = none yet
static int chooseColour(const bool seen[3],const int px[3],int last){
 int best=-1,bp=0;
 for(int c=0;c<3;c++)if(c!=last&&seen[c]&&px[c]>bp){best=c;bp=px[c];}
 return best;
}
