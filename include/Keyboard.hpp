#pragma once
#include <array>

struct Keyboard {
  const unsigned int MAX_ROWS = 3;
  const std::array<char, 9> topRow = {'Q', 'W', 'E', 'R', 'T', 'Z', 'U', 'I', 'O'};
  const std::array<char, 8> middleRow = {'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K'};
  const std::array<char, 9> bottomRow = {'P', 'Y', 'X', 'C', 'V', 'B', 'N', 'M', 'L'};
};