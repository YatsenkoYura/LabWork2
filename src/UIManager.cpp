#include "UIManager.h"
#include <iostream>
#include <string>
#include <limits>

UIManager::UIManager() {

}

UIManager::~UIManager() {

}

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
    int choice = 0;
    std::cin >> choice;
    
    if (std::cin.fail()) {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "\n     ❌ Некорректный ввод! Попробуйте снова.\n\n";
        return -1;
    }
    
    return choice;
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