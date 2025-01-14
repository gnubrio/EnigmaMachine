#pragma once
#include "Subwindows.hpp"
#include <ncurses.h>
#include <utility>

int setupWindows(WINDOW *windowMain, Subwindows &subwindows);
void drawSubwindowBoxes(Subwindows &subwindows);
void refreshWindows(WINDOW *windowMain, Subwindows &subwindows);

void drawKeyboard(WINDOW *windowKeyboard, char keyPress);
void removeKeyPress(
    WINDOW *windowKeyboard,
    std::pair<char, std::pair<unsigned int, unsigned int>> activeKey);