#include "GameEngine.h"

#include <algorithm>
#include <cmath>

std::ostream& operator<<(std::ostream& os, GameStatus status) {
    switch (status) {
        case GameStatus::Running: return os << "Running";
        case GameStatus::Won: return os << "Won";
        case GameStatus::Lost: return os << "Lost";
    }
    return os;
}

GameEngine::GameEngine(int rows, int cols, const Player& player) : rows_(rows), cols_(cols), turn_(1), player_(player) {
    if (rows_ < 0) rows_ = 0;
    if (cols_ < 0) cols_ = 0;
    board_.assign(static_cast<std::size_t>(rows_), std::vector<std::optional<Unit>>(static_cast<std::size_t>(cols_), std::nullopt));
}
GameEngine::GameEngine(const GameEngine& game): rows_(game.rows_), cols_(game.cols_), turn_(game.turn_), player_(game.player_), board_(game.board_), enemies_(game.enemies_) {}

GameEngine& GameEngine::operator=(const GameEngine& game) {
    if (this == &game) {
        return *this;
    }
    rows_ = game.rows_;
    cols_ = game.cols_;
    turn_ = game.turn_;
    player_ = game.player_;
    board_ = game.board_;
    enemies_ = game.enemies_;
    return *this;
}

GameEngine::~GameEngine() = default;


bool GameEngine::placeUnit(int row, int col, std::size_t unitIndex) {
    if (row < 0 || row >= rows_ || col < 0 || col >= cols_) return false;
    const auto r = static_cast<std::size_t>(row);
    const auto c = static_cast<std::size_t>(col);
    if (board_[r][c].has_value()) return false;
    if (unitIndex >= player_.getUnitCount()) return false;
    const Unit& unit = player_.getUnit(unitIndex);
    if (player_.spendResources(unit.getCost())) {
        board_[r][c] = unit;
        return true;
    }
    return false;
}

bool GameEngine::spawnEnemy(const Enemy& enemy, int row) {
    if (row < 0 || row >= rows_ || cols_ <= 0 || enemy.isDead()) {
        return false;
    }
    enemies_.emplace_back(enemy, row, static_cast<double>(cols_ - 1));
    return true;
}

std::ostream& operator<<(std::ostream& os, const GameEngine& game) {
    os << "=== Turn " << game.turn_ << " (" << game.getStatus() << ") ===\n";
    os << "Board (" << game.rows_ << "x" << game.cols_ << "):\n";

    for (std::size_t r = 0; r < game.board_.size(); ++r) {
        for (std::size_t c = 0; c < game.board_[r].size(); ++c) {
            const bool hasEnemy = std::any_of(
                game.enemies_.begin(),
                game.enemies_.end(),
                [r, c](const GameEngine::ActiveEnemy& activeEnemy) {
                    return !activeEnemy.enemy.isDead() &&
                           activeEnemy.row == static_cast<int>(r) &&
                           static_cast<int>(std::floor(activeEnemy.col)) == static_cast<int>(c);
                }
            );

            if (hasEnemy && game.board_[r][c].has_value()) {
                os << "[!]";
            } else if (hasEnemy) {
                os << "[E]";
            } else if (game.board_[r][c].has_value()) {
                switch (game.board_[r][c]->getType()) {
                    case UnitType::Offensive: os << "[O]"; break;
                    case UnitType::Defensive: os << "[D]"; break;
                    case UnitType::Farm: os << "[F]"; break;
                }
            } else {
                os << "[.]";
            }
        }
        os << "\n";
    }

    os << "\nPlayer:\n" << game.player_ << "\n";

    bool hasPlacedUnits = false;
    for (std::size_t r = 0; r < game.board_.size(); ++r) {
        for (std::size_t c = 0; c < game.board_[r].size(); ++c) {
            if (game.board_[r][c].has_value()) {
                if (!hasPlacedUnits) {
                    os << "Placed Units:\n";
                    hasPlacedUnits = true;
                }
                os << "Unit at (row " << r << ", col " << c << "):\n"
                   << *game.board_[r][c] << "\n";
            }
        }
    }
    if (!hasPlacedUnits) {
        os << "No Placed Units!\n\n";
    }

    if (game.enemies_.empty()) {
        os << "No Active Enemies!\n";
    } else {
        os << "Active Enemies:\n";
        int i = 1;
        for (const auto& activeEnemy : game.enemies_) {
            os << "Enemy " << i++ << " (row " << activeEnemy.row << ", col " << activeEnemy.col << "):\n"
               << activeEnemy.enemy << "\n";
        }
    }

    return os;
}

void GameEngine::collectFarmIncome() {
    for (const auto& row : board_) {
        for (const auto& cell : row) {
            if (cell.has_value() && !cell->isDead() && cell->getType() == UnitType::Farm) {
                player_.addResources(FARM_INCOME);
            }
        }
    }
}

void GameEngine::unitsAttack() {
    for (std::size_t r = 0; r < board_.size(); ++r) {
        for (std::size_t c = 0; c < board_[r].size(); ++c) {
            const auto& cell = board_[r][c];
            if (!cell.has_value() || cell->isDead() || cell->getType() != UnitType::Offensive) {
                continue;
            }

            ActiveEnemy* closest = nullptr;
            const auto unitRow = static_cast<int>(r);
            const auto unitCol = static_cast<double>(c);

            for (auto& activeEnemy : enemies_) {
                if (activeEnemy.enemy.isDead() || activeEnemy.row != unitRow || activeEnemy.col < unitCol) {
                    continue;
                }
                if (closest == nullptr || activeEnemy.col < closest->col) {
                    closest = &activeEnemy;
                }
            }

            if (closest != nullptr) {
                closest->enemy.loseHP(cell->getDmg());
            }
        }
    }
}

void GameEngine::cleanupDeadEnemies() {
    for (const auto& activeEnemy : enemies_) {
        if (activeEnemy.enemy.isDead()) {
            player_.addResources(activeEnemy.enemy.getReward());
        }
    }
    std::erase_if(enemies_, [](const ActiveEnemy& activeEnemy) {
        return activeEnemy.enemy.isDead();
    });
}

void GameEngine::enemiesAct() {
    const auto rBound = static_cast<std::size_t>(rows_);
    const auto cBound = static_cast<std::size_t>(cols_);

    for (auto& activeEnemy : enemies_) {
        if (activeEnemy.enemy.isDead()) {
            continue;
        }
        if (activeEnemy.row < 0 || static_cast<std::size_t>(activeEnemy.row) >= rBound) {
            continue;
        }

        const auto r = static_cast<std::size_t>(activeEnemy.row);
        const int frontCol = static_cast<int>(std::floor(activeEnemy.col));

        if (frontCol >= 0 && static_cast<std::size_t>(frontCol) < cBound) {
            auto& cell = board_[r][static_cast<std::size_t>(frontCol)];
            if (cell.has_value() && !cell->isDead()) {
                cell->loseHP(activeEnemy.enemy.getDmg());
                continue;
            }
        }

        if (frontCol - 1 >= 0 && static_cast<std::size_t>(frontCol - 1) < cBound) {
            auto& aheadCell = board_[r][static_cast<std::size_t>(frontCol - 1)];
            if (aheadCell.has_value() && !aheadCell->isDead()) {
                activeEnemy.col = std::max(activeEnemy.col - activeEnemy.enemy.getSpeed(), static_cast<double>(frontCol));
                aheadCell->loseHP(activeEnemy.enemy.getDmg());
                continue;
            }
        }

        const double nextCol = activeEnemy.col - activeEnemy.enemy.getSpeed();
        const int startCheck = std::min(frontCol - 1, cols_ - 1);
        const int endCheck = std::max(static_cast<int>(std::floor(nextCol)), 0);
        bool blocked = false;

        for (int c = startCheck; c >= endCheck; --c) {
            auto& cell = board_[r][static_cast<std::size_t>(c)];
            if (cell.has_value() && !cell->isDead()) {
                activeEnemy.col = static_cast<double>(c + 1);
                cell->loseHP(activeEnemy.enemy.getDmg());
                blocked = true;
                break;
            }
        }

        if (!blocked) {
            activeEnemy.col = nextCol;
            if (activeEnemy.col < 0.0) {
                player_.loseHP(activeEnemy.enemy.getDmg());
            }
        }
    }

    std::erase_if(enemies_, [](const ActiveEnemy& activeEnemy) {
        return activeEnemy.col < 0.0;
    });
}

void GameEngine::cleanupDeadUnits() {
    for (auto& row : board_) {
        for (auto& cell : row) {
            if (cell.has_value() && cell->isDead()) {
                cell = std::nullopt;
            }
        }
    }
}

void GameEngine::nextTurn() {
    collectFarmIncome();
    unitsAttack();
    cleanupDeadEnemies();
    enemiesAct();
    cleanupDeadUnits();
    ++turn_;
}

GameStatus GameEngine::getStatus() const {
    if (player_.loseCondition()) {
        return GameStatus::Lost;
    }
    if (turn_ > 1 && enemies_.empty()) {
        return GameStatus::Won;
    }
    return GameStatus::Running;
}
