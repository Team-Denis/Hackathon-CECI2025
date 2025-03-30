#pragma once

#include <string>

class Key {
public:
  explicit Key(std::string);
  static Key gen();
  [[nodiscard]] std::string to_string() const;

  std::string xor_key;
  int iter;
};