#include "EnigmaMachine.hpp"
#include <string>

Rotor::Rotor(std::string modelName, std::string symbols)
    : modelName_(modelName) {
  if (symbols.length() >= symbols_.size()) {
    for (size_t i = 0; i < symbols_.size(); ++i) {
      symbols_[i] = symbols[i];
    }
  }
}