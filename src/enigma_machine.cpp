#include "EnigmaMachine.hpp"
#include <array>
#include <string>

Rotor::Rotor(std::string modelName, std::string symbols, char notch)
    : modelName_(modelName), notch_(notch) {
  if (symbols.length() >= symbols_.size()) {
    for (size_t i = 0; i < symbols_.size(); ++i) {
      symbols_[i] = symbols[i];
    }
    activeSymbol_ = symbols[0];
  }
}

void Rotor::spin(int direction) {
  if (direction == 1) {
    if (activeSymbol_ == symbols_.back()) {
      position_ = 0;
      activeSymbol_ = symbols_[0];
    } else {
      position_++;
      activeSymbol_ = symbols_[position_];
    }
  } else if (direction == -1) {
    if (activeSymbol_ == symbols_.front()) {
      position_ = symbols_.size() - 1;
      activeSymbol_ = symbols_[position_];
    } else {
      position_--;
      activeSymbol_ = symbols_[position_];
    }
  }
}

char Rotor::getNotch() { return notch_; }

char Rotor::getActiveSymbol(int offset) {
  if (offset < 0 && symbols_[position_] == symbols_.front()) {
    return symbols_.back() + (offset + 1);
  } else if (offset > 0 && symbols_[position_] == symbols_.back()) {
    return symbols_[0 + (offset - 1)];
  }
  return symbols_[position_ + offset];
}

void EnigmaMachine::encrypt(char key) {}

void EnigmaMachine::spinRotors() {
  activeRotors_.back().spin();

  for (int i = activeRotors_.size() - 1; i > 0; --i) {
    if (activeRotors_[i] != activeRotors_.front() &&
        activeRotors_[i].getActiveSymbol() == activeRotors_[i].getNotch()) {
      activeRotors_[i - 1].spin();
    } else {
      return;
    }
  }
}

std::array<Rotor, EnigmaMachine::MAX_ROTORS_> EnigmaMachine::getActiveRotors() {
  return activeRotors_;
}