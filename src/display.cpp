#include "../include/Display.hpp"
#include <cmath>
#include <cstdlib>
#include <iostream>
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

  if (has_colors()) {
    start_color();
    assume_default_colors(-1, -1);

    init_pair(1, COLOR_BLACK, COLOR_WHITE);
    init_pair(2, COLOR_BLACK, COLOR_GREEN);
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

void rotorConfigMenu(WINDOW *windowRotors, EnigmaMachine &enigmaMachine,
                     const int ESC_KEY, const int ENTER_KEY) {
  wclear(windowRotors);
  highlightSubwindow(windowRotors);

  int keyPress = 0;
  unsigned int windowHeight, windowWidth = 0;
  getmaxyx(windowRotors, windowHeight, windowWidth);

  struct Button {
    Button(unsigned int index, unsigned int y, unsigned int x,
           unsigned int height, unsigned int width)
        : index(index), y(y), x(x), height(height), width(width) {}

    unsigned int index, row = 0;
    unsigned int y, x, height, width = 1;
    bool isSelected = false;
  };
  std::vector<Button> buttons;
  buttons.reserve(3);

  std::vector<Rotor> allRotors = enigmaMachine.getAvaliableRotors();
  std::vector<Rotor> activeRotors = enigmaMachine.getActiveRotors();
  unsigned int longestModelName = 0;
  for (const auto &rotor : allRotors) {
    unsigned int modelNameLength = rotor.getModelName().length();
    if (modelNameLength > longestModelName) {
      longestModelName = modelNameLength;
    }
  }

  unsigned int yStep = (windowHeight / 2) - (allRotors.size() / 2);
  unsigned int xStep =
      (windowWidth - (longestModelName * enigmaMachine.MAX_ROTORS_)) /
      (enigmaMachine.MAX_ROTORS_ + 1);

  for (size_t i = 0; i < enigmaMachine.MAX_ROTORS_; ++i) {
    Button button = Button(static_cast<unsigned int>(i), yStep, xStep * (i + 1),
                           allRotors.size() + 1, longestModelName);
    buttons.push_back(button);
  }

  if (!buttons.empty()) {
    buttons[0].isSelected = true;
  } else {
    return;
  }

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
      if (buttonPtr->row < allRotors.size() - 1) {
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
    }

    for (size_t i = 0; i < buttons.size(); ++i) {
      wattrset(windowRotors, A_NORMAL);

      if (buttons[i].index == i) {
        wattron(windowRotors, A_BOLD);
      } else {
        wattroff(windowRotors, A_BOLD);
      }
      mvwprintw(windowRotors, buttons[i].y, buttons[i].x, "Slot: %zu", i + 1);

      for (size_t j = 0; j < allRotors.size(); ++j) {
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
      }
    }
    wrefresh(windowRotors);
  } while ((keyPress = getch()) != ESC_KEY);
}

void drawKeyboard(WINDOW *windowKeyboard, int keyPress) {
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
    std::pair<char, std::pair<unsigned int, unsigned int>> activeKey) {
  static std::mutex activeKeyMutex;
  static std::set<char> activeKeys;

  char key = activeKey.first;
  unsigned int yCoord = activeKey.second.first;
  unsigned int xCoord = activeKey.second.second;

  {
    std::lock_guard<std::mutex> activeKeyLock(activeKeyMutex);
    if (activeKeys.count(key)) {
      return;
    }
    activeKeys.insert(key);
  }

  std::this_thread::sleep_for(std::chrono::seconds(1));
  mvwprintw(windowKeyboard, yCoord, xCoord, "%c", key);

  activeKeys.erase(key);
}

void drawRotors(WINDOW *windowRotors, const EnigmaMachine &enigmaMachine) {
  const unsigned int MAX_SYMBOLS_COLUMN = 3;
  unsigned int windowHeight, windowWidth = 0;
  getmaxyx(windowRotors, windowHeight, windowWidth);

  std::vector<Rotor> activeRotors = enigmaMachine.getActiveRotors();

  unsigned int yStep = windowHeight / MAX_SYMBOLS_COLUMN;

  for (unsigned int i = 0; i < MAX_SYMBOLS_COLUMN; ++i) {
    yStep++;
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
  }
}

bool drawOutput(WINDOW *windowOutput, int inputKey) {
  wclear(windowOutput);

  unsigned int windowHeight, windowWidth = 0;
  getmaxyx(windowOutput, windowHeight, windowWidth);

  const unsigned int Y_PADDING = 2;
  const unsigned int X_PADDING = 4;
  const unsigned int MAX_HEIGHT_CHARACTERS = windowHeight - Y_PADDING;
  const unsigned int MAX_WIDTH_CHARACTERS = windowWidth - X_PADDING;

  static std::string displayedText;
  bool spinRotor = true;

  if (inputKey == KEY_BACKSPACE) {
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
    displayedText += inputKey;
  }

  std::vector<std::string> substrings = {};
  unsigned int lines = (unsigned int)std::ceil((float)displayedText.length() /
                                               MAX_WIDTH_CHARACTERS);

  if (displayedText.length() > MAX_WIDTH_CHARACTERS) {
    unsigned int start = 0;
    unsigned int step = displayedText.length() / lines;

    for (unsigned int i = 0; i < lines; ++i) {
      unsigned int end = step * (i + 1);
      std::string substring = displayedText.substr(start, end);
      substrings.push_back(substring);
      start = end;
    }
  } else {
    substrings.push_back(displayedText);
  }

  unsigned int yStep = (windowHeight / 2) - (lines / 2);
  for (const auto &substring : substrings) {
    unsigned int xStep = (windowWidth / 2) - (substring.length() / 2);
    for (unsigned long int j = 0; j < substring.length(); ++j) {
      mvwprintw(windowOutput, yStep, xStep, "%c", substring[j]);
      xStep++;
    }
    yStep++;
  }

  return spinRotor;
}
