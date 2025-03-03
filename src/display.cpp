#include "../include/Display.hpp"
#include <cmath>
#include <cstdlib>
#include <mutex>
#include <ncurses.h>
#include <set>
#include <string>
#include <thread>
#include <utility>
#include <vector>

int setupWindows(WINDOW *windowMain, Subwindows &subwindows) {
  initscr();
  cbreak();
  noecho();
  keypad(stdscr, TRUE);
  curs_set(0);
  ESCDELAY = 1;

  if (has_colors()) {
    start_color();
    assume_default_colors(-1, -1);

    init_pair(1, COLOR_BLACK, COLOR_WHITE);
    init_pair(2, COLOR_BLACK, COLOR_GREEN);
  } else {
    return 1;
  }

  unsigned int terminalHeight, terminalWidth = 0;
  getmaxyx(stdscr, terminalHeight, terminalWidth);

  windowMain = newwin(terminalHeight, terminalWidth, 0, 0);

  unsigned int subwindowHeight = terminalHeight / subwindows.MAX_SUBWINDOWS;
  std::vector<unsigned int> subwindowYPositions;
  subwindowYPositions.reserve(4);
  for (unsigned int i = 0; i < subwindows.MAX_SUBWINDOWS; ++i) {
    subwindowYPositions.push_back(subwindowHeight * i);
  }

  subwindows.rotors = subwin(windowMain, subwindowHeight, terminalWidth,
                             subwindowYPositions[0], 0);
  subwindows.output = subwin(windowMain, subwindowHeight, terminalWidth,
                             subwindowYPositions[1], 0);
  subwindows.keyboard = subwin(windowMain, subwindowHeight, terminalWidth,
                               subwindowYPositions[2], 0);
  subwindows.plugBoard = subwin(windowMain, subwindowHeight, terminalWidth,
                                subwindowYPositions[3], 0);

  touchwin(windowMain);
  refresh();
  return 0;
}

void refreshWindows(WINDOW *windowMain, Subwindows &subwindows) {
  wrefresh(windowMain);
  wrefresh(subwindows.rotors);
  wrefresh(subwindows.output);
  wrefresh(subwindows.keyboard);
  wrefresh(subwindows.plugBoard);
}

void clearWindows(WINDOW *windowMain, Subwindows &subwindows) {
  wclear(windowMain);
  wclear(subwindows.rotors);
  wclear(subwindows.output);
  wclear(subwindows.keyboard);
  wclear(subwindows.plugBoard);
}

void drawSubwindowBoxes(Subwindows &subwindows) {
  box(subwindows.rotors, '|', '-');
  box(subwindows.output, '|', '-');
  box(subwindows.keyboard, '|', '-');
  box(subwindows.plugBoard, '|', '-');
}

void highlightSubwindow(WINDOW *subwindow) {
  wattron(subwindow, A_BOLD);
  box(subwindow, '|', '-');
  wattroff(subwindow, A_BOLD);
}

bool escapeMenu(WINDOW *windowOutput, EnigmaMachine &enigmaMachine,
                const int ESC_KEY, const int ENTER_KEY) {
  wclear(windowOutput);
  highlightSubwindow(windowOutput);

  unsigned int windowHeight, windowWidth = 0;
  getmaxyx(windowOutput, windowHeight, windowWidth);

  std::array<std::string, 3> selections = {"Resume", "Reset", "Exit"};
  unsigned int yStep = windowHeight / selections.size();
  unsigned int xStep = windowWidth / 2;
  unsigned int longestSelectionName = 1;
  for (const auto &selection : selections) {
    unsigned int length = selection.length();
    if (length > longestSelectionName) {
      longestSelectionName = length;
    }
  }

  int keyPress = 0;
  unsigned int selection = 0;
  bool reset = false;
  do {
    switch (keyPress) {
    case KEY_UP:
      if (selection > 0) {
        selection--;
      }
      break;
    case KEY_DOWN:
      if (selection < selections.size() - 1) {
        selection++;
      }
      break;
    }
    if (keyPress == ENTER_KEY) {
      if (selection == 0) {
        break;
      } else if (selection == 1) {
        enigmaMachine = setupEnigmaMachine();
        reset = true;
      } else if (selection == 2) {
        endwin();
        exit(0);
      }
    } else if (keyPress == ESC_KEY) {
      break;
    } else if (keyPress == KEY_RESIZE) {
      break;
    }

    for (size_t i = 0; i < selections.size(); ++i) {
      if (selection == i) {
        wattron(windowOutput, COLOR_PAIR(1));
      } else {
        wattrset(windowOutput, A_NORMAL);
      }
      mvwprintw(windowOutput, yStep + i, xStep - (longestSelectionName / 2),
                "%s", selections[i].c_str());
    }

    wrefresh(windowOutput);
  } while ((keyPress = getch()));
  wattrset(windowOutput, A_NORMAL);
  return reset;
}

void rotorConfigMenu(WINDOW *windowRotors, EnigmaMachine &enigmaMachine,
                     const int ESC_KEY, const int ENTER_KEY) {
  wclear(windowRotors);
  highlightSubwindow(windowRotors);

  unsigned int windowHeight, windowWidth = 0;
  getmaxyx(windowRotors, windowHeight, windowWidth);

  struct Button {
    Button(unsigned int index, unsigned int y, unsigned int x)
        : index(index), y(y), x(x) {}

    unsigned int index, row = 0;
    unsigned int y, x = 1;
    bool isSelected = false;
  };

  std::vector<Button> buttons;
  buttons.reserve(enigmaMachine.MAX_ROTORS_);

  std::vector<Rotor> allRotors = enigmaMachine.getAvaliableRotors();
  unsigned int allRotorsSize = allRotors.size();
  std::vector<Rotor> activeRotors = enigmaMachine.getActiveRotors();

  unsigned int longestModelName = 0;
  for (const auto &rotor : allRotors) {
    unsigned int modelNameLength = rotor.getModelName().length();
    if (modelNameLength > longestModelName) {
      longestModelName = modelNameLength;
    }
  }

  unsigned int buttonY = (windowHeight / 2) - (allRotorsSize / 2);
  unsigned int buttonX = (windowWidth / (enigmaMachine.MAX_ROTORS_ + 1)) -
                         (longestModelName / enigmaMachine.MAX_ROTORS_);

  for (size_t i = 0; i < enigmaMachine.MAX_ROTORS_; ++i) {
    Button button =
        Button(static_cast<unsigned int>(i), buttonY, buttonX * (i + 1));
    buttons.push_back(button);
  }

  if (!buttons.empty()) {
    buttons[0].isSelected = true;
  } else {
    return;
  }

  int keyPress = 0;
  Button *buttonPtr = &buttons[0];
  do {
    switch (keyPress) {
    case KEY_UP:
      if (buttonPtr->row > 0) {
        buttonPtr->row--;
      }
      break;
    case KEY_RIGHT:
      if (buttonPtr->index < enigmaMachine.MAX_ROTORS_ - 1) {
        unsigned int buttonRow = buttonPtr->row;
        buttonPtr->isSelected = false;
        buttonPtr = &buttons[buttonPtr->index + 1];
        buttonPtr->row = buttonRow;
        buttonPtr->isSelected = true;
      }
      break;
    case KEY_DOWN:
      if (buttonPtr->row < allRotorsSize - 1) {
        buttonPtr->row++;
      }
      break;
    case KEY_LEFT:
      if (buttonPtr->index > 0) {
        unsigned int buttonRow = buttonPtr->row;
        buttonPtr->isSelected = false;
        buttonPtr = &buttons[buttonPtr->index - 1];
        buttonPtr->row = buttonRow;
        buttonPtr->isSelected = true;
      }
      break;
    }
    if (keyPress == ENTER_KEY) {
      enigmaMachine.setRotor(allRotors[buttonPtr->row],
                             activeRotors[buttonPtr->index], buttonPtr->index);
      activeRotors = enigmaMachine.getActiveRotors();
    } else if (keyPress == KEY_RESIZE) {
      break;
    }

    for (size_t i = 0; i < enigmaMachine.MAX_ROTORS_; ++i) {
      wattron(windowRotors, A_BOLD);
      mvwprintw(windowRotors, buttons[i].y, buttons[i].x, "Slot: %zu", i + 1);
      wattroff(windowRotors, A_BOLD);

      for (size_t j = 0; j < allRotorsSize; ++j) {
        if (buttons[i].isSelected && buttons[i].row == j) {
          wattron(windowRotors, COLOR_PAIR(1));
        } else if (activeRotors[i].getModelName() ==
                   allRotors[j].getModelName()) {
          wattron(windowRotors, COLOR_PAIR(2));
        } else {
          wattroff(windowRotors, COLOR_PAIR(1));
        }
        mvwprintw(windowRotors, (buttons[i].y + 1) + j, buttons[i].x, "%s",
                  allRotors[j].getModelName().c_str());
        wattrset(windowRotors, A_NORMAL);
      }
    }
    wrefresh(windowRotors);
  } while ((keyPress = getch()) != ESC_KEY);
}

void plugBoardConfigMenu(WINDOW *windowPlugBoard, EnigmaMachine &enigmaMachine,
                         const int ESC_KEY) {
  wclear(windowPlugBoard);
  highlightSubwindow(windowPlugBoard);

  unsigned int windowHeight, windowWidth = 0;
  getmaxyx(windowPlugBoard, windowHeight, windowWidth);

  struct Button {
    unsigned int index = 0;
    unsigned int row = 0;
    unsigned int rowsHeight = Cable::MAX_PLUGS;
    unsigned int y, x = 1;
    bool arrow = 0;
  };

  std::vector<Cable> allCables = enigmaMachine.getActivePlugs();
  std::string cableID = "Plug ";
  unsigned int longestCableName = cableID.length() + 1;

  unsigned int buttonY = (windowHeight / 2) - Cable::MAX_PLUGS;
  unsigned int buttonX = (windowWidth / 2) - (longestCableName / 2);

  Button button;
  button.y = buttonY;
  button.x = buttonX;

  int keyPress = 0;
  do {
    switch (keyPress) {
    case KEY_UP:
      if (button.row > 0) {
        button.row--;
      }
      break;
    case KEY_RIGHT:
      if (button.row < Cable::MAX_PLUGS) {
        enigmaMachine.setPlug(button.index, button.row, 1);
      } else if (button.row == button.rowsHeight &&
                 button.index < allCables.size() - 1) {
        button.index++;
        button.arrow = 1;
      }
      break;
    case KEY_DOWN:
      if (button.row < button.rowsHeight) {
        button.row++;
      }
      break;
    case KEY_LEFT:
      if (button.row < Cable::MAX_PLUGS) {
        enigmaMachine.setPlug(button.index, button.row, -1);
      } else if (button.row == button.rowsHeight && button.index > 0) {
        button.index--;
        button.arrow = 0;
      }
      break;
    }
    if (keyPress == KEY_RESIZE) {
      break;
    }

    wattron(windowPlugBoard, A_BOLD);
    mvwprintw(windowPlugBoard, button.y, button.x, "%s",
              (cableID + std::to_string(button.index + 1) + " ").c_str());

    char plug;
    unsigned int yStep = button.y + 1;
    unsigned int xStep = button.x;
    for (unsigned int i = 0; i < Cable::MAX_PLUGS; ++i) {
      allCables = enigmaMachine.getActivePlugs();
      wattrset(windowPlugBoard, A_NORMAL);
      xStep = button.x;

      if (i % 2 == 0) {
        plug = allCables[button.index].output;
      } else {
        plug = allCables[button.index].input;
      }
      if (plug == '\0') {
        plug = ' ';
      }

      if (button.row == i) {
        wattron(windowPlugBoard, COLOR_PAIR(1));
      }
      mvwprintw(windowPlugBoard, yStep, xStep, "|");
      xStep++;
      mvwprintw(windowPlugBoard, yStep, xStep, "%c", plug);
      xStep++;
      mvwprintw(windowPlugBoard, yStep, xStep, "|");
      yStep++;
    }

    wattrset(windowPlugBoard, A_NORMAL);
    if (button.row == button.rowsHeight && button.arrow == 0) {
      wattron(windowPlugBoard, COLOR_PAIR(1));
      mvwprintw(windowPlugBoard, yStep, xStep, "<");
      wattroff(windowPlugBoard, COLOR_PAIR(1));
      xStep += (longestCableName - 1);
      mvwprintw(windowPlugBoard, yStep, xStep, ">");
    } else if (button.row == button.rowsHeight && button.arrow == 1) {
      mvwprintw(windowPlugBoard, yStep, xStep, "<");
      xStep += (longestCableName - 1);
      wattron(windowPlugBoard, COLOR_PAIR(1));
      mvwprintw(windowPlugBoard, yStep, xStep, ">");
      wattroff(windowPlugBoard, COLOR_PAIR(1));
    } else {
      mvwprintw(windowPlugBoard, yStep, xStep, "<");
      xStep += (longestCableName - 1);
      mvwprintw(windowPlugBoard, yStep, xStep, ">");
    }

    wrefresh(windowPlugBoard);
  } while ((keyPress = getch()) != ESC_KEY);
}

void drawKeyboard(WINDOW *windowKeyboard, const int keyPress) {
  unsigned int windowHeight, windowWidth = 0;
  getmaxyx(windowKeyboard, windowHeight, windowWidth);

  struct Keyboard {
    const unsigned int MAX_ROWS = 3;
    const std::array<char, 9> topRow = {'Q', 'W', 'E', 'R', 'T',
                                        'Z', 'U', 'I', 'O'};
    const std::array<char, 8> middleRow = {'A', 'S', 'D', 'F',
                                           'G', 'H', 'J', 'K'};
    const std::array<char, 9> bottomRow = {'P', 'Y', 'X', 'C', 'V',
                                           'B', 'N', 'M', 'L'};
  };

  Keyboard keyboard;
  std::pair<char, std::pair<int, int>> activeKey{0, {0, 0}};

  unsigned int yStep = windowHeight / keyboard.MAX_ROWS;
  int xStep = 0;

  auto draw = [&](auto &row) {
    xStep = windowWidth / 2 - row.size();
    for (const auto key : row) {
      if (key == keyPress) {
        wattron(windowKeyboard, A_DIM);
        mvwprintw(windowKeyboard, yStep, xStep, "%c", key);
        wattroff(windowKeyboard, A_DIM);
        xStep++;
        mvwprintw(windowKeyboard, yStep, xStep, " ");
        xStep++;

        activeKey = std::make_pair(key, std::make_pair(yStep, xStep - 2));
      } else {
        mvwprintw(windowKeyboard, yStep, xStep, "%c", key);
        xStep++;
        mvwprintw(windowKeyboard, yStep, xStep, " ");
        xStep++;
      }
    }
    yStep++;
  };

  draw(keyboard.topRow);
  draw(keyboard.middleRow);
  draw(keyboard.bottomRow);

  std::thread keyPressWorker(removeKeyPress, windowKeyboard, activeKey);
  keyPressWorker.detach();
}

void removeKeyPress(
    WINDOW *windowKeyboard,
    const std::pair<char, std::pair<unsigned int, unsigned int>> activeKey) {
  static std::mutex activeKeyMutex;
  static std::set<char> activeKeys;

  char key = activeKey.first;

  {
    std::lock_guard<std::mutex> activeKeyLock(activeKeyMutex);
    if (activeKeys.count(key)) {
      return;
    }
    activeKeys.insert(key);
  }

  std::this_thread::sleep_for(std::chrono::seconds(1));

  {
    unsigned int yCoord = activeKey.second.first;
    unsigned int xCoord = activeKey.second.second;

    std::lock_guard<std::mutex> lock(activeKeyMutex);
    mvwprintw(windowKeyboard, yCoord, xCoord, "%c", key);
    activeKeys.erase(key);
  }

  wrefresh(windowKeyboard);
}

void drawRotors(WINDOW *windowRotors, const EnigmaMachine &enigmaMachine) {
  unsigned int windowHeight, windowWidth = 0;
  getmaxyx(windowRotors, windowHeight, windowWidth);

  const unsigned int MAX_SYMBOLS_COLUMN = 3;

  std::vector<Rotor> activeRotors = enigmaMachine.getActiveRotors();

  unsigned int yStep = windowHeight / MAX_SYMBOLS_COLUMN;

  for (unsigned int i = 0; i < MAX_SYMBOLS_COLUMN; ++i) {
    unsigned int xStep = (windowWidth / 2) - EnigmaMachine::MAX_ROTORS_ * 2;

    if (i == MAX_SYMBOLS_COLUMN / 2) {
      wattron(windowRotors, A_BOLD);
    } else {
      wattroff(windowRotors, A_BOLD);
    }

    for (unsigned int j = 0; j < EnigmaMachine::MAX_ROTORS_; ++j) {
      mvwprintw(windowRotors, yStep, xStep, "|");
      xStep++;
      mvwprintw(windowRotors, yStep, xStep, "%c",
                activeRotors[j].getActiveSymbol(i - 1));
      xStep++;
      mvwprintw(windowRotors, yStep, xStep, "|");
      xStep++;
      mvwprintw(windowRotors, yStep, xStep, " ");
      xStep++;
    }
    yStep++;
  }
}

void drawPlugBoard(WINDOW *windowPlugBoard,
                   const EnigmaMachine &enigmaMachine) {
  unsigned int windowHeight, windowWidth = 0;
  getmaxyx(windowPlugBoard, windowHeight, windowWidth);

  std::vector<Cable> activePlugs = enigmaMachine.getActivePlugs();

  const unsigned int PLUG_WIDTH = 4;

  char plug;
  unsigned int yStep = (windowHeight / 2) - 1;
  for (unsigned int i = 0; i < Cable::MAX_PLUGS; ++i) {
    unsigned int xStep =
        (windowWidth / 2) - ((activePlugs.size() * PLUG_WIDTH) / 2);

    for (size_t j = 0; j < activePlugs.size(); ++j) {
      if (activePlugs[j].input == '\0' || activePlugs[j].output == '\0') {
        plug = ' ';
      } else if (i % 2 == 0) {
        plug = activePlugs[j].output;
      } else {
        plug = activePlugs[j].input;
      }
      mvwprintw(windowPlugBoard, yStep, xStep, "|");
      xStep++;
      mvwprintw(windowPlugBoard, yStep, xStep, "%c", plug);
      xStep++;
      mvwprintw(windowPlugBoard, yStep, xStep, "|");
      xStep++;
      mvwprintw(windowPlugBoard, yStep, xStep, " ");
      xStep++;
    }
    yStep++;
  }
}

bool drawOutput(WINDOW *windowOutput, const int inputKey, const bool reset) {
  unsigned int windowHeight, windowWidth = 0;
  getmaxyx(windowOutput, windowHeight, windowWidth);

  const unsigned int Y_PADDING = 2;
  const unsigned int X_PADDING = 4;
  const unsigned int MAX_HEIGHT_CHARACTERS = windowHeight - (Y_PADDING * 2);
  const unsigned int MAX_WIDTH_CHARACTERS = windowWidth - (X_PADDING * 2);

  static std::string displayedText;
  if (reset) {
    displayedText = "";
  }
  bool spinRotor = true;
  std::vector<std::string> substrings = {};
  unsigned int lines = (unsigned int)std::ceil((float)displayedText.length() /
                                               MAX_WIDTH_CHARACTERS);

  if (inputKey == KEY_BACKSPACE) {
    wclear(windowOutput);
    if (displayedText.length() > 0) {
      if (displayedText.back() == ' ') {
        spinRotor = false;
      }
      displayedText.pop_back();
    } else {
      spinRotor = false;
      return spinRotor;
    }
  } else if (inputKey != KEY_BACKSPACE && inputKey != 0) {
    if (lines > MAX_HEIGHT_CHARACTERS) {
      spinRotor = false;
      return spinRotor;
    }
    displayedText += inputKey;
  }

  if (displayedText.length() > MAX_WIDTH_CHARACTERS) {
    unsigned int start = 0;
    for (unsigned int i = 0; i < lines; ++i) {
      std::string substring = displayedText.substr(start, MAX_WIDTH_CHARACTERS);
      start += MAX_WIDTH_CHARACTERS;
      substrings.push_back(substring);
    }
  } else {
    substrings.push_back(displayedText);
  }

  for (size_t i = 0; i < substrings.size(); ++i) {
    mvwprintw(windowOutput, (Y_PADDING + i), X_PADDING, "%s",
              substrings[i].c_str());
  }

  return spinRotor;
}
