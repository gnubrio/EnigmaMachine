#include "Display.hpp"
#include <array>
#include <chrono>
#include <cmath>
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
  mousemask(ALL_MOUSE_EVENTS | REPORT_MOUSE_POSITION, NULL);
  curs_set(0);

  if (has_colors()) {
    start_color();
    assume_default_colors(-1, -1);
  } else {
    std::cerr << "Terminal does not support color!" << std::endl;
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

void mouseHandler(Subwindows &subwindows, MEVENT &mouseEvent) {
  unsigned int mouseY = mouseEvent.y;
  unsigned int mouseX = mouseEvent.x;

  unsigned int subwindowRotorHeight, subwindowRotorWidth;
  getmaxyx(subwindows.rotors, subwindowRotorHeight, subwindowRotorWidth);

  if (mouseEvent.bstate & BUTTON1_PRESSED) {

  } else if (mouseEvent.bstate & BUTTON3_PRESSED) {
  }
}

void drawSubwindowBoxes(Subwindows &subwindows) {
  box(subwindows.rotors, '|', '-');
  box(subwindows.output, '|', '-');
  box(subwindows.keyboard, '|', '-');
  box(subwindows.plugBoard, '|', '-');
}

void drawKeyboard(WINDOW *windowKeyboard, int keyPress) {
  unsigned int windowHeight, windowWidth = 0;
  getmaxyx(windowKeyboard, windowHeight, windowWidth);

  Keyboard keyboard;
  std::pair<char, std::pair<int, int>> activeKey{0, {0, 0}};

  unsigned int yStep = windowHeight / keyboard.MAX_ROWS;
  unsigned int xStep = 0;

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

void drawRotors(WINDOW *windowRotors, EnigmaMachine &enigmaMachine) {
  const unsigned int MAX_SYMBOLS_COLUMN = 3;
  unsigned int windowHeight, windowWidth = 0;
  getmaxyx(windowRotors, windowHeight, windowWidth);

  std::array<Rotor, EnigmaMachine::MAX_ROTORS_> activeRotors =
      enigmaMachine.getActiveRotors();
  unsigned int rotorCount = 0;
  unsigned int yStep = windowHeight / MAX_SYMBOLS_COLUMN;
  unsigned int xStep = (windowWidth / 2) - EnigmaMachine::MAX_ROTORS_ - 1;

  for (unsigned int i = 0; i < MAX_SYMBOLS_COLUMN; ++i) {
    yStep++;
    xStep = (windowWidth / 2) - EnigmaMachine::MAX_ROTORS_ * 2;

    if (i == MAX_SYMBOLS_COLUMN / 2) {
      wattron(windowRotors, A_BOLD);
    } else {
      wattroff(windowRotors, A_BOLD);
    }

    for (unsigned int j = 0; j < EnigmaMachine::MAX_ROTORS_; ++j) {
      rotorCount = 0;
      mvwprintw(windowRotors, yStep, xStep, "|");
      xStep++;
      mvwprintw(windowRotors, yStep, xStep, "%c",
                activeRotors[rotorCount + j].getActiveSymbol(i - 1));
      xStep++;
      mvwprintw(windowRotors, yStep, xStep, "|");
      xStep++;
      mvwprintw(windowRotors, yStep, xStep, " ");
      xStep++;
    }

    if (rotorCount <= EnigmaMachine::MAX_ROTORS_) {
      rotorCount++;
    } else {
      rotorCount = 0;
    }
  }
}

void drawOutput(WINDOW *windowOutput, int inputKey) {
  wclear(windowOutput);

  unsigned int windowHeight, windowWidth = 0;
  getmaxyx(windowOutput, windowHeight, windowWidth);

  const unsigned int Y_PADDING = 2;
  const unsigned int X_PADDING = 4;
  const unsigned int MAX_HEIGHT_CHARACTERS = windowHeight - Y_PADDING;
  const unsigned int MAX_WIDTH_CHARACTERS = windowWidth - X_PADDING;

  static std::string displayedText;

  if (inputKey == KEY_BACKSPACE && displayedText.length() > 0) {
    if (displayedText.length() > 0) {
      displayedText.pop_back();
    } else {
      return;
    }
  } else if (inputKey != KEY_BACKSPACE) {
    displayedText += inputKey;
  }

  std::vector<std::string> substrings = {};
  unsigned int lines = (unsigned int)std::ceil((float)displayedText.length() /
                                               MAX_WIDTH_CHARACTERS);

  if (displayedText.length() > MAX_WIDTH_CHARACTERS) {
    unsigned int start = 0;
    unsigned int end = 0;
    unsigned int step = displayedText.length() / lines;

    for (unsigned int i = 0; i < lines; ++i) {
      end = step * (i + 1);
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
}