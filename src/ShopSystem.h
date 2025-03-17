#ifndef SHOP_SYSTEM_H
#define SHOP_SYSTEM_H

#include "Character.h"
#include "UIManager.h"
#include <vector>
#include <string>

// Полное определение структуры ShopItem
struct ShopItem {
    std::string name;
    std::string description;
    int price;
    int value; // Величина эффекта предмета
    enum ItemType {
        HEALTH_POTION,
        MANA_POTION,
        WEAPON,
        ARMOR,
        SPECIAL,
        BUFF_POTION,    // Новый тип: зелья с временным баффом
        REGEN_POTION,   // Новый тип: зелья с эффектом регенерации
        DEBUFF_POTION   // Новый тип: зелья с негативным эффектом на врага
    } type;
};

class ShopSystem {
public:
    ShopSystem(UIManager& uiManager);
    
    // Метод открытия магазина
    void openShop(Character& player, int currentRound);
    
private:
    UIManager& uiManager;
    
    // Базовый набор товаров для магазина
    std::vector<ShopItem> baseShopItems;
    
    // Текущий набор доступных товаров
    std::vector<ShopItem> availableItems;
    
    // Вспомогательные методы
    void generateShopItems(int roundNumber);
    void displayShopItems(const Character& player, int roundNumber);
    void buyItem(Character& player, int itemIndex);
    void applyItemEffect(Character& player, const ShopItem& item);
    
    // Методы для улучшения снаряжения
    int calculateUpgradeCost(int currentStat, int roundNumber);
    void upgradeArmor(Character& player, int roundNumber);
    void upgradeWeapon(Character& player, int roundNumber);
};

#endif // SHOP_SYSTEM_H 