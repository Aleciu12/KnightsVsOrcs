//
// Created by Alex on 24.11.2026.
//

#include "Unit.h"

std::ostream& operator<<(std::ostream& os, UnitType type) {
    switch (type) {
        case UnitType::Offensive: return os << "Offensive";
        case UnitType::Defensive: return os << "Defensive";
        case UnitType::Farm: return os << "Farm";
    }
    return os;
}

Unit::Unit(const std::string& name, int hp, int dmg, int cost, UnitType type) : name_(name), hp_(hp), dmg_(dmg), cost_(cost), type_(type) {
    if (hp_ < 0) hp_ = 0;
    if (dmg_ < 0) dmg_ = 0;
    if (cost_ < 0) cost_ = 0;
}

std::ostream& operator<<(std::ostream& os, const Unit& unit) {
    os << "Name: " << unit.name_ << "\n" << "HP: " << unit.hp_ << "\n" << "Damage dealt: " <<  unit.dmg_ << "\n" << "Cost to place: " << unit.cost_ << "\n" << "Type: " << unit.type_ << "\n";

    return os;
}

void Unit::loseHP(int dmg) {
    if (dmg < 0) return;
    hp_ -= dmg;
    if (hp_ < 0) hp_ = 0;
}

bool Unit::isDead() const {
    return hp_ == 0;
}

