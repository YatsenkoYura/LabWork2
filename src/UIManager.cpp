#include "UIManager.h"
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
#ifndef _WIN32
    // Сохраняем исходные настройки терминала
    tcgetattr(STDIN_FILENO, &originalTermios);
#endif
    setlocale(LC_ALL, "");
}

UIManager::~UIManager() {
#ifndef _WIN32
    // Восстанавливаем исходные настройки терминала
    tcsetattr(STDIN_FILENO, TCSANOW, &originalTermios);
#endif
}

// Метод для получения одного символа без нажатия Enter
char UIManager::getCharImmediate() {
#ifdef _WIN32
    // В Windows используем _getch()
    return _getch();
#else
    // В Unix-подобных системах используем неканонический режим
    setNonCanonicalMode();
    char ch = getchar();
    restoreTerminalMode();
    return ch;
#endif
}

#ifndef _WIN32
// Установка неканонического режима терминала
void UIManager::setNonCanonicalMode() {
    struct termios newTermios = originalTermios;
    
    // Отключаем канонический режим и эхо
    newTermios.c_lflag &= ~(ICANON | ECHO);
    
    // Устанавливаем минимальное количество символов для чтения = 1
    newTermios.c_cc[VMIN] = 1;
    newTermios.c_cc[VTIME] = 0;
    
    // Применяем новые настройки
    tcsetattr(STDIN_FILENO, TCSANOW, &newTermios);
}

// Восстановление исходного режима терминала
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

        std::cout << "          ⚔️  ДУЭЛЬ ПРОТИВ ДЭНЫ  ⚔️          \n\n";
        
        std::cout << "┌────────────────────────────────────────────┐\n";
        std::cout << "│                                            │\n";
        std::cout << "│           ▶  1. В БОЙ                     │\n";
        std::cout << "│                                            │\n";
        std::cout << "│           ▶  2. НАСТРОЙКИ                 │\n";
        std::cout << "│                                            │\n";
        std::cout << "│           ▶  3. ВЫХОД                     │\n";
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
        std::cout << "│           ▶  1. ОТКРЫТЬ GITHUB            │\n";
        std::cout << "│                                            │\n";
        std::cout << "│           ▶  2. ВЕРНУТЬСЯ                 │\n";
        std::cout << "│                                            │\n";
        std::cout << "│                                            │\n";
        std::cout << "│       Разработчик: Kan0Shuya               │\n";
        std::cout << "│       (Яценко Юрий)                        │\n";
        std::cout << "│                                            │\n";
        std::cout << "└────────────────────────────────────────────┘\n\n";
        
        std::cout << "     Выберите действие (введите номер): ";
    }
} 

int UIManager::getPlayerChoice() {
    char ch = getCharImmediate();
    
    // Преобразуем символ в число, если это цифра
    if (ch >= '1' && ch <= '9') {
        return ch - '0';
    }
    
    return -1; // Некорректный ввод
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

    while (choice < 1 || choice > 2) {
        std::cout << "\n     ⚠️ Пожалуйста, выберите пункт от 1 до 2!\n\n";

        showScreen("settings");
        choice = getPlayerChoice();
    }
    
    return choice;
}

void UIManager::displayBattleScreen(const Character& player, const Character& enemy) {
    clearScreen();
    const int innerWidth = 44;

    // Функция для центрирования текста, если его размер меньше innerWidth
    auto centerText = [innerWidth](const std::string &s) -> std::string {
        if ((int)s.size() >= innerWidth) return s;
        int totalPad = innerWidth - s.size();
        int padLeft = totalPad / 2;
        int padRight = totalPad - padLeft;
        return std::string(padLeft, ' ') + s + std::string(padRight, ' ');
    };

    // Функция для левостороннего выравнивания
    auto leftAlign = [innerWidth](const std::string &s) -> std::string {
        if ((int)s.size() >= innerWidth) return s;
        return s + std::string(innerWidth - s.size(), ' ');
    };

    std::cout << "┌────────────────────────────────────────────┐" << std::endl;
    {
        std::string title = "БОЕВОЙ ЭКРАН";
        std::cout << "│" << centerText(title) << "│" << std::endl;
    }
    std::cout << "├────────────────────────────────────────────┤" << std::endl;
    {
        std::string enemyHeader = "ПРОТИВНИК: " + enemy.getName();
        std::cout << "│" << leftAlign(enemyHeader) << "│" << std::endl;
    }
    {
        std::string enemyStats;
        if (enemy.doesHaveMana())
            enemyStats = "HP: " + std::to_string(enemy.getHealth()) + "  Броня: " + std::to_string(enemy.getDefense()) + "  Мана: " + std::to_string(enemy.getMana());
        else
            enemyStats = "HP: " + std::to_string(enemy.getHealth()) + "  Броня: " + std::to_string(enemy.getDefense());
        std::cout << "│" << leftAlign(enemyStats) << "│" << std::endl;
    }
    std::cout << "├────────────────────────────────────────────┤" << std::endl;
    {
        std::string playerHeader = "ИГРОК: " + player.getName();
        std::cout << "│" << leftAlign(playerHeader) << "│" << std::endl;
    }
    {
        std::string playerStats = "HP: " + std::to_string(player.getHealth()) + "  Броня: " + std::to_string(player.getDefense()) + "  Мана: " + std::to_string(player.getMana());
        std::cout << "│" << leftAlign(playerStats) << "│" << std::endl;
    }
    std::cout << "└────────────────────────────────────────────┘" << std::endl << std::endl;
    
    std::cout << "Доступные действия:" << std::endl;
    std::cout << "1. Атаковать оружием" << std::endl;
    std::cout << "2. Использовать магию" << std::endl;
    std::cout << "3. Использовать предмет" << std::endl;
    std::cout << "4. Информация о противнике" << std::endl;
    std::cout << "5. Выйти из игры" << std::endl << std::endl;
    std::cout << "Выберите действие (1-5): ";
}

void UIManager::displayMagicOptions(const Character& player) {
    clearScreen();
    const int innerWidth = 44;
    
    auto centerText = [innerWidth](const std::string &s) -> std::string {
        if ((int)s.size() >= innerWidth) return s;
        int totalPad = innerWidth - s.size();
        int padLeft = totalPad / 2;
        int padRight = totalPad - padLeft;
        return std::string(padLeft, ' ') + s + std::string(padRight, ' ');
    };
    auto leftAlign = [innerWidth](const std::string &s) -> std::string {
        if ((int)s.size() >= innerWidth) return s;
        return s + std::string(innerWidth - s.size(), ' ');
    };
    
    std::cout << "┌────────────────────────────────────────────┐" << std::endl;
    {
        std::string title = "МАГИЧЕСКИЕ СПОСОБНОСТИ";
        std::cout << "│" << centerText(title) << "│" << std::endl;
    }
    std::cout << "├────────────────────────────────────────────┤" << std::endl;
    {
        std::string option1 = "1. Огненный шар (атака, 10 маны)";
        std::string option2 = "2. Ледяной щит (защита, 15 маны)";
        std::string option3 = "3. Телепортация (уклонение, 20 маны)";
        std::string option4 = "4. Вернуться назад";
        std::cout << "│" << leftAlign(option1) << "│" << std::endl;
        std::cout << "│" << leftAlign(option2) << "│" << std::endl;
        std::cout << "│" << leftAlign(option3) << "│" << std::endl;
        std::cout << "│" << leftAlign(option4) << "│" << std::endl;
    }
    std::cout << "└────────────────────────────────────────────┘" << std::endl;
    std::cout << "Текущая мана: " << player.getMana() << std::endl;
    std::cout << "Выберите магию (1-4): ";
}

void UIManager::displayInventory(const Character& player) {
    clearScreen();
    const int innerWidth = 44;
    
    auto centerText = [innerWidth](const std::string &s) -> std::string {
        if ((int)s.size() >= innerWidth) return s;
        int totalPad = innerWidth - s.size();
        int padLeft = totalPad / 2;
        int padRight = totalPad - padLeft;
        return std::string(padLeft, ' ') + s + std::string(padRight, ' ');
    };
    auto leftAlign = [innerWidth](const std::string &s) -> std::string {
        if ((int)s.size() >= innerWidth) return s;
        return s + std::string(innerWidth - s.size(), ' ');
    };

    std::cout << "┌────────────────────────────────────────────┐" << std::endl;
    {
        std::string title = "ИНВЕНТАРЬ";
        std::cout << "│" << centerText(title) << "│" << std::endl;
    }
    std::cout << "├────────────────────────────────────────────┤" << std::endl;
    const auto& inventory = player.getInventory();
    if (inventory.empty()) {
        std::string emptyMsg = "ИНВЕНТАРЬ ПУСТ";
        std::cout << "│" << centerText(emptyMsg) << "│" << std::endl;
    } else {
        int index = 1;
        for (const auto& item : inventory) {
            std::string lineItem = std::to_string(index) + ". " + item.getName();
            std::cout << "│" << leftAlign(lineItem) << "│" << std::endl;
            index++;
        }
    }
    std::cout << "└────────────────────────────────────────────┘" << std::endl;
    std::cout << "Выберите предмет (1-" << inventory.size() + 1 << "): ";
}

void UIManager::displayBattleResult(const std::string& message) {
    clearScreen();
    const int innerWidth = 44;
    
    auto centerText = [innerWidth](const std::string &s) -> std::string {
        if ((int)s.size() >= innerWidth) return s;
        int totalPad = innerWidth - s.size();
        int padLeft = totalPad / 2;
        int padRight = totalPad - padLeft;
        return std::string(padLeft, ' ') + s + std::string(padRight, ' ');
    };
    auto leftAlign = [innerWidth](const std::string &s) -> std::string {
        if ((int)s.size() >= innerWidth) return s;
        return s + std::string(innerWidth - s.size(), ' ');
    };

    std::cout << "┌────────────────────────────────────────────┐" << std::endl;
    {
        std::string title = "РЕЗУЛЬТАТ СРАЖЕНИЯ";
        std::cout << "│" << centerText(title) << "│" << std::endl;
    }
    std::cout << "├────────────────────────────────────────────┤" << std::endl;
    {
        std::cout << "│ " << leftAlign(message) << "│" << std::endl;
    }
    std::cout << "└────────────────────────────────────────────┘" << std::endl << std::endl;
    std::cout << "Нажмите Enter для продолжения...";
    std::cin.ignore();
    std::cin.get();
}

void UIManager::clearScreen() {
    #ifdef _WIN32
    system("cls");
    #else
    system("clear");
    #endif
} 