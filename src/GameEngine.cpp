#include "GameEngine.h"
#include "UIManager.h"
#include <iostream>
#include <limits>
#include <string>

GameEngine::GameEngine() 
    : uiManager(), 
      battleSystem(uiManager), 
      aiController(uiManager), 
      shopSystem(uiManager),
      inventorySystem(uiManager),
      currentRound(1),
      score(0) {
    
    // Инициализация персонажей
    setupCharacters();
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
                initializeGame();
                runGameLoop();
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

void GameEngine::initializeGame() {
    // Сброс игровых переменных
    currentRound = 1;
    score = 0;
    
    // Настройка персонажей для начала игры
    setupCharacters();
}

void GameEngine::runGameLoop() {
    bool gameInProgress = true;
    
    while (gameInProgress) {
        // Бой с противником
        bool playerWon = processBattle();
        
        if (playerWon) {
            // Игрок победил
            score += 100 * currentRound;
            
            // Открываем магазин
            shopSystem.openShop(player, currentRound);
            
            // Подготовка к следующему раунду
            currentRound++;
            updateEnemyForNextRound();
        } else {
            // Игрок проиграл
            gameOver();
            gameInProgress = false;
        }
    }
}

void GameEngine::gameOver() {
    std::cout << "Игра окончена! Ваш счет: " << score << std::endl;
    std::cout << "Вы прошли " << currentRound - 1 << " раундов." << std::endl;
    
    // Здесь можно добавить сохранение результата в таблицу лидеров
    
    std::cout << "Нажмите Enter для продолжения...";
    std::cin.ignore();
    std::cin.get();
}

void GameEngine::setupCharacters() {
    // Настройка игрока
    player = Character("Герой", 100, 10, 50, true);
    
    // Настройка противника
    enemy = Character("Дэн", 80, 8, 0, false); // Убираем ману у Дэна
}

void GameEngine::updateEnemyForNextRound() {
    // Увеличение характеристик противника с каждым раундом
    enemy = Character("Дэн", 
                     80 + 20 * currentRound, 
                     8 + 2 * currentRound, 
                     0,  // Мана всегда 0
                     false); // hasMana = false
}

bool GameEngine::processBattle() {
    battleSystem.startBattle(player, enemy);
    
    if (!enemy.isAlive()) {
        score += 100;
        return true;
    } else {
        uiManager.displayBattleResult("Поражение! Вы пали в бою...");
        return false;
    }
} 