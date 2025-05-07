#ifndef AI_CONTROLLER_H
#define AI_CONTROLLER_H

#include "Character.h"
#include <vector>
#include <string>

/**
 * @file AIController.h
 * @brief Declares the classes and structures related to enemy AI behavior.
 */

class UIManager;

/**
 * @enum EnemyAttackType
 * @brief Types of attacks available to the enemy AI.
 */
enum class EnemyAttackType {
    NORMAL_ATTACK,  /**< Standard attack action. */
    DODGE_ATTACK,   /**< Evade or dodge action. */
    STUN_ATTACK,    /**< Attack with stun effect. */
    SKIP_TURN       /**< Skip the current turn. */
};

/**
 * @struct EnemyAttack
 * @brief Represents an enemy attack option with type, name, power, and cost.
 */
struct EnemyAttack {
    EnemyAttackType type;  /**< Type of the attack. */
    std::string name;      /**< Display name of the attack. */
    int power;             /**< Power value used for damage calculation. */
    int cost;              /**< Cost (e.g., mana or action points) to perform this attack. */
};


/**
 * @class AIController
 * @brief Implements enemy AI behavior, action selection and upgrades between rounds.
 *
 * This class is responsible for managing the logic of the enemy opponent in battle.
 * It selects actions to perform, upgrades enemy attributes between rounds, and interacts
 * with other systems (UI, characters, etc). Typically, an instance exists per AI-controlled enemy (e.g., Den).
 */
class AIController {
public:
    /**
     * @brief Constructor.
     * @param uiManager Reference to the UI manager for user interface interactions.
     */
    AIController(UIManager& uiManager);

    /**
     * @brief Distributes upgrade points for the enemy based on the player's choice/action.
     * @param enemy Reference to the enemy character.
     * @param playerChoice An integer representing the player's previous choice to take into account for enemy upgrades.
     *
     * Use this after each round, e.g. to allocate skill points based on how the player fought.
     */
    void distributePointsBasedOnPlayerChoice(Character& enemy, int playerChoice);

    /**
     * @brief Determines the next action for the enemy to take.
     * @param enemy Reference to the enemy character.
     * @param round Current round number.
     * @param predictionOnly If true, returns a predicted action, but does not actually perform it.
     * @return EnemyAttack describing the chosen or predicted action.
     *
     * Call this on the enemy's turn to decide their battle move.
     */
    EnemyAttack determineNextAction(Character& enemy, int round, bool predictionOnly = false);

    /**
     * @brief Selects and purchases a set of attacks for the given round based on AI logic.
     * @param enemy Reference to the enemy character.
     * @param round Current round number.
     *
     * Should be called at the start of each round.
     */
    void buyAttacksForRound(Character& enemy, int round);

    /**
     * @brief Handles the enemy's turn, selecting and performing an action versus the player.
     * @param enemy Reference to the AI-controlled character.
     * @param player Reference to the player character.
     */
    void processEnemyTurn(Character& enemy, Character& player);

    /**
     * @brief Performs a direct enemy attack on the player.
     * @param enemy Reference to the AI-controlled character.
     * @param player Reference to the player character.
     */
    void enemyAttack(Character& enemy, Character& player);

    /**
     * @brief Performs a magic attack or effect by the enemy against the player.
     * @param enemy Reference to the AI-controlled character.
     * @param player Reference to the player character.
     */
    void enemyUseMagic(Character& enemy, Character& player);

    /**
     * @brief Enables the enemy to use an item from their inventory.
     * @param enemy Reference to the AI-controlled character.
     */
    void enemyUseItem(Character& enemy);

    /**
     * @brief Calculates the damage done by one character to another.
     * @param attacker The character performing the attack.
     * @param defender The character defending.
     * @return The calculated amount of damage.
     */
    int calculateDamage(const Character& attacker, const Character& defender);

    /**
     * @brief Configures special behavior for the first round (optional).
     * Call this before the first round to set up unique AI behavior.
     */
    void setFirstRoundBehavior();

    /**
     * @brief Returns the list of available attacks the enemy can use.
     * @return Vector of EnemyAttack objects.
     */
    std::vector<EnemyAttack> getAvailableAttacks() const {
        return availableAttacks;
    }

private:
    UIManager& uiManager;  /**< Reference to the UI manager. */

    /**
     * @brief List of attacks available to the enemy in the current round.
     */
    std::vector<EnemyAttack> availableAttacks;

    /**
     * @brief List of all actions available for the enemy AI logic to choose from.
     */
    std::vector<EnemyAttack> availableActions;

    bool attackInFirstRound;          /**< Flag for special attack logic in the first round. */
    int firstRoundAttackCounter;      /**< Counter for first-round attacks. */
    int firstRoundSpecialAttackLimit; /**< Maximum count for using special attacks in the first round. */
    EnemyAttackType lastAttackType;   /**< The last attack type used by the enemy. */

    /**
     * @brief Helper: Purchases a specified count of attack type for the enemy.
     * @param type The type of attack to acquire.
     * @param count How many attacks of this type to purchase.
     * @param power Attack power value.
     * @param manaCost Mana or resource cost for the attack.
     * @param name Display name for the attack.
     */
    void buyAttackOfType(EnemyAttackType type, int count, int power, int manaCost, const std::string& name);

    /**
     * @brief Resets the availableAttacks list at the beginning of a new round.
     */
    void resetAvailableAttacks();
};

#endif