#pragma once
#include "EnigmaMachine.hpp"
#include <array>
#include <ncurses.h>
#include <utility>

struct Subwindows {
  const unsigned int MAX_SUBWINDOWS = 4;
  WINDOW *rotors, *lampboard, *keyboard, *plugBoard = nullptr;
};

struct Keyboard {
  const unsigned int MAX_ROWS = 3;
  const std::array<char, 9> topRow = {'Q', 'W', 'E', 'R', 'T',
                                      'Z', 'U', 'I', 'O'};
  const std::array<char, 8> middleRow = {'A', 'S', 'D', 'F',
                                         'G', 'H', 'J', 'K'};
  const std::array<char, 9> bottomRow = {'P', 'Y', 'X', 'C', 'V',
                                         'B', 'N', 'M', 'L'};
};

struct RotorDisplay {
  
};

int setupWindows(WINDOW *windowMain, Subwindows &subwindows);
void refreshWindows(WINDOW *windowMain, Subwindows &subwindows);
void mouseHandler(Subwindows &subwindows, MEVENT &mouseEvent);

void drawSubwindowBoxes(Subwindows &subwindows);
void drawKeyboard(WINDOW *windowKeyboard, char keyPress);
void removeKeyPress(
    WINDOW *windowKeyboard,
    std::pair<char, std::pair<unsigned int, unsigned int>> activeKey);
void drawRotors(WINDOW *windowRotors, EnigmaMachine &enigmaMachine);