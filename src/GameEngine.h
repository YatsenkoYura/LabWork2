#ifndef GAME_ENGINE_H
#define GAME_ENGINE_H

#include "Character.h"
#include "BattleSystem.h"
#include "AIController.h"
#include "ShopSystem.h"
#include "InventorySystem.h"
#include "UIManager.h"
#include <cstdlib>

#ifdef _WIN32
#include <windows.h> // Для Sleep()
#else
#include <unistd.h> // Для sleep()
#endif

class GameEngine {
public:
    GameEngine();
    ~GameEngine();
    
    void startGame();
    void run();
    
private:
    // Объекты для управления игровыми системами
    UIManager uiManager;
    AIController aiController;
    BattleSystem battleSystem;
    ShopSystem shopSystem;
    InventorySystem inventorySystem;
    
    // Игровые переменные
    int currentRound;
    int score;
    
    // Персонажи
    Character player;
    Character enemy;
    
    // Игровые методы
    void processSettings();
    void initializeGame();
    void runGameLoop();
    void gameOver();
    
    // Методы управления персонажами
    void setupCharacters();
    void updateEnemyForNextRound();
    bool processBattle();
    void handlePostVictory(Character& player);
};

#endif // GAME_ENGINE_H
