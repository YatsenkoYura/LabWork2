#include "UIManager.h"
#include <iostream>

void UIManager::showScreen(const std::string& screenType) {
    if (screenType == "start") {
        std::cout << "Добро пожаловать в Дуэль с Деном!" << std::endl;
        std::cout << "1. Начать игру" << std::endl;
        std::cout << "2. Настройки" << std::endl;
        std::cout << "3. Выход" << std::endl;
    } else if (screenType == "main") {
        
    }
} 