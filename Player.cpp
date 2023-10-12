#pragma once
#include <string>

class Player {
public:
    std:: string name;
    std::string time_str;
    int time;
    Player(std::string name, std::string time_str) {
        this->name = name;
        this->time_str = time_str;
    }
};