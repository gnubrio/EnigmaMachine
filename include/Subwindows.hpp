#pragma once
#include <ncurses.h>

struct Subwindows {
  const unsigned int MAX_SUBWINDOWS = 4;
  WINDOW *rotors, *lampboard, *keyboard, *plugBoard = nullptr;
};