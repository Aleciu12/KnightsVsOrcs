#undef NDEBUG
#include <cassert>

#include "Unit.h"
#include "Enemy.h"
#include "Player.h"
#include "GameEngine.h"
#include "DataLoader.h"
#include <iostream>
#include <sstream>
#include <string>

namespace {

void testGameEnginePlacementAndSpawn() {
    const Unit knight{"Knight", 100, 50, 50, UnitType::Offensive};
    const Unit farm{"Farm", 60, 0, 25, UnitType::Farm};
    const Player player{"Commander", 50, 100, {knight, farm}};
    GameEngine engine{2, 4, player};

    assert(engine.getStatus() == GameStatus::Running);
    bool placed = engine.placeUnit(-1, 0, 0);
    assert(placed == false);
    placed = engine.placeUnit(0, -1, 0);
    assert(placed == false);
    placed = engine.placeUnit(2, 0, 0);
    assert(placed == false);
    placed = engine.placeUnit(0, 4, 0);
    assert(placed == false);
    placed = engine.placeUnit(0, 0, 2);
    assert(placed == false);

    placed = engine.placeUnit(0, 0, 0);
    assert(placed == true);
    placed = engine.placeUnit(0, 0, 1);
    assert(placed == false);
    placed = engine.placeUnit(0, 1, 1);
    assert(placed == false);

    const Enemy orc{"Orc", 50, 10, 40, 1.0, EnemyType::Average};
    const Enemy deadOrc{"DeadOrc", 0, 10, 40, 1.0, EnemyType::Average};
    bool spawned = engine.spawnEnemy(orc, -1);
    assert(spawned == false);
    spawned = engine.spawnEnemy(orc, 2);
    assert(spawned == false);
    spawned = engine.spawnEnemy(deadOrc, 0);
    assert(spawned == false);
    spawned = engine.spawnEnemy(orc, 0);
    assert(spawned == true);

    GameEngine emptyBoard{0, 0, player};
    spawned = emptyBoard.spawnEnemy(orc, 0);
    assert(spawned == false);
}

void testGameEngineFarmAndCombatAndWin() {
    const Unit knight{"Knight", 100, 50, 50, UnitType::Offensive};
    const Unit farm{"Farm", 60, 0, 25, UnitType::Farm};
    const Unit extra{"CheapUnit", 20, 10, 65, UnitType::Offensive};
    const Player player{"Commander", 75, 100, {knight, farm, extra}};
    GameEngine engine{2, 4, player};

    bool placed = engine.placeUnit(0, 0, 0);
    assert(placed == true);
    placed = engine.placeUnit(1, 0, 1);
    assert(placed == true);
    placed = engine.placeUnit(0, 1, 2);
    assert(placed == false);

    const Enemy orc{"Orc", 50, 10, 40, 1.0, EnemyType::Average};
    const bool spawned = engine.spawnEnemy(orc, 0);
    assert(spawned == true);
    assert(engine.getStatus() == GameStatus::Running);

    engine.nextTurn();
    assert(engine.getStatus() == GameStatus::Won);
    placed = engine.placeUnit(0, 1, 2);
    assert(placed == true);
}

void testGameEngineBlockingAndUnitCleanup() {
    const Unit wall{"ShieldWall", 60, 0, 10, UnitType::Defensive};
    const Unit farm{"GoldFarm", 35, 0, 10, UnitType::Farm};
    const Player player{"Defender", 50, 100, {wall, farm}};
    GameEngine engine{1, 3, player};

    bool placed = engine.placeUnit(0, 1, 0);
    assert(placed == true);

    const Enemy agile{"ScoutOrc", 200, 30, 50, 1.2, EnemyType::Agile};
    bool spawned = engine.spawnEnemy(agile, 0);
    assert(spawned == true);

    engine.nextTurn();
    assert(engine.getStatus() == GameStatus::Running);
    placed = engine.placeUnit(0, 1, 0);
    assert(placed == false);

    engine.nextTurn();
    assert(engine.getStatus() == GameStatus::Running);
    placed = engine.placeUnit(0, 1, 0);
    assert(placed == true);

    GameEngine bruteEngine{1, 3, player};
    const Enemy brute{"OrcBrute", 180, 35, 60, 0.5, EnemyType::Tank};
    spawned = bruteEngine.spawnEnemy(brute, 0);
    assert(spawned == true);
    bruteEngine.nextTurn();
    placed = bruteEngine.placeUnit(0, 0, 1);
    assert(placed == true);
    bruteEngine.nextTurn();
    assert(bruteEngine.getStatus() == GameStatus::Running);
    placed = bruteEngine.placeUnit(0, 0, 1);
    assert(placed == true);
}

void testGameEngineLossAndCopyAndOutput() {
    const Unit knight{"Knight", 100, 10, 10, UnitType::Offensive};
    const Unit wall{"Wall", 100, 0, 10, UnitType::Defensive};
    const Unit farm{"Farm", 100, 0, 10, UnitType::Farm};
    const Player player{"Hero", 100, 50, {knight, wall, farm}};
    GameEngine engine{2, 2, player};

    bool placed = engine.placeUnit(0, 0, 0);
    assert(placed == true);
    placed = engine.placeUnit(0, 1, 1);
    assert(placed == true);
    placed = engine.placeUnit(1, 0, 2);
    assert(placed == true);

    const Enemy boss{"BossOrc", 300, 50, 100, 1.0, EnemyType::Tank};
    const bool spawned = engine.spawnEnemy(boss, 1);
    assert(spawned == true);

    std::ostringstream oss;
    oss << engine;
    const std::string rendered = oss.str();
    assert(rendered.find("Turn 1 (Running)") != std::string::npos);
    assert(rendered.find("[O][D]") != std::string::npos);
    assert(rendered.find("[F][E]") != std::string::npos);
    assert(rendered.find("Hero") != std::string::npos);
    assert(rendered.find("BossOrc") != std::string::npos);

    GameEngine copyEngine{engine};
    GameEngine assignedEngine{1, 1, player};
    assignedEngine = engine;

    copyEngine.nextTurn();
    copyEngine.nextTurn();
    copyEngine.nextTurn();
    assert(copyEngine.getStatus() == GameStatus::Running);
    copyEngine.nextTurn();
    assert(copyEngine.getStatus() == GameStatus::Lost);

    assert(engine.getStatus() == GameStatus::Running);
    assert(assignedEngine.getStatus() == GameStatus::Running);
}

void testDataLoader() {
    const auto units = loadUnits("assets/units.json");
    assert(units.size() == 3);
    assert(units[0].getType() == UnitType::Offensive);
    assert(units[0].getDmg() == 45);
    assert(units[0].getCost() == 50);
    assert(units[1].getType() == UnitType::Defensive);
    assert(units[2].getType() == UnitType::Farm);

    const auto enemies = loadEnemies("assets/enemies.json");
    assert(enemies.size() == 3);
    assert(enemies[0].getReward() == 35);
    assert(enemies[1].getSpeed() > 1.0);
    assert(enemies[2].getDmg() == 35);
}

}


int main() {
    Unit unit{"Hero", 250, 50, 10, UnitType::Offensive};
    Unit dead_as_hell{"Dead", 0, -5, 10, UnitType::Farm};
    assert(unit.isDead() == false);
    unit.loseHP(249);
    assert(unit.isDead() == false);
    unit.loseHP(1);
    assert(unit.isDead() == true);
    unit.loseHP(-50);
    assert(unit.isDead() == true);
    assert(dead_as_hell.isDead() == true);
    std::cout << unit << "\n";
    std::cout << dead_as_hell << "\n";

    Enemy en1{"Villain", 250, 3, 50, 1.2, EnemyType::Agile};
    Enemy en2{"VERY dead", 0, -5, -100, -525, EnemyType::Tank};

    assert(en1.isDead() == false);
    std::cout << en1 << "\n";
    assert(en2.isDead() == true);
    en1.loseHP(250);
    assert(en1.isDead() == true);
    std::cout << en2 << "\n";
    assert(en1.getReward() == 50);
    assert(en2.getReward() == 0);

    Player p{"Savior", 500, 300, {unit}};
    std::cout << p << "\n";
    bool spent = p.spendResources(-50);
    assert(spent == false);
    assert(p.loseCondition() == false);
    p.loseHP(250);
    assert(p.loseCondition() == false);
    p.loseHP(-50);
    assert(p.loseCondition() == false);
    p.loseHP(50);
    assert(p.loseCondition() == true);
    spent = p.spendResources(500);
    assert(spent == true);
    spent = p.spendResources(1);
    assert(spent == false);
    p.addResources(-50);
    spent = p.spendResources(50);
    assert(spent == false);
    p.addResources(500);
    spent = p.spendResources(350);
    assert(spent == true);

    std::cout << p << "\n";

    Player p1{"a", 1, 1, {}};
    std::cout << p1 << "\n";

    testGameEnginePlacementAndSpawn();
    testGameEngineFarmAndCombatAndWin();
    testGameEngineBlockingAndUnitCleanup();
    testGameEngineLossAndCopyAndOutput();
    testDataLoader();
    return 0;
}
