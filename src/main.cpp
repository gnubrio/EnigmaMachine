#include "Display.hpp"
#include "EnigmaMachine.hpp"
#include <cctype>
#include <ncurses.h>
#include <utility>

int main(void) {
  const unsigned int ESC_KEY = 27;
  const unsigned int SPACE_KEY = 32;

  int keyPress = 0;
  MEVENT mouseEvent;

  WINDOW *windowMain = nullptr;
  Subwindows subwindows;

  EnigmaMachine engimaMachine;

  int error = setupWindows(windowMain, subwindows);
  if (error) {
    endwin();
    return 1;
  }

  do {
    if (getmouse(&mouseEvent) == true) {
      mouseHandler(subwindows, mouseEvent);
    }

    switch (keyPress) {
    case SPACE_KEY:
      drawOutput(subwindows.output, SPACE_KEY);
      break;
    case KEY_BACKSPACE:
      drawOutput(subwindows.output, KEY_BACKSPACE);
      break;
    case 0:
      drawKeyboard(subwindows.keyboard, keyPress);
      drawRotors(subwindows.rotors, engimaMachine);
    }

    if (isalpha(keyPress)) {
      keyPress = toupper(keyPress);
      char encryptedLetter = keyPress;

      engimaMachine.encrypt(encryptedLetter);
      engimaMachine.spinRotors();

      drawKeyboard(subwindows.keyboard, keyPress);
      drawOutput(subwindows.output, encryptedLetter);
      drawRotors(subwindows.rotors, engimaMachine);
    }

    drawSubwindowBoxes(subwindows);
    refreshWindows(windowMain, subwindows);
  } while ((keyPress = getch()) != ESC_KEY);

  endwin();

  return 0;
}