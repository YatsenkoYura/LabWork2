#include "GameEngine.h"
#include "UIManager.h"
#include <iostream>
#include <limits>
#include <string>

GameEngine::GameEngine() 
    : uiManager(), 
      aiController(uiManager), 
      battleSystem(uiManager, aiController), 
      shopSystem(uiManager),
      inventorySystem(uiManager),
      currentRound(1),
      score(0) {
    
    // Инициализация персонажей
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
                    case 1: // Новая игра
                        initializeGame();
                        runGameLoop();
                        break;
                    case 2: // Настройки
                        processSettings();
                        break;
                    case 3: // Выход
                        isRunning = false;
                        break;
                    default:
                        std::cout << "Неверный выбор. Попробуйте снова." << std::endl;
                        break;
                }
            } catch (const std::exception& e) {
                std::cerr << "Ошибка во время выполнения: " << e.what() << std::endl;
                // Небольшая пауза, чтобы пользователь мог прочитать сообщение
                #ifdef _WIN32
                Sleep(3000);
                #else
                usleep(3000000);
                #endif
            } catch (...) {
                std::cerr << "Неизвестная ошибка во время выполнения" << std::endl;
                // Небольшая пауза
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
        case 1: // Открыть GitHub
            std::cout << "\n     Открываем GitHub репозиторий...\n";
            #ifdef _WIN32
            system("start https://github.com/YatsenkoYura/LabWork2");
            #elif defined(__APPLE__)
            system("open https://github.com/YatsenkoYura/LabWork2");
            #else
            system("xdg-open https://github.com/YatsenkoYura/LabWork2");
            #endif
            
            // Небольшая задержка
            #ifdef _WIN32
            Sleep(1000);
            #else
            usleep(1000000);
            #endif
            break;
            
        case 2: // Настройка пауз в диалогах
            uiManager.displaySettingsMenu(battleSystem);
            break;
            
        case 3: // Вернуться в главное меню
            return;
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
            
            // Показываем экран распределения очков
            handlePostVictory(player);
            
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
    
    // Небольшая задержка перед возвратом в меню
    #ifdef _WIN32
    Sleep(2000);
    #else
    usleep(2000000);
    #endif
}

void GameEngine::setupCharacters() {
    // Настройка игрока - убираем защиту (делаем её 0)
    player = Character("Герой", 100, 0, 50, true, 10);
    
    // Настройка противника - даем ему ману для очков действий
    enemy = Character("Дэн", 80, 0, 30, true, 8); // Даем Дэну 30 очков действий
}

void GameEngine::updateEnemyForNextRound() {
    // Увеличение характеристик противника с каждым раундом
    enemy = Character("Дэн", 
                     80 + 20 * currentRound,  // Увеличение здоровья
                     0,                       // Защита всегда 0
                     30 + 10 * currentRound,  // Увеличение очков действий (маны) с каждым раундом
                     true,                    // hasMana = true для очков действий
                     8 + 2 * currentRound);   // Увеличение атаки
    
    // Закупка атак для нового раунда
    aiController.buyAttacksForRound(enemy, currentRound);
}

bool GameEngine::processBattle() {
    // Подготавливаем атаки для Дэна перед началом раунда
    aiController.buyAttacksForRound(enemy, currentRound);
    
    battleSystem.startBattle(player, enemy, currentRound);
    
    if (!enemy.isAlive()) {
        // Увеличиваем счет за победу
        score += 100 * currentRound;
        return true;
    } else {
        // Игрок проиграл
        return false;
    }
}

void GameEngine::handlePostVictory(Character &player) {
    // Сначала очищаем экран
    #ifdef _WIN32
    system("cls");
    #else
    system("clear");
    #endif
    
    // Полностью восстанавливаем здоровье игрока
    player.heal(player.getMaxHealth());
    
    // Восстанавливаем ману игрока до максимума, а не превышая её
    if (player.doesHaveMana()) {
        // Вычисляем, сколько маны нужно восстановить
        int manaToRestore = player.getMaxMana() - player.getMana();
        if (manaToRestore > 0) {
            player.useMana(-manaToRestore); // Используем отрицательное значение для восстановления
        }
    }
    
    // Сообщение о победе
    std::cout << "\n\n";
    std::cout << "                  ПОБЕДА!                  " << std::endl;
    std::cout << "------------------------------------------------" << std::endl;
    std::cout << "\nВы победили врага! Получено 100 очков." << std::endl;
    std::cout << "\nЗдоровье и мана полностью восстановлены!" << std::endl;
    std::cout << "\nТекущие характеристики:" << std::endl;
    std::cout << "Здоровье: " << player.getHealth() << "/" << player.getMaxHealth() << std::endl;
    std::cout << "Атака: " << player.getAttack() << std::endl;
    std::cout << "Мана: " << player.getMana() << "/" << player.getMaxMana() << std::endl;
    
    // Даем 3 очка вместо 5
    int points = 3;
    
    // Для отслеживания выбора игрока
    int lastPlayerChoice = 0;
    
    // Распределение очков
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
        
        std::cout << "\nВыберите атрибут для улучшения:" << std::endl;
        std::cout << "1. Здоровье (+10)" << std::endl;
        std::cout << "2. Атака (+3)" << std::endl;
        std::cout << "3. Мана (+5)" << std::endl;
        
        // Используем getCharImmediate для моментального ввода без Enter
        char choice = uiManager.getCharImmediate();
        
        switch(choice) {
            case '1': // Увеличиваем здоровье
                player.boostHealth(10);
                lastPlayerChoice = 1;
                points--;
                break;
            case '2': // Увеличиваем атаку
                player.boostAttack(3);
                lastPlayerChoice = 2;
                points--;
                break;
            case '3': // Увеличиваем ману
                player.boostMana(5);
                lastPlayerChoice = 3;
                points--;
                break;
            default:
                // Если нажата не 1-3, ничего не делаем и продолжаем цикл
                break;
        }
        
        // Если игрок сделал выбор, противник тоже прокачивается
        if (lastPlayerChoice > 0) {
            // Вызываем распределение очков ИИ
            aiController.distributePointsBasedOnPlayerChoice(enemy, lastPlayerChoice);
            
            // Сбрасываем выбор игрока
            lastPlayerChoice = 0;
        }
    }
    
    // Показываем финальные характеристики
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
    
    // Переход в магазин (без ожидания нажатия клавиши)
    shopSystem.openShop(player, currentRound);
} 