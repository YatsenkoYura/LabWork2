#ifndef GAMEENGINE_H
#define GAMEENGINE_H

#include "UIManager.h"

class GameEngine {  
public:
    GameEngine();
    ~GameEngine();
    void startGame();
private:
    UIManager uiManager;
};

#endif
