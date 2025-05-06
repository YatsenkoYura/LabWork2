#ifndef BUFF_SYSTEM_H
#define BUFF_SYSTEM_H

#include "Character.h"
#include "UIManager.h"
#include <string>
#include <vector>

/**
 * @file BuffSystem.h
 * @brief Contains declarations for ActiveBuff struct and BuffSystem class (manages character buffs).
 */

/**
 * @struct ActiveBuff
 * @brief Describes an active buff, providing stat bonuses, regeneration, and duration.
 *
 * Each ActiveBuff represents a temporary effect, such as a spell or potion buff,
 * which can boost stats and/or regenerate resources for a limited duration per character.
 */
struct ActiveBuff {
    std::string name;    /**< Name/id of the buff. */
    int attackBonus;     /**< Increase to attack stat while buff is active. */
    int defenseBonus;    /**< Increase to defense stat while buff is active. */
    int healthRegen;     /**< Amount of health restored per turn. */
    int manaRegen;       /**< Amount of mana restored per turn. */
    int duration;        /**< Remaining number of turns this buff will last. */

    /**
     * @brief Constructs an ActiveBuff with given parameters.
     * @param n Name of the buff.
     * @param atk Bonus to attack.
     * @param def Bonus to defense.
     * @param hp Amount of health restored per turn.
     * @param mp Amount of mana restored per turn.
     * @param dur Duration of buff (in turns).
     */
    ActiveBuff(const std::string& n, int atk, int def, int hp, int mp, int dur)
            : name(n), attackBonus(atk), defenseBonus(def),
              healthRegen(hp), manaRegen(mp), duration(dur) {}
};

/**
 * @class BuffSystem
 * @brief Manages application, updating, and removal of buffs for a character.
 *
 * Handles the lifecycle of all temporary stat-modifying effects, their display,
 * and automated turn-by-turn updating, including regeneration and expiration.
 */
class BuffSystem {
public:
    /**
     * @brief Constructs a new BuffSystem.
     * @param uiManager Reference to the UI manager for visual/audible feedback.
     */
    BuffSystem(UIManager& uiManager);

    /**
     * @brief Adds a new buff to a character.
     * @param character Target character to buff.
     * @param name Name/id of the buff (for display and uniqueness).
     * @param attackBonus Added attack stat.
     * @param defenseBonus Added defense stat.
     * @param healthRegen Regeneration of health per turn.
     * @param manaRegen Regeneration of mana per turn.
     * @param duration Number of turns this buff lasts.
     *
     * If a buff with the same name exists, resets its duration and bonuses.
     */
    void addBuff(Character& character, const std::string& name, int attackBonus, int defenseBonus,
                 int healthRegen, int manaRegen, int duration);

    /**
     * @brief Updates all active buffs: applies effects, decreases duration, and removes expired buffs.
     * @param character Target character whose buffs should be updated.
     *
     * This should be called once per character turn.
     */
    void updateBuffs(Character& character);

    /**
     * @brief Removes all buffs (used on defeat, restarts, etc).
     */
    void clearBuffs();

    /**
     * @brief Checks if a buff with a given name is currently active.
     * @param name Name of the buff to check.
     * @return True if such buff exists, false otherwise.
     */
    bool hasBuff(const std::string& name) const;

private:
    UIManager& uiManager;                  /**< Reference to UI manager. */
    std::vector<ActiveBuff> activeBuffs;   /**< List of currently active buffs. */

    /**
     * @brief Pauses the flow for display/dialogue purposes (e.g. new buff shows).
     */
    void pauseForDialogue() const;
};

#endif