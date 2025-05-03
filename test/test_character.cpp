#include <gtest/gtest.h>
#include "../src/h/Character.h"

TEST(CharacterTest, ParameterizedConstructor) {
    Character c("TestChar", 100, 5, 20, true, 15);
    EXPECT_EQ(c.getName(), "TestChar");
    EXPECT_EQ(c.getHealth(), 100);
    EXPECT_EQ(c.getMaxHealth(), 100);
    EXPECT_EQ(c.getDefense(), 5);
    EXPECT_EQ(c.getAttack(), 15);
    EXPECT_EQ(c.getMana(), 20);
    EXPECT_EQ(c.getMaxMana(), 20);
    EXPECT_EQ(c.getGold(), 0);
    EXPECT_TRUE(c.doesHaveMana());
    EXPECT_TRUE(c.isAlive());
    EXPECT_EQ(c.getInventory().size(), 0u);
}

TEST(CharacterTest, TakeDamageAndIsAlive) {
    Character c("Hero", 50, 0, 0, false, 5);
    c.takeDamage(20);
    EXPECT_EQ(c.getHealth(), 30);
    EXPECT_TRUE(c.isAlive());
    c.takeDamage(30);
    EXPECT_EQ(c.getHealth(), 0);
    EXPECT_FALSE(c.isAlive());
}

TEST(CharacterTest, UseMana) {
    Character c("Mage", 20, 0, 10, true, 2);
    EXPECT_TRUE(c.useMana(5));
    EXPECT_EQ(c.getMana(), 5);
    EXPECT_FALSE(c.useMana(10));
    EXPECT_EQ(c.getMana(), 5);
}

TEST(CharacterTest, Heal) {
    Character c("Healable", 50, 0, 0, false, 0);
    c.takeDamage(30);
    EXPECT_EQ(c.getHealth(), 20);
    c.heal(15);
    EXPECT_EQ(c.getHealth(), 35);
    c.heal(100);
    EXPECT_EQ(c.getHealth(), c.getMaxHealth());
}

TEST(CharacterTest, BoostStats) {
    Character c("Booster", 10, 1, 5, true, 2);
    c.boostAttack(8);
    EXPECT_EQ(c.getAttack(), 10);
    c.boostDefense(4);
    EXPECT_EQ(c.getDefense(), 5);
    c.boostHealth(10);
    EXPECT_EQ(c.getMaxHealth(), 20);
    EXPECT_EQ(c.getHealth(), 20);
    c.boostMana(10);
    EXPECT_EQ(c.getMaxMana(), 15);
    EXPECT_EQ(c.getMana(), 15);
}

TEST(CharacterTest, InventoryManagement) {
    Character c("Collector", 10, 0, 0, false, 0);
    Item potion("Potion", "Heals 20 HP");
    Item sword("Sword", "A sharp blade");
    c.addToInventory(potion);
    c.addToInventory(sword);
    const auto& inv = c.getInventory();
    ASSERT_EQ(inv.size(), 2u);
    EXPECT_EQ(inv[0].name, "Potion");
    EXPECT_EQ(inv[1].name, "Sword");
    c.removeFromInventory(0);
    ASSERT_EQ(c.getInventory().size(), 1u);
    EXPECT_EQ(c.getInventory()[0].name, "Sword");
}

TEST(CharacterTest, GoldManagement) {
    Character c("Dealer", 10, 0, 0, false, 0);
    c.setGold(100);
    EXPECT_EQ(c.getGold(), 100);
    c.addGold(50);
    EXPECT_EQ(c.getGold(), 150);
    EXPECT_TRUE(c.spendGold(120));
    EXPECT_EQ(c.getGold(), 30);
    EXPECT_FALSE(c.spendGold(40));
    EXPECT_EQ(c.getGold(), 30);
}