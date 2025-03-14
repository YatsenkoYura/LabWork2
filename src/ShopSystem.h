#ifndef SHOP_SYSTEM_H
#define SHOP_SYSTEM_H

#include "Character.h"
#include "UIManager.h"

class ShopSystem {
public:
    ShopSystem(UIManager& uiManager);
    
    // Метод открытия магазина-заглушки:
    // Игрок может выбрать: 1. Идти в бой, 2. Выйти из игры.
    void openShop(Character& player, int currentRound);
    
private:
    UIManager& uiManager;
    
    // Вспомогательные методы
    void displayShopItems(const Character& player, int roundNumber);
    void buyItem(Character& player, int itemIndex);
    void upgradeCharacterStat(Character& player, int statIndex);
};

#endif // SHOP_SYSTEM_H 