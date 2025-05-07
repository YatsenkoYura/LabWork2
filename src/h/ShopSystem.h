#ifndef SHOP_SYSTEM_H
#define SHOP_SYSTEM_H

#include "Character.h"
#include "UIManager.h"
#include <vector>
#include <string>

/**
 * @file ShopSystem.h
 * @brief Handles shop item management and upgrades during the game.
 */

/**
 * @struct ShopItem
 * @brief Represents an item that can be bought in the shop.
 */
struct ShopItem {
    std::string name;        ///< Item name.
    std::string description; ///< Short description for the player.
    int price;               ///< Gold cost.
    int value;               ///< Amount/stat provided by purchase.
    /// Type of shop item.
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

/**
 * @class ShopSystem
 * @brief Manages in-game shop: offers, sales, and stat upgrades for the player.
 */
class ShopSystem {
public:
    /// Constructor.
    ShopSystem(UIManager& uiManager);

    /// Open shop menu for the player for the current round.
    void openShop(Character& player, int currentRound);

private:
    UIManager& uiManager;                  ///< Reference to UI.
    std::vector<ShopItem> baseShopItems;   ///< Shop item template list.
    std::vector<ShopItem> availableItems;  ///< Items available for purchase this round.

    /// Generate available shop items based on difficulty round.
    void generateShopItems(int roundNumber);
    /// Display all products and stats to the player.
    void displayShopItems(const Character& player, int roundNumber);
    /// Buy item using index from the shop list.
    void buyItem(Character& player, int itemIndex);
    /// Grant purchased item's effect to the player.
    void applyItemEffect(Character& player, const ShopItem& item);

    /// Calculate the cost for upgrading a stat this round.
    int calculateUpgradeCost(int currentStat, int roundNumber);
    /// Handle armor stat upgrade.
    void upgradeArmor(Character& player, int roundNumber);
    /// Handle weapon stat upgrade.
    void upgradeWeapon(Character& player, int roundNumber);
};

#endif