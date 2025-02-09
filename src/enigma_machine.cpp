#include "../include/EnigmaMachine.hpp"

EnigmaMachine setupEnigmaMachine() {
  Rotor rotorI = Rotor("Enigma I | Rotor I", "EKMFLGDQVZNTOWYHXUSPAIBRCJ", 'Q');
  Rotor rotorII =
      Rotor("Enigma I | Rotor II", "AJDKSIRUXBLHWTMCQGZNPYFVOE", 'E');
  Rotor rotorIII =
      Rotor("Enigma I | Rotor III", "BDFHJLCPRTXVZNYEIWGAKMUSQO", 'V');
  Rotor rotorIV = Rotor("M3 Army | Rotor I", "ESOVPZJAYQUIRHXLNFTGKDCMWB", 'J');
  Rotor rotorV = Rotor("M3 Army | Rotor II", "VZBRGITYUPSDNHLXAWMJQOFECK", 'Z');
  std::vector<Rotor> rotors = {rotorI, rotorII, rotorIII, rotorIV, rotorV};

  Reflector reflectorA = Reflector("Reflector A", "EJMZALYXVBWFCRQUONTSPIKHGD");
  std::vector<Reflector> reflectors = {reflectorA};

  Cable cable1 = Cable('\0', '\0');
  Cable cable2 = Cable('\0', '\0');
  Cable cable3 = Cable('\0', '\0');
  Cable cable4 = Cable('\0', '\0');
  Cable cable5 = Cable('\0', '\0');
  Cable cable6 = Cable('\0', '\0');
  Cable cable7 = Cable('\0', '\0');
  Cable cable8 = Cable('\0', '\0');
  Cable cable9 = Cable('\0', '\0');
  Cable cable10 = Cable('\0', '\0');
  std::vector<Cable> cables = {cable1, cable2, cable3, cable4, cable5,
                               cable6, cable7, cable8, cable9, cable10};

  return EnigmaMachine(rotors, reflectors, cables);
}

Rotor::Rotor(const std::string &modelName, const std::string &symbols,
             char notch)
    : modelName_(modelName), notch_(notch) {
  if (symbols.length() >= symbols_.size()) {
    for (size_t i = 0; i < symbols_.size(); ++i) {
      symbols_[i] = symbols[i];
    }
    activeSymbol_ = symbols[0];
  }

  for (size_t i = 0; i < MAX_SYMBOLS_; ++i) {
    alphabet_['A' + i] = i;
  }
}

void Rotor::spin(int direction) {
  if (direction == 1) {
    if (activeSymbol_ == symbols_.back()) {
      position_ = 0;
      activeSymbol_ = symbols_[position_];
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
  if (alphabet_.find(key) != alphabet_.end()) {
    key = shiftIndex(symbols_, position_, alphabet_.at(key));
  }
}

template <typename T, size_t N>
T &Rotor::shiftIndex(std::array<T, N> &symbols, size_t index,
                     size_t shift) {
  size_t start = (index + shift) % N;
  return symbols[start];
}

const std::string &Rotor::getModelName() const { return modelName_; }

char Rotor::getNotch(const int offset) const {
  if (offset == 0) {
    return notch_;
  }

  for (unsigned long int i = 0; i < symbols_.size(); ++i) {
    if (notch_ == symbols_[i]) {
      if (symbols_[i] == symbols_.back()) {
        return symbols_[offset - 1];
      } else {
        return symbols_[i + offset];
      }
    }
  }

  return '\0';
}

char Rotor::getActiveSymbol(const int offset) const {
  if (offset < 0 && symbols_[position_] == symbols_.front()) {
    return symbols_.back() + (offset + 1);
  } else if (offset > 0 && symbols_[position_] == symbols_.back()) {
    return symbols_[0 + (offset - 1)];
  }
  return symbols_[position_ + offset];
}

Reflector::Reflector(const std::string &modelName, const std::string &symbols)
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

EnigmaMachine::EnigmaMachine(std::vector<Rotor> &rotors,
                             std::vector<Reflector> &reflectors,
                             std::vector<Cable> &cables)
    : avaliableRotors_(rotors), avaliableReflectors_(reflectors),
      activePlugs_(cables) {
  for (unsigned int i = 0; i < MAX_ROTORS_; ++i) {
    activeRotors_.push_back(avaliableRotors_[i]);
  }
}

void EnigmaMachine::setPlug(const int index, const bool input,
                            const int direction) {
  char *plug;
  if (input) {
    plug = &activePlugs_[index].input;
  } else {
    plug = &activePlugs_[index].output;
  }

  if (direction == 1) {
    if (*plug == '\0') {
      *plug = 'A';
    } else if (*plug < 'Z') {
      (*plug)++;
    }
  } else if (direction == -1) {
    if (*plug == 'A') {
      *plug = '\0';
    } else if (*plug > 'A') {
      (*plug)--;
    }
  }
}

void EnigmaMachine::encrypt(char &key) {
  for (auto cable : activePlugs_) {
    cable.transfer(key);
  }

  for (auto rotor : activeRotors_) {
    rotor.transfer(key);
  }

  currentReflector_.transfer(key);

  for (int i = MAX_ROTORS_; i > 0; --i) {
    activeRotors_[i - 1].transfer(key);
  }

  for (int i = MAX_CABLES_; i > 0; --i) {
    activePlugs_[i - 1].transfer(key);
  }
}

void EnigmaMachine::spinRotors(int direction) {
  activeRotors_.back().spin(direction);

  if (direction == -1) {
    for (int i = MAX_ROTORS_ - 1; i > 0; --i) {
      if (activeRotors_[i].getActiveSymbol() == activeRotors_[i].getNotch()) {
        activeRotors_[i - 1].spin(direction);
      } else {
        return;
      }
    }
  } else if (direction == 1) {
    for (int i = MAX_ROTORS_ - 1; i > 0; --i) {
      if (activeRotors_[i].getActiveSymbol() == activeRotors_[i].getNotch(1)) {
        activeRotors_[i - 1].spin(direction);
      } else {
        return;
      }
    }
  }
}

void EnigmaMachine::setRotor(const Rotor &inputRotor,
                             const Rotor &originalRotor, unsigned int index) {
  bool swap = false;
  unsigned int swapIndex = 0;

  for (size_t i = 0; i < activeRotors_.size(); ++i) {
    if (activeRotors_[i] == inputRotor) {
      if (i == index) {
        return;
      } else {
        swap = true;
        swapIndex = i;
      }
    }
  }

  if (swap) {
    activeRotors_[swapIndex] = originalRotor;
  }
  activeRotors_[index] = inputRotor;
}

const std::vector<Rotor> &EnigmaMachine::getAvaliableRotors() const {
  return avaliableRotors_;
}

const std::vector<Rotor> &EnigmaMachine::getActiveRotors() const {
  return activeRotors_;
}

const std::vector<Cable> &EnigmaMachine::getActivePlugs() const {
  return activePlugs_;
}
