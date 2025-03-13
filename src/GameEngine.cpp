#include "GameEngine.h"
#include "UIManager.h"

void GameEngine::init() {
    uiManager.showScreen("start");
}

void GameEngine::startGame() {
    while (true) {
        uiManager.showScreen("main");
    }
} 