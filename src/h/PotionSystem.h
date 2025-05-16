#ifndef POTION_SYSTEM_H
#define POTION_SYSTEM_H

#include "Character.h"
#include "UIManager.h"
#include "BuffSystem.h"

/**
 * @file PotionSystem.h
 * @brief Simple manager for using different types of potions.
 */

/// Potion effect types.
enum class PotionType {
    HEALTH_POTION,    ///< Heals HP.
    MANA_POTION,      ///< Restores MP.
    BUFF_POTION,      ///< Increases stats.
    REGEN_POTION,     ///< Grants regeneration over time.
    SPECIAL_POTION    ///< Any special effect.
};

/**
 * @class PotionSystem
 * @brief Handles using potions and applying their effects to a Character.
 */
class PotionSystem {
public:
    /// Constructor.
    PotionSystem(UIManager& uiManager, BuffSystem& buffSystem);
    /// Uses a potion (auto-detects its type). Returns true if used.
    bool usePotion(Character& character, const Item& item);
    /// Uses a health potion. Returns true if used.
    bool useHealthPotion(Character& character, const Item& item);

    /// Uses a mana potion. Returns true if used.
    bool useManaPotion(Character& character, const Item& item);

    /// Uses a stat buff potion. Returns true if used.
    bool useBuffPotion(Character& character, const Item& item);

    /// Uses a regeneration potion. Returns true if used.
    bool useRegenPotion(Character& character, const Item& item);

    /// Uses a special potion. Returns true if used.
    bool useSpecialPotion(Character& character, const Item& item);

private:
    UIManager& uiManager;
    BuffSystem& buffSystem;

    /// Helper to extract a value from item description (e.g. "100 HP").
    int extractValueFromDescription(const std::string& description, const std::string& suffix);
    /// Short pause for player to read messages.
    void pauseForDialogue() const;
};

#endif