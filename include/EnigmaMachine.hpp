#pragma once
#include <array>
#include <memory>
#include <string>

class Rotor {
public:
  Rotor(std::string modelName, std::string symbols, char notch);
  void spin(int direction = -1);
  char getNotch();
  char getActiveSymbol(int offset = 0);

  bool operator==(const Rotor &other) const {
    return modelName_ == other.modelName_;
  }

private:
  std::string modelName_ = "";
  std::array<char, 26> symbols_ = {};
  char notch_ = '\0';
  unsigned int position_ = 0;
  char activeSymbol_ = '\0';
};

class EnigmaMachine {
public:
  static constexpr unsigned int MAX_ROTORS_ = 3;

  void encrypt(char key);
  void spinRotors();
  std::array<Rotor, MAX_ROTORS_> getActiveRotors();

private:
  Rotor rotorI_ =
      Rotor("Enigma I | Rotor I", "EKMFLGDQVZNTOWYHXUSPAIBRCJ", 'Q');
  Rotor rotorII_ =
      Rotor("Enigma I | Rotor II", "AJDKSIRUXBLHWTMCQGZNPYFVOE", 'E');
  Rotor rotorIII_ =
      Rotor("Enigma I | Rotor III", "BDFHJLCPRTXVZNYEIWGAKMUSQO", 'V');
  Rotor rotorIV_ =
      Rotor("M3 Army | Rotor I", "ESOVPZJAYQUIRHXLNFTGKDCMWB", 'J');
  Rotor rotorV_ =
      Rotor("M3 Army | Rotor II", "VZBRGITYUPSDNHLXAWMJQOFECK", 'Z');
  std::array<Rotor, MAX_ROTORS_> activeRotors_ = {rotorI_, rotorII_, rotorIII_};
};