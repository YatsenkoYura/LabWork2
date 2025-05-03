#include "../h/GameEngine.h"
#include "../h/UIManager.h"
#include <iostream>
#include <limits>
#include <string>

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

GameEngine::GameEngine() 
    : uiManager(), 
      aiController(uiManager), 
      battleSystem(uiManager, aiController), 
      shopSystem(uiManager),
      inventorySystem(uiManager),
      scoreManager(uiManager),
      currentRound(1) {
    
    setupCharacters();
}

GameEngine::~GameEngine() {
}

void GameEngine::run() {
    try {
        bool isRunning = true;

        while (isRunning) {
            try {
                uiManager.displayMainMenu();
                int choice = uiManager.getPlayerChoice();
                
                switch (choice) {
                    case 1:
                        initializeGame();
                        runGameLoop();
                        break;
                    case 2:
                        processSettings();
                        break;
                    case 3:
                        loadGame();
                        break;
                    case 4:
                        scoreManager.displayLeaderboard();
                        break;
                    case 5:
                        isRunning = false;
                        break;
                    default:
                        std::cout << "Неверный выбор. Попробуйте снова." << std::endl;
                        break;
                }
            } catch (const std::exception& e) {
                std::cerr << "Ошибка во время выполнения: " << e.what() << std::endl;
                #ifdef _WIN32
                Sleep(3000);
                #else
                usleep(3000000);
                #endif
            } catch (...) {
                std::cerr << "Неизвестная ошибка во время выполнения" << std::endl;
                #ifdef _WIN32
                Sleep(3000);
                #else
                usleep(3000000);
                #endif
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "Критическая ошибка: " << e.what() << std::endl;
    } catch (...) {
        std::cerr << "Неизвестная критическая ошибка" << std::endl;
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
            
            #ifdef _WIN32
            Sleep(1000);
            #else
            usleep(1000000);
            #endif
            break;
            
        case 2:
            uiManager.displaySettingsMenu(battleSystem);
            break;
            
        case 3:
            return;
    }
}

void GameEngine::initializeGame() {
    currentRound = 1;
    scoreManager.setScore(0);
    
    setupCharacters();
}

void GameEngine::runGameLoop() {
    bool gameInProgress = true;
    
    while (gameInProgress) {
        bool exitGame = showGameMenu();
        if (exitGame) {
            gameInProgress = false;
            continue;
        }
        
        bool playerWon = processBattle();
        
        if (playerWon) {
            scoreManager.addScore(100 * currentRound);
            
            handlePostVictory(player);
            
            currentRound++;
            updateEnemyForNextRound();
        } else {
            gameOver();
            gameInProgress = false;
        }
    }
}

bool GameEngine::showGameMenu() {
    bool exitGame = false;
    
    uiManager.clearScreen();
    std::cout << "\n\n";
    std::cout << "            ИГРОВОЕ МЕНЮ            " << std::endl;
    std::cout << "-------------------------------------" << std::endl;
    std::cout << "\nТекущий раунд: " << currentRound << std::endl;
    std::cout << "Текущий счет: " << scoreManager.getScore() << std::endl;
    
    std::cout << "\nВыберите действие:" << std::endl;
    std::cout << "1. Продолжить игру" << std::endl;
    std::cout << "2. Сохранить игру" << std::endl;
    std::cout << "3. Выйти в главное меню" << std::endl;
    
    char choice = uiManager.getCharImmediate();
    
    switch (choice) {
        case '1':
            return false; // Продолжаем игру
        case '2':
            saveGame();
            return showGameMenu(); // Возвращаемся в меню после сохранения
        case '3':
            return true; // Выходим в главное меню
        default:
            return showGameMenu(); // Если неверный ввод, повторяем меню
    }
}

void GameEngine::saveGame() {
    scoreManager.displaySaveGameMenu(player, currentRound);
}

void GameEngine::loadGame() {
    std::string playerName;
    bool loaded = scoreManager.loadGame(player, currentRound, playerName);
    
    if (loaded) {
        updateEnemyForNextRound(); // Обновляем статы врага для текущего раунда
        runGameLoop(); // Запускаем игровой цикл с загруженными данными
    }
}

void GameEngine::gameOver() {
    scoreManager.processGameOver(currentRound - 1);
}

void GameEngine::setupCharacters() {
    player = Character("Герой", 50, 0, 30, true, 12);
    
    enemy = Character("Дэн", 40, 0, 20, true, 5);
}

void GameEngine::updateEnemyForNextRound() {
    enemy = Character("Дэн", 
                     40 + 15 * currentRound,
                     0,
                     20 + 10 * currentRound,
                     true,
                     5 + 2 * currentRound);
    
    aiController.buyAttacksForRound(enemy, currentRound);
}

bool GameEngine::processBattle() {
    aiController.buyAttacksForRound(enemy, currentRound);
    
    battleSystem.startBattle(player, enemy, currentRound);
    
    if (!enemy.isAlive()) {
        scoreManager.addScore(100 * currentRound);
        return true;
    } else {
        return false;
    }
}

void GameEngine::handlePostVictory(Character &player) {
    #ifdef _WIN32
    system("cls");
    #else
    system("clear");
    #endif
    
    player.heal(player.getMaxHealth());
    
    if (player.doesHaveMana()) {
        int manaToRestore = player.getMaxMana() - player.getMana();
        if (manaToRestore > 0) {
            player.useMana(-manaToRestore);
        }
    }
    
    std::cout << "\n\n";
    std::cout << "                  ПОБЕДА!                  " << std::endl;
    std::cout << "------------------------------------------------" << std::endl;
    std::cout << "\nВы победили врага! Получено 100 очков." << std::endl;
    std::cout << "\nЗдоровье и мана полностью восстановлены!" << std::endl;
    std::cout << "\nТекущие характеристики:" << std::endl;
    std::cout << "Здоровье: " << player.getHealth() << "/" << player.getMaxHealth() << std::endl;
    std::cout << "Атака: " << player.getAttack() << std::endl;
    std::cout << "Мана: " << player.getMana() << "/" << player.getMaxMana() << std::endl;
    std::cout << "Текущий счет: " << scoreManager.getScore() << std::endl;
    
    int points = 3;
    
    int lastPlayerChoice = 0;
    
    while (points > 0) {
        #ifdef _WIN32
        system("cls");
        #else
        system("clear");
        #endif
        
        std::cout << "\n\n";
        std::cout << "            РАСПРЕДЕЛЕНИЕ ОЧКОВ            " << std::endl;
        std::cout << "------------------------------------------------" << std::endl;
        std::cout << "\nОсталось очков: " << points << std::endl;
        std::cout << "\nТекущие характеристики:" << std::endl;
        std::cout << "Здоровье: " << player.getHealth() << "/" << player.getMaxHealth() << std::endl;
        std::cout << "Атака: " << player.getAttack() << std::endl;
        std::cout << "Мана: " << player.getMana() << "/" << player.getMaxMana() << std::endl;
        std::cout << "Текущий счет: " << scoreManager.getScore() << std::endl;
        
        std::cout << "\nВыберите атрибут для улучшения:" << std::endl;
        std::cout << "1. Здоровье (+15)" << std::endl;
        std::cout << "2. Атака (+3)" << std::endl;
        std::cout << "3. Мана (+5)" << std::endl;
        
        char choice = uiManager.getCharImmediate();
        
        switch(choice) {
            case '1':
                player.boostHealth(15);
                lastPlayerChoice = 1;
                points--;
                break;
            case '2':
                player.boostAttack(3);
                lastPlayerChoice = 2;
                points--;
                break;
            case '3':
                player.boostMana(5);
                lastPlayerChoice = 3;
                points--;
                break;
            default:
                break;
        }
        
        if (lastPlayerChoice > 0) {
            aiController.distributePointsBasedOnPlayerChoice(enemy, lastPlayerChoice);
            
            lastPlayerChoice = 0;
        }
    }
    
    #ifdef _WIN32
    system("cls");
    #else
    system("clear");
    #endif
    
    std::cout << "\n\n";
    std::cout << "       РАСПРЕДЕЛЕНИЕ ОЧКОВ ЗАВЕРШЕНО        " << std::endl;
    std::cout << "------------------------------------------------" << std::endl;
    std::cout << "\nВаши финальные характеристики:" << std::endl;
    std::cout << "Здоровье: " << player.getHealth() << "/" << player.getMaxHealth() << std::endl;
    std::cout << "Атака: " << player.getAttack() << std::endl;
    std::cout << "Мана: " << player.getMana() << "/" << player.getMaxMana() << std::endl;
    std::cout << "Текущий счет: " << scoreManager.getScore() << std::endl;
    
    shopSystem.openShop(player, currentRound);
} 