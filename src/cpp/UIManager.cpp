#include "../h/UIManager.h"
#include "../h/BattleSystem.h"
#include <iostream>
#include <string>
#include <limits>
#include <iomanip>
#include <clocale>

#ifdef _WIN32
#include <conio.h>
#else
#include <termios.h>
#include <unistd.h>
#endif

UIManager::UIManager() {
    try {
        #ifndef _WIN32
        tcgetattr(STDIN_FILENO, &originalTermios);
        #endif
        
        setlocale(LC_ALL, "");
        
    } catch (const std::exception& e) {
        std::cerr << "Ошибка при инициализации UI: " << e.what() << std::endl;
    } catch (...) {
        std::cerr << "Неизвестная ошибка при инициализации UI" << std::endl;
    }
}

UIManager::~UIManager() {
#ifndef _WIN32
    tcsetattr(STDIN_FILENO, TCSANOW, &originalTermios);
#endif
}

char UIManager::getCharImmediate() {
#ifdef _WIN32
    return _getch();
#else
    setNonCanonicalMode();
    char ch = getchar();
    restoreTerminalMode();
    return ch;
#endif
}

#ifndef _WIN32
void UIManager::setNonCanonicalMode() {
    struct termios newTermios = originalTermios;
    
    newTermios.c_lflag &= ~(ICANON | ECHO);
    
    newTermios.c_cc[VMIN] = 1;
    newTermios.c_cc[VTIME] = 0;
    
    tcsetattr(STDIN_FILENO, TCSANOW, &newTermios);
}

void UIManager::restoreTerminalMode() {
    tcsetattr(STDIN_FILENO, TCSANOW, &originalTermios);
}
#endif

void UIManager::showScreen(const std::string& screenName) {
    if (screenName == "main") {

        #ifdef _WIN32
        system("cls");
        #else
        system("clear");
        #endif
        
        std::cout << "\n\n";

        std::cout << "          ⚔️  ДУЭЛЬ ПРОТИВ ДЭНА  ⚔️          \n\n";
        
        std::cout << "┌────────────────────────────────────────────┐\n";
        std::cout << "│                                            │\n";
        std::cout << "│           ▶  1. В БОЙ                     │\n";
        std::cout << "│                                            │\n";
        std::cout << "│           ▶  2. НАСТРОЙКИ                 │\n";
        std::cout << "│                                            │\n";
        std::cout << "│           ▶  3. ВЫХОД                     │\n";
        std::cout << "│                                            │\n";
        std::cout << "│      Автор: Яценко Юрий (Kan0Shuuya)      │\n";
        std::cout << "│                                            │\n";
        std::cout << "└────────────────────────────────────────────┘\n\n";
        
        std::cout << "     Выберите действие (введите номер): ";
    }
    else if (screenName == "settings") {
        #ifdef _WIN32
        system("cls");
        #else
        system("clear");
        #endif
        
        std::cout << "\n\n";
        std::cout << "          ⚙️  О ПРОГРАММЕ  ⚙️          \n\n";
        
        std::cout << "┌────────────────────────────────────────────┐\n";
        std::cout << "│                                            │\n";
        std::cout << "│    ▶  1. ОТКРЫТЬ GITHUB РЕПОЗИТОРИЙ       │\n";
        std::cout << "│                                            │\n";
        std::cout << "│    ▶  2. ИЗМЕНИТЬ ПАУЗЫ В ДИАЛОГАХ        │\n";
        std::cout << "│                                            │\n";
        std::cout << "│    ▶  3. ВЕРНУТЬСЯ В МЕНЮ                 │\n";
        std::cout << "│                                            │\n";
        std::cout << "│       Разработчик: Яценко Юрий             │\n";
        std::cout << "│                   (Kan0Shuuya)             │\n";
        std::cout << "│                                            │\n";
        std::cout << "│       GitHub: github.com/YatsenkoYura      │\n";
        std::cout << "│                                            │\n";
        std::cout << "└────────────────────────────────────────────┘\n\n";
        
        std::cout << "     Выберите действие (введите номер): ";
    }
} 

int UIManager::getPlayerChoice() {
    char ch = getCharImmediate();
    
    if (ch >= '1' && ch <= '9') {
        return ch - '0';
    }
    
    return -1;
}

int UIManager::processMainMenu() {
    showScreen("main");
    int choice = getPlayerChoice();
    while (choice < 1 || choice > 3) {
        std::cout << "\n     ⚠️ Пожалуйста, выберите пункт от 1 до 3!\n\n";

        showScreen("main");
        choice = getPlayerChoice();
    }
    
    return choice;
}

int UIManager::processSettingsMenu() {
    showScreen("settings");

    int choice = getPlayerChoice();

    while (choice < 1 || choice > 3) {
        std::cout << "\n     ⚠️ Пожалуйста, выберите пункт от 1 до 3!\n\n";

        showScreen("settings");
        choice = getPlayerChoice();
    }
    
    return choice;
}

void UIManager::displayBattleScreen(const Character& player, const Character& enemy) {
    clearScreen();
    
    std::cout << "\n\n";
    std::cout << "                БОЕВОЙ ЭКРАН                " << std::endl;
    std::cout << "------------------------------------------------" << std::endl << std::endl;
    
    std::cout << "ПРОТИВНИК: " << enemy.getName() << std::endl;
    std::cout << "HP: " << enemy.getHealth() << "  Атака: " << enemy.getAttack();
    if (enemy.doesHaveMana())
        std::cout << "  Мана: " << enemy.getMana();
    std::cout << std::endl;
    
    std::cout << "------------------------------------------------" << std::endl;
    
    std::cout << "ИГРОК: " << player.getName() << std::endl;
    std::cout << "HP: " << player.getHealth() << "/" << player.getMaxHealth();
    std::cout << "  Атака: " << player.getAttack();
    if (player.doesHaveMana())
        std::cout << "  Мана: " << player.getMana() << "/" << player.getMaxMana();
    std::cout << std::endl;
    
    std::cout << "------------------------------------------------" << std::endl << std::endl;
    
    std::cout << "Выберите действие:" << std::endl;
    std::cout << "1. Атаковать" << std::endl;
    std::cout << "2. Использовать магию" << std::endl;
    std::cout << "3. Использовать предмет" << std::endl;
    std::cout << "4. Выйти из игры" << std::endl << std::endl;
    
    std::cout << "Ваш выбор (1-4): ";
}

void UIManager::displayMagicOptions(const Character& player) {
    clearScreen();
    
    std::cout << "\n\n";
    std::cout << "             МАГИЧЕСКИЕ СПОСОБНОСТИ             " << std::endl;
    std::cout << "------------------------------------------------" << std::endl << std::endl;
    
    std::cout << "1. Огненный шар (атака, 15 маны)" << std::endl;
    std::cout << "2. Ледяной щит (защита, 10 маны)" << std::endl;
    std::cout << "3. Телепортация (лечение, 20 маны)" << std::endl;
    std::cout << "4. Вернуться назад" << std::endl << std::endl;
    
    std::cout << "Текущая мана: " << player.getMana() << std::endl << std::endl;
    std::cout << "Выберите магию (1-4): ";
}

void UIManager::displayInventory(const Character& player) {
    clearScreen();
    
    std::cout << "\n\n";
    std::cout << "                  ИНВЕНТАРЬ                  " << std::endl;
    std::cout << "------------------------------------------------" << std::endl << std::endl;
    
    const auto& inventory = player.getInventory();
    if (inventory.empty()) {
        std::cout << "            ИНВЕНТАРЬ ПУСТ            " << std::endl;
    } else {
        int index = 1;
        for (const auto& item : inventory) {
            std::cout << index << ". " << item.name << std::endl;
            index++;
        }
    }
    
    std::cout << std::endl;
    std::cout << "Выберите предмет (1-" << inventory.size() + 1 << "): ";
}

void UIManager::displayBattleResult(const std::string& message) {
    clearScreen();
    
    std::cout << "\n\n";
    std::cout << "              РЕЗУЛЬТАТ СРАЖЕНИЯ              " << std::endl;
    std::cout << "------------------------------------------------" << std::endl << std::endl;
    
    std::cout << message << std::endl << std::endl;
    
    #ifdef _WIN32
    Sleep(1000);
    #else
    usleep(1000000);
    #endif
}

void UIManager::clearScreen() {
    try {
        #ifdef _WIN32
        system("cls");
        #else
        system("clear");
        #endif
    } catch (const std::exception& e) {
        std::cerr << "Ошибка при очистке экрана: " << e.what() << std::endl;
    } catch (...) {
    }
}

void UIManager::displayBattleOptions() {
    clearScreen();
    
    std::cout << "\n\n";
    std::cout << "                БОЕВЫЕ ДЕЙСТВИЯ                " << std::endl;
    std::cout << "------------------------------------------------" << std::endl << std::endl;
    
    std::cout << "Выберите действие:" << std::endl;
    std::cout << "1. Атаковать оружием" << std::endl;
    std::cout << "2. Использовать магию" << std::endl;
    std::cout << "3. Использовать предмет" << std::endl;
    std::cout << "4. Информация о враге" << std::endl;
    std::cout << "5. Выйти из игры" << std::endl << std::endl;
    
    std::cout << "Ваш выбор (1-5): ";
}

void UIManager::displaySettingsMenu(BattleSystem& battleSystem) {
    clearScreen();
    std::cout << "=== НАСТРОЙКИ ИГРЫ ===" << std::endl << std::endl;
    std::cout << "1. Изменить время пауз в диалогах (текущее: " 
              << battleSystem.getDialoguePauseDuration() << " мс)" << std::endl;
    std::cout << "2. Вернуться в главное меню" << std::endl << std::endl;
    
    std::cout << "Ваш выбор (1-2): ";
    int choice = getPlayerChoice();
    
    switch (choice) {
        case 1:
            changeDialoguePauseSettings(battleSystem);
            break;
        case 2:
            return;
        default:
            std::cout << "Неверный выбор. Возврат в меню настроек." << std::endl;
            #ifdef _WIN32
            Sleep(1000);
            #else
            usleep(1000000);
            #endif
            displaySettingsMenu(battleSystem);
            break;
    }
}

void UIManager::changeDialoguePauseSettings(BattleSystem& battleSystem) {
    clearScreen();
    std::cout << "=== НАСТРОЙКА ВРЕМЕНИ ПАУЗ ===" << std::endl << std::endl;
    std::cout << "Текущее время паузы: " << battleSystem.getDialoguePauseDuration() << " мс" << std::endl;
    std::cout << "Выберите новое время паузы:" << std::endl;
    std::cout << "1. Очень быстро (1000 мс)" << std::endl;
    std::cout << "2. Быстро (1500 мс)" << std::endl;
    std::cout << "3. Средне (2000 мс)" << std::endl;
    std::cout << "4. Медленно (2500 мс)" << std::endl;
    std::cout << "5. Очень медленно (3000 мс)" << std::endl;
    std::cout << "6. Ввести своё значение" << std::endl;
    std::cout << "7. Вернуться в меню настроек" << std::endl << std::endl;
    
    std::cout << "Ваш выбор (1-7): ";
    int choice = getPlayerChoice();
    
    int newDuration = battleSystem.getDialoguePauseDuration();
    
    switch (choice) {
        case 1:
            newDuration = 1000;
            break;
        case 2:
            newDuration = 1500;
            break;
        case 3:
            newDuration = 2000;
            break;
        case 4:
            newDuration = 2500;
            break;
        case 5:
            newDuration = 3000;
            break;
        case 6:
            std::cout << "Введите новое время паузы в миллисекундах (500-5000): ";
            std::cin >> newDuration;
            if (newDuration < 500) newDuration = 500;
            if (newDuration > 5000) newDuration = 5000;
            break;
        case 7:
            displaySettingsMenu(battleSystem);
            return;
        default:
            std::cout << "Неверный выбор. Возврат в меню настроек." << std::endl;
            #ifdef _WIN32
            Sleep(1000);
            #else
            usleep(1000000);
            #endif
            displaySettingsMenu(battleSystem);
            return;
    }
    
    battleSystem.setDialoguePauseDuration(newDuration);
    std::cout << "Время паузы установлено на " << newDuration << " мс." << std::endl;
    
    #ifdef _WIN32
    Sleep(1500);
    #else
    usleep(1500000);
    #endif
    
    displaySettingsMenu(battleSystem);
}

void UIManager::displayMainMenu() {
    try {
        clearScreen();
        std::cout << "===========================================" << std::endl;
        std::cout << "          БИТВА С ДЭНОМ ПЕРЕБИТОВЫМ       " << std::endl;
        std::cout << "===========================================" << std::endl << std::endl;
        
        std::cout << "1. Новая игра" << std::endl;
        std::cout << "2. Настройки" << std::endl;
        std::cout << "3. Таблица лидеров" << std::endl;
        std::cout << "4. Выход" << std::endl << std::endl;
        
        std::cout << "Ваш выбор (1-4): ";
    } catch (const std::exception& e) {
        std::cerr << "Ошибка при отображении главного меню: " << e.what() << std::endl;
    } catch (...) {
        std::cerr << "Неизвестная ошибка при отображении главного меню" << std::endl;
    }
} 