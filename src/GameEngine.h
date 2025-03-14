#ifndef GAMEENGINE_H
#define GAMEENGINE_H

#include "UIManager.h"
#include "Character.h"
#include "BattleSystem.h"
#include "AIController.h"
#include "ShopSystem.h"
#include "InventorySystem.h"
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
    void processSettings();
    
    // Основные методы игрового процесса
    void initializeGame();
    void runGameLoop();
    void gameOver();
    bool processBattle();
    
private:
    UIManager uiManager;
    BattleSystem battleSystem;
    AIController aiController;
    ShopSystem shopSystem;
    InventorySystem inventorySystem;
    
    Character player;
    Character enemy;
    
    int currentRound;
    int score;
    
    // Вспомогательные методы
    void setupCharacters();
    void updateEnemyForNextRound();
};

#endif
