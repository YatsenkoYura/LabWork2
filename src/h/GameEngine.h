#ifndef GAME_ENGINE_H
#define GAME_ENGINE_H

#include "Character.h"
#include "UIManager.h"
#include "AIController.h"
#include "BattleSystem.h"
#include "ShopSystem.h"
#include "InventorySystem.h"
#include "ScoreManager.h"

class GameEngine {
public:
    GameEngine();
    ~GameEngine();
    
    void run();
    
private:
    UIManager uiManager;
    AIController aiController;
    BattleSystem battleSystem;
    ShopSystem shopSystem;
    InventorySystem inventorySystem;
    ScoreManager scoreManager;
    
    Character player;
    Character enemy;
    
    int currentRound;
    
    void initializeGame();
    void runGameLoop();
    void setupCharacters();
    void updateEnemyForNextRound();
    void processSettings();
    bool processBattle();
    void gameOver();
    void handlePostVictory(Character& player);
    
    bool showGameMenu();
    void saveGame();
    void loadGame();
};

#endif
