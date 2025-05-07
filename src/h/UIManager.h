#ifndef UIMANAGER_H
#define UIMANAGER_H

#include <iostream>
#include <string>
#include "Character.h"

#ifndef _WIN32
#include <termios.h>
#include <unistd.h>
#endif

class BattleSystem;

/**
 * @file UIManager.h
 * @brief Handles all user interface output and input.
 */

/**
 * @class UIManager
 * @brief Manages game UI: screens, menus, battle options, and user input.
 */
class UIManager {
public:
    /// Constructor.
    UIManager();
    /// Destructor.
    ~UIManager();

    /// Show a named UI screen (by name string).
    void showScreen(const std::string& screenName);

    /// Get numeric option from player input (e.g., menu choice).
    int getPlayerChoice();

    /// Get a single char from user immediately (no enter needed).
    char getCharImmediate();

    /// Show combat menu options for the player.
    void displayBattleOptions();

    /// Show spell casting options for the player.
    void displayMagicOptions(const Character& player);

    /// Display full battle status (player, enemy stats, etc).
    void displayBattleScreen(const Character& player, const Character& enemy);

    /// List the player's inventory.
    void displayInventory(const Character& player);

    /// Display battle result message (win/lose/etc).
    void displayBattleResult(const std::string& message);

    /// Clear the terminal screen.
    void clearScreen();

    /// Main menu handler, returns menu option.
    int processMainMenu();

    /// Settings menu handler, returns menu option.
    int processSettingsMenu();

    /// Show welcome at game start.
    void displayWelcomeScreen();

    /// Draw the main menu.
    void displayMainMenu();

    /// Draw the settings menu.
    void displaySettingsMenu(BattleSystem& battleSystem);

    /// Allow user to change dialogue delay settings.
    void changeDialoguePauseSettings(BattleSystem& battleSystem);

    /// Show a game over screen with final score.
    void displayGameOverScreen(int score);

    /// Show difficulty selection screen.
    void displayDifficultyMenu();

private:
#ifndef _WIN32
    struct termios originalTermios; ///< Stored terminal settings for restoring after raw mode.
    /// Switch terminal to raw (unbuffered) input mode.
    void setNonCanonicalMode();
    /// Restore normal terminal mode.
    void restoreTerminalMode();
#endif
};

#endif