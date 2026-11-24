//
// Created by Alex on 24.11.2026.
//

#ifndef OOP_UNIT_H
#define OOP_UNIT_H
#include <string>
#include <ostream>

enum class UnitType {
    Offensive,
    Defensive,
    Farm
};
std::ostream& operator<<(std::ostream& os, UnitType type);

class Unit {
private:
    std::string name_;
    int hp_, dmg_, cost_;
    UnitType type_;

public:
    Unit(const std::string & name, int hp, int dmg, int cost, UnitType type);
    friend std::ostream& operator<<(std::ostream& os, const Unit& unit);
    void loseHP(int dmg);
    bool isDead() const;
};


#endif //OOP_UNIT_H
