#include "../include/Display.hpp"
#include "../include/EnigmaMachine.hpp"
#include <cctype>
#include <ncurses.h>

int main(void) {
  WINDOW *windowMain = nullptr;
  Subwindows subwindows;
  EnigmaMachine enigmaMachine = setupEnigmaMachine();

  int error = setupWindows(windowMain, subwindows);
  if (error) {
    endwin();
    return 1;
  }

  const int ESC_KEY = 27;
  const int SPACE_KEY = 32;
  const int ENTER_KEY = 10;

  int keyPress = 0;

  do {
    if (isalpha(keyPress)) {
      keyPress = toupper(keyPress);
      char encryptedLetter = keyPress;
      enigmaMachine.encrypt(encryptedLetter);

      drawKeyboard(subwindows.keyboard, keyPress);
      bool shouldSpin = drawOutput(subwindows.output, encryptedLetter);
      if (shouldSpin) {
        enigmaMachine.spinRotors(-1);
        drawRotors(subwindows.rotors, enigmaMachine);
      }

      drawPlugBoard(subwindows.plugBoard, enigmaMachine);
    } else if (keyPress == SPACE_KEY) {
      drawOutput(subwindows.output, SPACE_KEY);
    } else if (keyPress == KEY_BACKSPACE) {
      bool shouldSpin = drawOutput(subwindows.output, KEY_BACKSPACE);
      if (shouldSpin) {
        enigmaMachine.spinRotors(1);
        drawRotors(subwindows.rotors, enigmaMachine);
      }
    } else if (keyPress == ESC_KEY) {
      bool reset = escapeMenu(subwindows.output, enigmaMachine, ENTER_KEY);

      wclear(subwindows.output);
      drawOutput(subwindows.output, 0, reset);
      if (reset) {
        drawRotors(subwindows.rotors, enigmaMachine);
        drawPlugBoard(subwindows.plugBoard, enigmaMachine);
      }
    } else if (keyPress == KEY_RESIZE) {
      keyPress = 0;
      clearWindows(windowMain, subwindows);
      drawKeyboard(subwindows.keyboard, keyPress);
      drawOutput(subwindows.output, keyPress);
      drawRotors(subwindows.rotors, enigmaMachine);
      drawPlugBoard(subwindows.plugBoard, enigmaMachine);
    } else if (keyPress == 0) {
      drawKeyboard(subwindows.keyboard, keyPress);
      drawRotors(subwindows.rotors, enigmaMachine);
      drawPlugBoard(subwindows.plugBoard, enigmaMachine);
    }
    switch (keyPress) {
    case KEY_UP:
      rotorConfigMenu(subwindows.rotors, enigmaMachine, ESC_KEY, ENTER_KEY);

      wclear(subwindows.rotors);
      drawRotors(subwindows.rotors, enigmaMachine);
      break;
    case KEY_DOWN:
      plugBoardConfigMenu(subwindows.plugBoard, enigmaMachine, ESC_KEY);

      wclear(subwindows.plugBoard);
      drawPlugBoard(subwindows.plugBoard, enigmaMachine);
      break;
    }

    drawSubwindowBoxes(subwindows);
    refreshWindows(windowMain, subwindows);
  } while ((keyPress = getch()));

  endwin();
  return 0;
}
