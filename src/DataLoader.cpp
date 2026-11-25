#include "DataLoader.h"

#include <fstream>
#include <nlohmann/json.hpp>
#include <stdexcept>

namespace {

UnitType parseUnitType(const std::string& value) {
    if (value == "Offensive") return UnitType::Offensive;
    if (value == "Defensive") return UnitType::Defensive;
    if (value == "Farm") return UnitType::Farm;
    throw std::invalid_argument("Unknown UnitType: " + value);
}

EnemyType parseEnemyType(const std::string& value) {
    if (value == "Average") return EnemyType::Average;
    if (value == "Agile") return EnemyType::Agile;
    if (value == "Tank") return EnemyType::Tank;
    throw std::invalid_argument("Unknown EnemyType: " + value);
}

nlohmann::json readJsonFile(const std::string& filePath) {
    std::ifstream input(filePath);
    if (!input.is_open()) {
        throw std::runtime_error("Could not open data file: " + filePath);
    }
    nlohmann::json data;
    input >> data;
    return data;
}

}

std::vector<Unit> loadUnits(const std::string& filePath) {
    const nlohmann::json data = readJsonFile(filePath);
    std::vector<Unit> units;
    units.reserve(data.size());
    for (const auto& entry : data) {
        units.emplace_back(
            entry.at("name").get<std::string>(),
            entry.at("hp").get<int>(),
            entry.at("dmg").get<int>(),
            entry.at("cost").get<int>(),
            parseUnitType(entry.at("type").get<std::string>())
        );
    }
    return units;
}

std::vector<Enemy> loadEnemies(const std::string& filePath) {
    const nlohmann::json data = readJsonFile(filePath);
    std::vector<Enemy> enemies;
    enemies.reserve(data.size());
    for (const auto& entry : data) {
        enemies.emplace_back(
            entry.at("name").get<std::string>(),
            entry.at("hp").get<int>(),
            entry.at("dmg").get<int>(),
            entry.at("reward").get<int>(),
            entry.at("speed").get<double>(),
            parseEnemyType(entry.at("type").get<std::string>())
        );
    }
    return enemies;
}
