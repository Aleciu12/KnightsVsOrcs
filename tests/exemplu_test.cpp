// Main ships no test framework on purpose: plain assert / static_assert only.
// Full framework examples live on tests/gtest and tests/Boost-ext-ut branches.

// assert() disappears when NDEBUG is defined (Release, RelWithDebInfo): the
// binary would exit 0 having checked nothing, and CI would call that a pass.
// Keep the checks on in every build type.
#undef NDEBUG
#include <cassert>

#include "Unit.h"
#include "Enemy.h"
#include "Player.h"
#include <iostream>


// checked through the public interface only: the helper behind the invariant is
// file-local in src/Example.cpp and cannot even be named here
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
    return 0;
}
