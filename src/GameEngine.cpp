#include "GameEngine.h"
#include "UIManager.h"
#include <iostream>
#include <limits>
#include <string>

GameEngine::GameEngine() : uiManager() {

}

GameEngine::~GameEngine() {

}

void GameEngine::startGame() {
    bool isRunning = true;
    
    while (isRunning) {
        int choice = uiManager.processMainMenu();

        switch (choice) {
            case 1:
                std::cout << "\n     Подготовка к бою...\n";
                std::cout << "\n     Нажмите Enter для продолжения...";
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                std::cin.get();
                break;
                
            case 2:
                processSettings();
                break;
                
            case 3:
                std::cout << "\n     Спасибо за игру! До встречи!\n";
                isRunning = false;
                break;
        }
    }
}

void GameEngine::processSettings() {
    int settingsChoice = uiManager.processSettingsMenu();
    
    switch (settingsChoice) {
        case 1:
            std::cout << "\n     Открываем GitHub репозиторий...\n";
            #ifdef _WIN32
            system("start https://github.com/YatsenkoYura/LabWork2");
            #elif defined(__APPLE__)
            system("open https://github.com/YatsenkoYura/LabWork2");
            #else
            system("xdg-open https://github.com/YatsenkoYura/LabWork2");
            #endif
            
            std::cout << "\n     Нажмите Enter для продолжения...";
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cin.get();
            break;
            
        case 2:
            break;
    }
} 