#ifndef GAME_ENGINE_H
#define GAME_ENGINE_H

#include "Character.h"
#include "UIManager.h"
#include "AIController.h"
#include "BattleSystem.h"
#include "ShopSystem.h"
#include "InventorySystem.h"
#include "ScoreManager.h"

/**
 * @file GameEngine.h
 * @brief Declares the GameEngine class, which manages the overall lifecycle and flow of the game.
 */

/**
 * @class GameEngine
 * @brief The central class that manages the game state, controls the main loop, components, and player progress.
 *
 * Responsible for initializing all major subsystems, orchestrating the main menu, settings, battle, inventory,
 * score management, and the overall flow of rounds. Acts as the root coordinator for all gameplay.
 */
class GameEngine {
public:
    /**
     * @brief Constructs the GameEngine and initializes all subsystems.
     */
    GameEngine();

    /**
     * @brief Cleans up resources and safely shuts down subsystems.
     */
    ~GameEngine();

    /**
     * @brief Launches and runs the game. Handles the primary game loop and transitions.
     *
     * Call this to begin the full game experience, from start screen through victory/defeat and quitting.
     */
    void run();

private:
    UIManager uiManager;            /**< Handles all UI input/output functionality. */
    AIController aiController;      /**< Orchestrates enemy AI behaviors. */
    BattleSystem battleSystem;      /**< Manages combat logic and player/enemy turns. */
    ShopSystem shopSystem;          /**< Handles in-game shop and upgrades. */
    InventorySystem inventorySystem;/**< Manages inventory and item usage. */
    ScoreManager scoreManager;      /**< Manages high score and leaderboard functionality. */

    Character player;               /**< The player's main character object. */
    Character enemy;                /**< The current (or next) enemy character. */

    int currentRound;               /**< Current battle round (increases with victories). */

    /**
     * @brief Prepares and initializes key game components at the start or reset.
     */
    void initializeGame();

    /**
     * @brief Runs the main game loop, including screens, rounds, and transitions.
     */
    void runGameLoop();

    /**
     * @brief Initializes or resets main player and enemy character stats and inventory.
     */
    void setupCharacters();

    /**
     * @brief Upgrades enemy stats and state to match next round difficulty.
     */
    void updateEnemyForNextRound();

    /**
     * @brief Processes settings screen and applies any changes.
     */
    void processSettings();

    /**
     * @brief Executes one full battle; returns whether player survived or not.
     * @return True if the player survived the battle, false if defeated.
     */
    bool processBattle();

    /**
     * @brief Called when the game ends (player defeat), updates scores and shows summary screens.
     */
    void gameOver();

    /**
     * @brief Handles rewards, shop and upgrade flow after each player victory.
     * @param player Reference to the main player character (for upgrades and item grants).
     */
    void handlePostVictory(Character& player);

    /**
     * @brief Shows the main game menu and processes menu commands.
     * @return True if the user continues, false if exits to OS.
     */
    bool showGameMenu();

    /**
     * @brief Saves the current game state to file (or persistent storage).
     */
    void saveGame();

    /**
     * @brief Loads the game state from file (or persistent storage).
     */
    void loadGame();
};

#endif