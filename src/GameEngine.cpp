#include "GameEngine.h"
#include "UIManager.h"


GameEngine::GameEngine() : uiManager() {

}

GameEngine::~GameEngine() {

}

void GameEngine::startGame() {
    while (true) {
        uiManager.showScreen("main");
    }
} 