#pragma once
// Old drivers use int[][2]; new drivers use a width/height struct.
template<class T> static auto sizeWidth(const T &s)->decltype(s.width){return s.width;}
template<class T> static auto sizeHeight(const T &s)->decltype(s.height){return s.height;}
template<class T,unsigned N> static int sizeWidth(const T (&s)[N]){return s[0];}
template<class T,unsigned N> static int sizeHeight(const T (&s)[N]){return s[1];}
