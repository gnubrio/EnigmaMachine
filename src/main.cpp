#include "Display.hpp"
#include <cctype>
#include <ncurses.h>

int main(void) {
  const unsigned int ESC_KEY = 27;
  char keyPress = 0;

  WINDOW *windowMain = nullptr;
  Subwindows subwindows;

  int error = setupWindows(windowMain, subwindows);
  if (error) {
    endwin();
    return 1;
  }

  do {
    drawSubwindowBoxes(subwindows);
    if (isalpha(keyPress) || keyPress == 0) {
      drawKeyboard(subwindows.keyboard, toupper(keyPress));
    }
    refreshWindows(windowMain, subwindows);
  } while ((keyPress = getch()) != ESC_KEY);

  endwin();
  return 0;
}