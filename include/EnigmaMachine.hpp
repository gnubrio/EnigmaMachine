#pragma once
#include <array>
#include <string>
#include <unordered_map>
#include <vector>

class EnigmaMachine;
EnigmaMachine setupEnigmaMachine();

class Rotor {
public:
  Rotor(const std::string &modelName, const std::string &symbols, char notch);
  virtual ~Rotor() = default;
  virtual void spin(int direction = -1);
  void transfer(char &key);
  template <typename T, size_t N>
  T &shiftIndex(std::array<T, N> &activeRotors, size_t index, size_t shift);
  const std::string &getModelName() const;
  virtual char getNotch(const int offset = 0) const;
  virtual char getActiveSymbol(const int offset = 0) const;

  bool operator==(const Rotor &other) const {
    return modelName_ == other.modelName_;
  }
  bool operator!=(const Rotor &other) const {
    return modelName_ != other.modelName_;
  }

protected:
  static constexpr unsigned int MAX_SYMBOLS_ = 26;
  std::string modelName_ = "";
  std::array<char, MAX_SYMBOLS_> symbols_ = {};
  std::unordered_map<char, int> alphabet_;
  unsigned int position_ = 0;

private:
  char notch_ = '\0';
  char activeSymbol_ = '\0';
};

class Reflector : public Rotor {
public:
  Reflector(const std::string &modelName, const std::string &symbols);

private:
};

struct Cable {
  Cable(char input, char output)
      : input(toupper(input)), output(toupper(output)) {}

  static constexpr unsigned int MAX_PLUGS = 2;
  char input = '\0';
  char output = '\0';

  void transfer(char &key);
};

class EnigmaMachine {
public:
  EnigmaMachine(std::vector<Rotor> &rotors, std::vector<Reflector> &reflectors,
                std::vector<Cable> &cables);

  static constexpr unsigned int MAX_ROTORS_ = 3;
  static constexpr unsigned int MAX_CABLES_ = 10;

  void encrypt(char &key);
  void spinRotors(int direction = -1);
  void setRotor(const Rotor &inputRotor, const Rotor &originalRotor,
                unsigned int index);
  void setSymbol(const Rotor &rotor, int direction);
  void setPlug(const int index, const bool input, const int direction);

  const std::vector<Rotor> &getAvaliableRotors() const;
  const std::vector<Rotor> &getActiveRotors() const;
  const std::vector<Cable> &getActivePlugs() const;

private:
  static constexpr unsigned int MAX_REFLECTORS_ = 1;

  std::vector<Rotor> avaliableRotors_ = {};
  std::vector<Reflector> avaliableReflectors_ = {};
  std::vector<Rotor> activeRotors_;

  Reflector currentReflector_ = avaliableReflectors_[0];

  std::vector<Cable> activePlugs_ = {};
};
