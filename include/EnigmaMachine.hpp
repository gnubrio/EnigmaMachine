#pragma once
#include <array>
#include <string>

class Rotor {
public:
  Rotor(std::string modelName, std::string symbols);
private:
  std::string modelName_ = "";
  std::array<char, 26> symbols_ = {};
};