#pragma once
#include "../include/EnigmaMachine.hpp"
#include <ncurses.h>
#include <utility>

struct Subwindows {
  const unsigned int MAX_SUBWINDOWS = 4;
  WINDOW *rotors, *output, *keyboard, *plugBoard = nullptr;
};

int setupWindows(WINDOW *windowMain, Subwindows &subwindows);
void refreshWindows(WINDOW *windowMain, Subwindows &subwindows);
void clearWindows(WINDOW *windowMain, Subwindows &subwindows);

void drawSubwindowBoxes(Subwindows &subwindows);
void highlightSubwindow(WINDOW *subwindow);

void rotorConfigMenu(WINDOW *windowRotors, EnigmaMachine &enigmaMachine,
                     const int ESC_KEY, const int ENTER_KEY);

void drawKeyboard(WINDOW *windowKeyboard, int keyPress);
void removeKeyPress(
    WINDOW *windowKeyboard,
    std::pair<char, std::pair<unsigned int, unsigned int>> activeKey);

void drawRotors(WINDOW *windowRotors, const EnigmaMachine &enigmaMachine);

bool drawOutput(WINDOW *windowOutput, int inputKey);
