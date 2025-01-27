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

  char letter = 65;
  for (size_t i = 0; i < alaphbet_.size(); ++i) {
    const_cast<char &>(alaphbet_[i]) = letter;
    letter++;
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

void Rotor::transfer(char &key) {
  for (unsigned int i = 0; i < MAX_SYMBOLS_; ++i) {
    if (key == alaphbet_[i]) {
      key = shiftIndex(symbols_, position_, i);
    }
  }
}

template <typename T, size_t N>
T &Rotor::shiftIndex(std::array<T, N> &activeRotors, size_t index,
                     size_t shift) {
  size_t start = (index + shift) % N;
  return activeRotors[start];
}

char Rotor::getNotch() const { return notch_; }

char Rotor::getActiveSymbol(int offset) const {
  if (offset < 0 && symbols_[position_] == symbols_.front()) {
    return symbols_.back() + (offset + 1);
  } else if (offset > 0 && symbols_[position_] == symbols_.back()) {
    return symbols_[0 + (offset - 1)];
  }
  return symbols_[position_ + offset];
}

Reflector::Reflector(std::string modelName, std::string symbols)
    : Rotor(modelName, symbols, '\0') {}

void Cable::transfer(char &key) {
  if (this->input == '\0' || this->output == '\0') {
    return;
  }

  if (this->input == key) {
    key = this->output;
  } else if (this->output == key) {
    key = this->input;
  }
}

void EnigmaMachine::encrypt(char &key) {
  for (auto cable : activePlugs_) {
    cable.transfer(key);
  }

  for (auto rotor : activeRotors_) {
    rotor.transfer(key);
  }

  currentReflector_->transfer(key);

  for (int i = MAX_ROTORS_; i > 0; --i) {
    activeRotors_[i].transfer(key);
  }

  for (int i = MAX_CABLES_; i > 0; --i) {
    activePlugs_[i].transfer(key);
  }
}

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

std::array<Rotor, EnigmaMachine::MAX_ROTORS_>
EnigmaMachine::getActiveRotors() const {
  return activeRotors_;
}