#ifndef BATTLE_SYSTEM_H
#define BATTLE_SYSTEM_H

#include "Character.h"
#include "UIManager.h"
#include "AIController.h"
#include "BuffSystem.h"
#include "PotionSystem.h"
#include <string>
#include <vector>
#include <iostream>
#include <random>
#include <chrono>
#include <thread>

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

/**
 * @file BattleSystem.h
 * @brief Declares the BattleSystem class that manages combat logic between player and enemy.
 */

class UIManager;
class AIController;
class BuffSystem;
class PotionSystem;
struct EnemyAttack;

/**
 * @class BattleSystem
 * @brief Orchestrates combat flow, player and enemy actions, and handles battle states.
 *
 * Handles the main battle loop, input/output, usage of spells and items,
 * communicates with AI, buffs, UI, and manages turn-based mechanics between the player and the enemy.
 *
 * @see Character, AIController, BuffSystem, PotionSystem, UIManager, EnemyAttack
 */
class BattleSystem {
public:
    /**
     * @brief Constructs the BattleSystem and attaches subsystems (UI, AI, Buffs, Potions).
     * @param uiManager Reference to the UI manager.
     * @param ai Reference to the enemy AI controller.
     */
    BattleSystem(UIManager& uiManager, AIController& ai);

    /**
     * @brief Begins the battle sequence for a given round.
     * @param player Reference to the player character.
     * @param enemy Reference to the enemy character.
     * @param round Current round number.
     *
     * Manages the battle loop, alternating turns between player and enemy until one is defeated.
     */
    void startBattle(Character& player, Character& enemy, int round);

    /**
     * @brief Performs a standard player attack on the enemy.
     * @param player Reference to the player character.
     * @param enemy Reference to the enemy character.
     */
    void attackEnemy(Character& player, Character& enemy);

    /**
     * @brief Allows the player to cast a spell during their turn.
     * @param player Reference to the player.
     * @param enemy Reference to the enemy.
     * @param spellChoice The chosen spell (integer identifier).
     * @return True if casting succeeded, false otherwise (e.g. insufficient mana).
     */
    bool castMagic(Character& player, Character& enemy, int spellChoice);

    /**
     * @brief Allows the player to use an item from inventory in battle.
     * @param player Reference to the player character.
     * @param enemy Reference to the enemy character.
     */
    void useItem(Character& player, Character& enemy);

    /**
     * @brief Immediately exits the battle and game session.
     */
    void exitGame();

    /**
     * @brief Processes the player's turn, letting them choose and execute an action.
     * @param player Reference to the player character.
     * @param enemy Reference to the enemy character.
     */
    void processPlayerTurn(Character& player, Character& enemy);

    /**
     * @brief Executes a specific enemy action (attack/magic/etc) as determined by AI logic.
     * @param player Reference to the player (target).
     * @param enemy Reference to the enemy (executor).
     * @param action The action chosen by the AI for the enemy.
     * @param playerLastChoice Previous action chosen by the player.
     * @param playerLastSpell Previous spell used by the player (optional, default is 0).
     */
    void executeEnemyAction(Character& player, Character& enemy, const EnemyAttack& action, int playerLastChoice, int playerLastSpell = 0);

    /**
     * @brief Processes the full sequence for the player's turn including state tracking.
     * @param player Reference to the player character.
     * @param enemy Reference to the enemy character.
     * @param playerChoice Player's action choice for this turn.
     * @param playerLastChoice Reference to int for storing player's last action.
     * @param playerLastSpell Reference to int for storing player's last spell.
     * @param currentRound Number of the current round.
     */
    void executePlayerTurn(Character& player, Character& enemy, int playerChoice, int& playerLastChoice, int& playerLastSpell, int currentRound);

    /**
     * @brief Shows the battle outcome (win, lose, etc) on the UI.
     * @param result Short string description of the result ("Victory", "Defeat", etc).
     */
    void displayBattleResult(const std::string& result);

    /**
     * @brief Pauses for a dialogue or dramatic effect, using a configured duration.
     */
    void pauseForDialogue() const;

    /**
     * @brief Updates all active buffs on the player, expiring or refreshing as needed.
     * @param player Reference to the player character.
     */
    void updateBuffs(Character& player);

    /**
     * @brief Calculates final damage from attacker to defender based on stats and conditions.
     * @param attacker Attacking character.
     * @param defender Defending character.
     * @return Calculated integer value of damage.
     */
    int calculateDamage(const Character& attacker, const Character& defender);

    /**
     * @brief Changes the pause duration for dialogue effects.
     * @param milliseconds Desired pause duration in milliseconds.
     */
    void setDialoguePauseDuration(int milliseconds);

    /**
     * @brief Reads the current configured dialogue pause duration.
     * @return Milliseconds for pause.
     */
    int getDialoguePauseDuration() const;

    /**
     * @brief Shows the enemy's reaction text/statistics after being attacked.
     * @param enemy Reference to the enemy character.
     * @param successfulAttack Whether the attack was successful.
     * @param damage The damage dealt.
     */
    void displayEnemyResponseToAttack(Character& enemy, bool successfulAttack, int damage);

    /**
     * @brief Displays the enemy's response after the player dodged.
     * @param enemy Reference to the enemy character.
     * @param playerDodged True if the player successfully dodged.
     * @param playerLastSpell Indicates which spell (if any) the player used to dodge.
     */
    void displayEnemyResponseToDodge(Character& enemy, bool playerDodged, int playerLastSpell);

    /**
     * @brief Displays the enemy's reaction to being stunned by the player.
     * @param enemy Reference to the enemy character.
     * @param playerStunned True if the player has successfully stunned the enemy.
     */
    void displayEnemyResponseToStun(Character& enemy, bool playerStunned);

    /**
     * @brief Prints current enemy attack statistics for debugging or player hints.
     */
    void displayEnemyAttackStats();

    /**
     * @brief Displays a hint to the player about the enemy's next planned action.
     * @param action Enemy's next action (attack, spell, etc.).
     */
    void displayEnemyHint(const EnemyAttack& action);

private:
    UIManager& uiManager;      /**< Reference to the UI manager for all display and input. */
    AIController& aiController;/**< Reference to the AI controller for enemy decisions. */
    BuffSystem buffSystem;     /**< Manages active buffs and status effects in battle. */
    PotionSystem potionSystem; /**< Manages potion usage and inventory for player. */

    bool playerStunned;        /**< True if the player is currently stunned. */
    int playerStunDuration;    /**< Remaining stun duration turns for the player. */

    int dialoguePauseDuration; /**< Current pause duration for dialogues, in milliseconds. */

    /**
     * @brief Displays detailed info about the enemy on the UI.
     * @param enemy Reference to the enemy character.
     */
    void showEnemyInfo(const Character& enemy);

    /**
     * @brief Handles magic logic and effects for a spell cast in battle.
     * @param player Reference to the player.
     * @param enemy Reference to the enemy.
     * @param spellChoice Spell number to execute.
     */
    void useMagic(Character& player, Character& enemy, int spellChoice);

    /**
     * @brief Shows info or a hint about the enemy's next action in UI.
     * @param nextAction Reference to the upcoming enemy action.
     */
    void showEnemyInfo(const EnemyAttack& nextAction);

    /**
     * @brief Clears the terminal/console screen for clean UI output.
     */
    void clearScreen();
};

#endif