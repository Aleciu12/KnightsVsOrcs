#ifndef OOP_DATALOADER_H
#define OOP_DATALOADER_H

#include "Enemy.h"
#include "Unit.h"
#include <string>
#include <vector>

std::vector<Unit> loadUnits(const std::string& filePath);
std::vector<Enemy> loadEnemies(const std::string& filePath);

#endif
