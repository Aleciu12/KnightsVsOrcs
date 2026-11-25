#include "DataLoader.h"
#include "Enemy.h"
#include "GameEngine.h"
#include "Player.h"
#include "Unit.h"

#include <cstddef>
#include <exception>
#include <iostream>
#include <string>
#include <vector>

namespace {

void printCatalogs(const std::vector<Unit>& unitCatalog, const std::vector<Enemy>& enemyCatalog) {
    std::cout << "\nUnitati disponibile";
    if (!unitCatalog.empty()) {
        std::cout << " (unitIndex: 0.." << (unitCatalog.size() - 1) << ")";
    }
    std::cout << ":\n";
    for (std::size_t i = 0; i < unitCatalog.size(); ++i) {
        std::cout << "  [" << i << "] " << unitCatalog[i] << "\n";
    }

    std::cout << "\nInamici disponibili";
    if (!enemyCatalog.empty()) {
        std::cout << " (enemyIndex: 0.." << (enemyCatalog.size() - 1) << ")";
    }
    std::cout << ":\n";
    for (std::size_t i = 0; i < enemyCatalog.size(); ++i) {
        std::cout << "  [" << i << "] " << enemyCatalog[i] << "\n";
    }
}

void printCommands(int boardRows, int boardCols, std::size_t unitCount, std::size_t enemyCount) {
    std::cout << "\nComenzi disponibile:\n";
    std::cout << "  place <row> <col> <unitIndex>  - plaseaza o unitate"
              << " (row: 0.." << (boardRows - 1)
              << ", col: 0.." << (boardCols - 1);
    if (unitCount > 0) {
        std::cout << ", unitIndex: 0.." << (unitCount - 1);
    }
    std::cout << ")\n";

    std::cout << "  spawn <enemyIndex> <row>       - spawneaza un inamic (";
    if (enemyCount > 0) {
        std::cout << "enemyIndex: 0.." << (enemyCount - 1) << ", ";
    }
    std::cout << "row: 0.." << (boardRows - 1) << ")\n";
    std::cout << "  next                           - avanseaza o tura\n";
    std::cout << "  save                           - salveaza checkpoint\n";
    std::cout << "  load                           - restaureaza checkpoint\n";
    std::cout << "  status                         - afiseaza starea curenta\n";
    std::cout << "  help                           - afiseaza catalogul de unitati si inamici\n";
    std::cout << "  quit                           - iesire din joc\n";
}

}

int main() {
    std::vector<Unit> unitCatalog;
    std::vector<Enemy> enemyCatalog;
    try {
        unitCatalog = loadUnits("assets/units.json");
        enemyCatalog = loadEnemies("assets/enemies.json");
    } catch (const std::exception& err) {
        std::cerr << "Failed to load game data: " << err.what() << "\n";
        return 1;
    }

    std::cout << "=== Knights vs. Orcs ===\n";
    std::cout << "Nume: " << std::flush;

    std::string playerName;
    if (!(std::cin >> playerName)) {
        playerName = "Commander";
    }

    constexpr int boardRows = 3;
    constexpr int boardCols = 5;
    constexpr int startingGold = 150;
    constexpr int startingBaseHp = 100;

    const Player player{playerName, startingGold, startingBaseHp, unitCatalog};
    GameEngine game{boardRows, boardCols, player};
    GameEngine checkpoint{game};

    printCatalogs(unitCatalog, enemyCatalog);
    std::cout << "\n" << game << "\n";

    std::string command;
    while (game.getStatus() == GameStatus::Running) {
        printCommands(boardRows, boardCols, unitCatalog.size(), enemyCatalog.size());
        std::cout << "Comanda: " << std::flush;
        if (!(std::cin >> command)) {
            break;
        }
        if (command == "place") {
            int row = 0;
            int col = 0;
            std::size_t unitIndex = 0;
            if (!(std::cin >> row >> col >> unitIndex)) {
                break;
            }
            if (game.placeUnit(row, col, unitIndex)) {
                std::cout << "Placed unit " << unitIndex << " at (" << row << ", " << col << ").\n";
            } else {
                std::cout << "Cannot place unit " << unitIndex << " at (" << row << ", " << col << ").\n";
            }
        } else if (command == "spawn") {
            std::size_t enemyIndex = 0;
            int row = 0;
            if (!(std::cin >> enemyIndex >> row)) {
                break;
            }
            if (enemyIndex < enemyCatalog.size() && game.spawnEnemy(enemyCatalog[enemyIndex], row)) {
                std::cout << "Spawned enemy " << enemyIndex << " on row " << row << ".\n";
            } else {
                std::cout << "Cannot spawn enemy " << enemyIndex << " on row " << row << ".\n";
            }
        } else if (command == "next") {
            game.nextTurn();
            std::cout << game << "\n";
        } else if (command == "save") {
            checkpoint = game;
            std::cout << "Checkpoint saved.\n";
        } else if (command == "load") {
            game = checkpoint;
            std::cout << "Checkpoint restored.\n" << game << "\n";
        } else if (command == "status") {
            std::cout << game << "\n";
        } else if (command == "help") {
            printCatalogs(unitCatalog, enemyCatalog);
        } else if (command == "quit") {
            break;
        } else {
            std::cout << "Unknown command: " << command << "\n";
        }
    }

    std::cout << "Final status: " << game.getStatus() << "\n";
    return 0;
}
