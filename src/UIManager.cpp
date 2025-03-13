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
        std::cout << "\033[2J\033[1;1H";
        
        std::cout << "╔══════════════════════════════════════════════════╗" << std::endl;
        std::cout << "║                                                  ║" << std::endl;
        std::cout << "║             ДУЭЛЬ ПРОТИВ ДЭНЫ                   ║" << std::endl;
        std::cout << "║                                                  ║" << std::endl;
        std::cout << "╠══════════════════════════════════════════════════╣" << std::endl;
        std::cout << "║                                                  ║" << std::endl;
        std::cout << "║              1. В бой                           ║" << std::endl;
        std::cout << "║              2. Магазин                         ║" << std::endl;
        std::cout << "║              3. Выход                           ║" << std::endl;
        std::cout << "║                                                  ║" << std::endl;
        std::cout << "╠══════════════════════════════════════════════════╣" << std::endl;
        std::cout << "║  Введите номер выбора:                           ║" << std::endl;
        std::cout << "╚══════════════════════════════════════════════════╝" << std::endl;
    }
} 

int UIManager::getPlayerChoice() {
    int choice = 0;
    std::cout << " > ";
    std::cin >> choice;
    
    if (std::cin.fail()) {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        return -1;
    }
    
    return choice;
} 