#include "Enemy.h"

std::ostream& operator<<(std::ostream& os, EnemyType type) {
    switch (type) {
        case EnemyType::Average: return os << "Average";
        case EnemyType::Agile: return os << "Agile";
        case EnemyType::Tank: return os << "Tank";
    }
    return os;
}

Enemy::Enemy(const std::string &name, int hp, int dmg, int reward, double speed, EnemyType type) : name_(name), hp_(hp), dmg_(dmg), reward_(reward), speed_(speed), type_(type) {
    if (hp < 0) hp_ = 0;
    if (dmg < 0) dmg_ = 0;
    if (speed < 0) speed_ = 0;
    if (reward < 0) reward_ = 0;
}

std::ostream& operator<<(std::ostream& os, const Enemy& enemy) {
    os << "Name: " << enemy.name_ << "\n" << "HP: " << enemy.hp_ << "\n" << "Damage dealt: " <<  enemy.dmg_ << "\n" << "Speed (tiles walked per turn): " << enemy.speed_ << "\n" << "Reward: " << enemy.reward_ << "\n" << "Type: " << enemy.type_ << "\n";
    return os;
}

void Enemy::loseHP(int dmg) {
    if (dmg < 0) return;
    hp_ -= dmg;
    if (hp_ < 0) hp_ = 0;
}

bool Enemy::isDead() const {
    return hp_ == 0;
}

int Enemy::getReward() const {
    return reward_;
}

int Enemy::getDmg() const {
    return dmg_;
}

double Enemy::getSpeed() const {
    return speed_;
}

