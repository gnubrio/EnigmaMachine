#include "Display.hpp"
#include <array>
#include <chrono>
#include <iostream>
#include <mutex>
#include <ncurses.h>
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
  subwindows.lampboard = subwin(windowMain, subwindowHeight, terminalWidth,
                                subwindowYPositions[1], 0);
  subwindows.keyboard = subwin(windowMain, subwindowHeight, terminalWidth,
                               subwindowYPositions[2], 0);
  subwindows.plugBoard = subwin(windowMain, subwindowHeight, terminalWidth,
                                subwindowYPositions[3], 0);

  touchwin(windowMain);
  refresh();
  return 0;
}

void drawSubwindowBoxes(Subwindows &subwindows) {
  box(subwindows.rotors, '|', '-');
  box(subwindows.lampboard, '|', '-');
  box(subwindows.keyboard, '|', '-');
  box(subwindows.plugBoard, '|', '-');
}

void refreshWindows(WINDOW *windowMain, Subwindows &subwindows) {
  wrefresh(windowMain);
  wrefresh(subwindows.rotors);
  wrefresh(subwindows.lampboard);
  wrefresh(subwindows.keyboard);
  wrefresh(subwindows.plugBoard);
}

void drawKeyboard(WINDOW *windowKeyboard, char keyPress) {
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
  char key = activeKey.first;
  unsigned int yCoord = activeKey.second.first;
  unsigned int xCoord = activeKey.second.second;

  std::mutex activeKeysMutex;
  std::unique_lock<std::mutex> activeKeyLock(activeKeysMutex);

  std::this_thread::sleep_for(std::chrono::seconds(1));

  mvwprintw(windowKeyboard, yCoord, xCoord, "%c", key);
  wrefresh(windowKeyboard);

  activeKeyLock.unlock();
}
