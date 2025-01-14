#include "Display.hpp"
#include "Subwindows.hpp"
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

  refresh();

  do {
    drawSubwindowBoxes(subwindows);
    drawKeyboard(subwindows.keyboard, toupper(keyPress));
    refreshWindows(windowMain, subwindows);
  } while ((keyPress = getch()) != ESC_KEY);

  endwin();
  return 0;
}