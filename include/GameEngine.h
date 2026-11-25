#ifndef OOP_GAMEENGINE_H
#define OOP_GAMEENGINE_H
#include "Player.h"
#include "Unit.h"
#include "Enemy.h"
#include <ostream>
#include <optional>
#include <vector>
#include <cstddef>

enum class GameStatus {
    Running,
    Won,
    Lost
};
std::ostream& operator<<(std::ostream& os, GameStatus status);


class GameEngine {
private:
    static constexpr int FARM_INCOME = 25;
    int rows_, cols_, turn_;
    Player player_;
    std::vector<std::vector<std::optional<Unit>>> board_;
    struct ActiveEnemy {
        Enemy enemy;
        int row = 0;
        double col = 0.0;
    };
    std::vector<ActiveEnemy> enemies_;

    void collectFarmIncome();
    void unitsAttack();
    void cleanupDeadEnemies();
    void enemiesAct();
    void cleanupDeadUnits();
public:
    GameEngine(int rows, int cols, const Player& player);
    GameEngine(const GameEngine& game);
    GameEngine& operator=(const GameEngine& game);
    ~GameEngine();
    bool placeUnit(int row, int col, std::size_t unitIndex);
    bool spawnEnemy(const Enemy& enemy, int row);
    friend std::ostream& operator<<(std::ostream& os, const GameEngine& game);
    void nextTurn();
    GameStatus getStatus() const;
};


#endif
