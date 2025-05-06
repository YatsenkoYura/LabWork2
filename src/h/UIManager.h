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

class UIManager {
public:
    UIManager();
    ~UIManager();
    void showScreen(const std::string& screenName);
    int getPlayerChoice();
    char getCharImmediate();
    
    void displayBattleOptions();
    void displayMagicOptions(const Character& player);
    void displayBattleScreen(const Character& player, const Character& enemy);
    void displayInventory(const Character& player);
    void displayBattleResult(const std::string& message);
    void clearScreen();

    int processMainMenu();
    int processSettingsMenu();

    void displayWelcomeScreen();
    void displayMainMenu();
    void displaySettingsMenu(BattleSystem& battleSystem);
    void changeDialoguePauseSettings(BattleSystem& battleSystem);
    void displayGameOverScreen(int score);
    void displayDifficultyMenu();

private:
#ifndef _WIN32
    struct termios originalTermios;
    void setNonCanonicalMode();
    void restoreTerminalMode();
#endif
};

#endif