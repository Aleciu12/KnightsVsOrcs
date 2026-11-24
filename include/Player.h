//
// Created by Alex on 24.11.2026.
//

#ifndef OOP_PLAYER_H
#define OOP_PLAYER_H
#include <string>
#include <vector>
#include <ostream>

#include "Unit.h"


class Player {
private:
    std::string name_;
    int gold_, base_hp_;
    std::vector<Unit> units_;
    bool canAfford(int cost) const;
public:
    Player(const std::string& name, int gold, int base_hp, const std::vector<Unit>& units);
    bool spendResources(int amount);
    void addResources(int amount);
    void loseHP(int dmg);
    bool loseCondition() const;
    friend std::ostream& operator<<(std::ostream& os, const Player& player);
};


#endif //OOP_PLAYER_H
