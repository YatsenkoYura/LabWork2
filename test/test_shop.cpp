#include <gtest/gtest.h>
#define private public
#include "../src/h/ShopSystem.h"
#undef private
#include "../src/h/Character.h"

class DummyUIManager : public UIManager {
public:
    char getCharImmediate() { return 'q'; }
    void clearScreen() {}
};

TEST(ShopSystem, GenerateShopItems_AddsFourItemsWithModifiedStats) {
    DummyUIManager ui;
    ShopSystem shop(ui);
    shop.generateShopItems(3); // Например, раунд 3

    const auto& items = shop.availableItems; // Теперь доступно!
    EXPECT_EQ(items.size(), 4);
    for (const auto& item : items) {
        EXPECT_GE(item.price, 0);
        EXPECT_GE(item.value, 0);
    }
}

TEST(ShopSystem, CalculateUpgradeCost_CorrectResult) {
    DummyUIManager ui;
    ShopSystem shop(ui);

    int defense = 12;
    int round = 2;
    int cost = shop.calculateUpgradeCost(defense, round);
    // 100 + 12*10 + 2*50 = 320
    EXPECT_EQ(cost, 320);
}

TEST(ShopSystem, UpgradeArmor_NotEnoughGold_NoChange) {
    DummyUIManager ui;
    ShopSystem shop(ui);

    Character player("test", 20, 10, 0, false, 2);
    player.setGold(0); // недостаточно золота
    int defense_before = player.getDefense();

    shop.upgradeArmor(player, 1);

    EXPECT_EQ(player.getDefense(), defense_before);
    EXPECT_EQ(player.getGold(), 0);
}

TEST(ShopSystem, UpgradeArmor_EnoughGold_Success) {
    DummyUIManager ui;
    ShopSystem shop(ui);

    Character player("test", 20, 10, 0, false, 2);
    int cost = shop.calculateUpgradeCost(player.getDefense(), 1);
    player.setGold(cost + 5);

    shop.upgradeArmor(player, 1);

    EXPECT_EQ(player.getDefense(), 15); // +5 от boostDefense
    EXPECT_EQ(player.getGold(), 5);
}

TEST(ShopSystem, UpgradeWeapon_NotEnoughGold_NoChange) {
    DummyUIManager ui;
    ShopSystem shop(ui);

    Character player("test", 20, 5, 0, false, 10);
    player.setGold(5); // мало
    int atk_before = player.getAttack();

    shop.upgradeWeapon(player, 1);

    EXPECT_EQ(player.getAttack(), atk_before);
    EXPECT_EQ(player.getGold(), 5);
}

TEST(ShopSystem, UpgradeWeapon_EnoughGold_Success) {
    DummyUIManager ui;
    ShopSystem shop(ui);

    Character player("test", 20, 5, 0, false, 10);
    int cost = shop.calculateUpgradeCost(player.getAttack(), 1);
    player.setGold(cost);

    shop.upgradeWeapon(player, 1);

    EXPECT_EQ(player.getAttack(), 13); // +3
    EXPECT_EQ(player.getGold(), 0);
}