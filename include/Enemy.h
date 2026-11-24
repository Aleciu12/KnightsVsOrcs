//
// Created by Alex on 24.11.2026.
//

#ifndef OOP_ENEMY_H
#define OOP_ENEMY_H
#include <string>
#include <ostream>


enum class EnemyType {
    Average,
    Agile,
    Tank
};
std::ostream& operator<<(std::ostream& os, EnemyType type);

class Enemy {
private:
    std::string name_;
    int hp_, dmg_, reward_;
    float speed_;
    EnemyType type_;

public:
    Enemy(const std::string& name, int hp, int dmg, int reward, double speed, EnemyType type);
    friend std::ostream& operator<<(std::ostream& os, const Enemy& enemy);
    void loseHP(int dmg);
    bool isDead() const;
    int getReward(int reward);
};


#endif //OOP_ENEMY_H
