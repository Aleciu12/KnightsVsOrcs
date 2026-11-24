//
// Created by Alex on 24.11.2026.
//

#include "Player.h"

Player::Player(const std::string& name, int gold, int base_hp, const std::vector<Unit>& units) : name_(name), gold_(gold), base_hp_(base_hp), units_(units) {
    if (gold_ < 0) gold_ = 0;
    if (base_hp_ < 0) base_hp_ = 0;
}

bool Player::canAfford(int cost) const {
    if (cost < 0) return false;
    return (gold_ - cost >= 0);
}

bool Player::spendResources(int amount) {
    if (!canAfford(amount)) return false;
    gold_ -= amount;
    return true;
}

void Player::addResources(int amount) {
    if (amount <= 0) return;
    gold_ += amount;
}

void Player::loseHP(int dmg) {
    if (dmg < 0) return;
    base_hp_ -= dmg;
    if (base_hp_ < 0) base_hp_ = 0;
}

bool Player::loseCondition() const {
    return (base_hp_ <= 0);
}

std::ostream& operator<<(std::ostream& os, const Player& player) {
    os << "Name: " << player.name_ << "\n" << "Gold: " << player.gold_ << "\n" << "HP of Base left: " << player.base_hp_ << "\n\n";

    if (player.units_.empty()) os << "No Units!\n";
    else {
        os << "Units: " << "\n";
        int i = 1;
        for (const auto& unit: player.units_) {
            os << "Unit " << i++ << ": " << "\n" << unit << "\n";
        }
    }
    return os;
}

