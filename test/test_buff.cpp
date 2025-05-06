#include <gtest/gtest.h>
#define private public
#include "../src/h/BuffSystem.h"
#undef private

class DummyUIManager : public UIManager {
public:
    void clearScreen() {}
    char getCharImmediate() { return 'q'; }
};

TEST(BuffSystem, AddBuff_ApplyBonuses) {
    DummyUIManager ui;
    BuffSystem system(ui);
    Character hero("Буффер", 100, 5, 10, true, 20);

    int old_attack = hero.getAttack();
    int old_defense = hero.getDefense();

    system.addBuff(hero, "Баф силы", 7, 3, 0, 0, 2); // apply +7 atk, +3 def

    EXPECT_TRUE(system.hasBuff("Баф силы"));
    EXPECT_EQ(hero.getAttack(), old_attack + 7);
    EXPECT_EQ(hero.getDefense(), old_defense + 3);
}

TEST(BuffSystem, UpdateBuffs_ExpireBuffAndRemoveBonuses) {
    DummyUIManager ui;
    BuffSystem system(ui);
    Character hero("Воин", 50, 2, 0, false, 10);

    int old_attack = hero.getAttack();
    system.addBuff(hero, "Мощь", 8, 0, 0, 0, 1);

    EXPECT_EQ(hero.getAttack(), old_attack + 8);

    system.updateBuffs(hero);

    EXPECT_EQ(hero.getAttack(), old_attack);
    EXPECT_FALSE(system.hasBuff("Мощь"));
}

TEST(BuffSystem, ClearBuffs_RemovesAllWithoutBonusRollback) {
    DummyUIManager ui;
    BuffSystem system(ui);
    Character hero("Танк", 80, 10, 0, false, 20);

    system.addBuff(hero, "Защита", 0, 10, 0, 0, 3);
    EXPECT_TRUE(system.hasBuff("Защита"));
    system.clearBuffs();
}