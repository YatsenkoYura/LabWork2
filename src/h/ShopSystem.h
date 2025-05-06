#ifndef SHOP_SYSTEM_H
#define SHOP_SYSTEM_H

#include "Character.h"
#include "UIManager.h"
#include <vector>
#include <string>


struct ShopItem {
    std::string name;
    std::string description;
    int price;
    int value;
    enum ItemType {
        HEALTH_POTION,
        MANA_POTION,
        WEAPON,
        ARMOR,
        SPECIAL,
        BUFF_POTION,
        REGEN_POTION,
        DEBUFF_POTION
    } type;
};

class ShopSystem {
public:
    ShopSystem(UIManager& uiManager);
    
    void openShop(Character& player, int currentRound);
    
private:
    UIManager& uiManager;
    
    std::vector<ShopItem> baseShopItems;
    
    std::vector<ShopItem> availableItems;
    
    void generateShopItems(int roundNumber);
    void displayShopItems(const Character& player, int roundNumber);
    void buyItem(Character& player, int itemIndex);
    void applyItemEffect(Character& player, const ShopItem& item);
    
    int calculateUpgradeCost(int currentStat, int roundNumber);
    void upgradeArmor(Character& player, int roundNumber);
    void upgradeWeapon(Character& player, int roundNumber);
};

#endif