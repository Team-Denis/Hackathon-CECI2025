#include <Encryption/Key.h>
#include <iomanip>
#include <iostream>
#include <random>
#include <sstream>
#include <stdexcept>
#include <string>

Key::Key(std::string key_str) {
  if (key_str.size() != 32 + 1 + 4) {  // 32 hex chars + ':' + 6 hex chars
    throw std::invalid_argument("Invalid key format");
  }
  if (key_str[32] != ':') {
    throw std::invalid_argument("Invalid key format: missing separator");
  }

  // Store the hex string directly
  xor_key = key_str.substr(0, 32);

  iter = std::stoi(key_str.substr(33, 4), nullptr, 16);
}

Key Key::gen() {
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<int> dist(0, 15);  // For hex digits (0-F)
  std::stringstream ss;

  // Generate 32 hex characters
  for (int i = 0; i < 32; i++) {
    ss << std::hex << dist(gen);
  }

  ss << ":";

  // Generate 6 hex characters
  for (int i = 0; i < 4; i++) {
    ss << std::hex << dist(gen);
  }

  std::string key_str = ss.str();
  return Key(key_str);
}

std::string Key::to_string() const {
  std::stringstream ss;
  ss << xor_key << ':' << std::hex << std::setw(4) << std::setfill('0') << iter;
  return ss.str();
}
