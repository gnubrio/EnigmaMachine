#pragma once
#include <array>
#include <cctype>
#include <memory>
#include <string>

class Rotor {
public:
  Rotor(std::string modelName, std::string symbols, char notch);
  virtual ~Rotor() = default;
  virtual void spin(int direction = -1);
  void transfer(char &key);
  template <typename T, size_t N>
  T &shiftIndex(std::array<T, N> &activeRotors, size_t index, size_t shift);
  virtual char getNotch() const;
  virtual char getActiveSymbol(int offset = 0) const;

  bool operator==(const Rotor &other) const {
    return modelName_ == other.modelName_;
  }

protected:
  static constexpr unsigned int MAX_SYMBOLS_ = 26;
  std::string modelName_ = "";
  std::array<char, MAX_SYMBOLS_> symbols_ = {};
  const std::array<char, MAX_SYMBOLS_> alaphbet_ = {};
  unsigned int position_ = 0;

private:
  char notch_ = '\0';
  char activeSymbol_ = '\0';
};

class Reflector : public Rotor {
public:
  Reflector(std::string modelName, std::string symbols);

private:
};

struct Cable {
  Cable(char input, char output)
      : input(toupper(input)), output(toupper(output)) {}
  char input = '\0';
  char output = '\0';

  void transfer(char &key);
};

class EnigmaMachine {
public:
  static constexpr unsigned int MAX_ROTORS_ = 3;
  static constexpr unsigned int MAX_CABLES_ = 10;

  void encrypt(char &key);
  void spinRotors();
  std::array<Rotor, MAX_ROTORS_> getActiveRotors() const;

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

  Reflector reflectorA_ =
      Reflector("Reflector A", "EJMZALYXVBWFCRQUONTSPIKHGD");
  std::unique_ptr<Reflector> currentReflector_ =
      std::make_unique<Reflector>(reflectorA_);

  Cable cable1_ = Cable('\0', '\0');
  Cable cable2_ = Cable('\0', '\0');
  Cable cable3_ = Cable('\0', '\0');
  Cable cable4_ = Cable('\0', '\0');
  Cable cable5_ = Cable('\0', '\0');
  Cable cable6_ = Cable('\0', '\0');
  Cable cable7_ = Cable('\0', '\0');
  Cable cable8_ = Cable('\0', '\0');
  Cable cable9_ = Cable('\0', '\0');
  Cable cable10_ = Cable('\0', '\0');
  std::array<Cable, MAX_CABLES_> activePlugs_ = {
      cable1_, cable2_, cable3_, cable4_, cable5_,
      cable6_, cable7_, cable8_, cable9_, cable10_};
};