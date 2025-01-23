#include "Display.hpp"
#include "EnigmaMachine.hpp"
#include <cctype>
#include <ncurses.h>

int main(void) {
  const unsigned int ESC_KEY = 27;
  char keyPress = 0;
  MEVENT mouseEvent;

  WINDOW *windowMain = nullptr;
  Subwindows subwindows;

  EnigmaMachine engimaMachine;

  int error = setupWindows(windowMain, subwindows);
  if (error) {
    endwin();
    return 1;
  }

  bool setup = true;
  do {
    if (setup == true) {
      drawSubwindowBoxes(subwindows);
      setup = false;
    }

    if (isalpha(keyPress) || keyPress == 0) {
      drawKeyboard(subwindows.keyboard, toupper(keyPress));
      engimaMachine.spinRotors();
      drawRotors(subwindows.rotors, engimaMachine);
    }

    if (getmouse(&mouseEvent) == true) {
      mouseHandler(subwindows, mouseEvent);
    }

    refreshWindows(windowMain, subwindows);
  } while ((keyPress = getch()) != ESC_KEY);

  endwin();
  return 0;
}