#pragma once

#include <string>

class Key {
public:
    explicit Key(std::string);

    static Key generate();

    [[nodiscard]] std::string toString() const;

    std::string XORKey;
    int iter;
};
