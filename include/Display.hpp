#pragma once
#include "Subwindows.hpp"
#include <ncurses.h>

int setupWindows(WINDOW *windowMain, Subwindows &subwindows);
void drawSubwindowBoxes(Subwindows &subwindows);
void refreshWindows(WINDOW *windowMain, Subwindows &subwindows);
void drawKeyboard(WINDOW *windowKeyboard, char keyPress);
