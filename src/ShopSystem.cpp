#include "ShopSystem.h"
#include <iostream>
#include <cstdlib>
#include <limits>

ShopSystem::ShopSystem(UIManager& uiManager) : uiManager(uiManager) {
}

void ShopSystem::openShop(Character& player, int currentRound) {
    // Очищаем экран и выводим заголовок магазина
    uiManager.clearScreen();
    int width = 80;
    std::string line(width, '-');
    
    std::cout << "+" << line << "+" << std::endl;
    std::string title = "МАГАЗИН";
    int padding = (width - title.size()) / 2;
    std::cout << "|" << std::string(padding, ' ') << title 
              << std::string(width - title.size() - padding, ' ') << "|" << std::endl;
    std::cout << "+" << line << "+" << std::endl << std::endl;
    
    std::cout << "Добро пожаловать в магазин! Пока здесь нет товаров.\n\n";
    std::cout << "1. Идти в бой на следующий раунд\n";
    std::cout << "2. Выйти из игры\n";
    std::cout << "\nВыберите действие (1-2): ";
    
    int choice;
    std::cin >> choice;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    if(choice == 2) {
        std::cout << "\nСпасибо за игру! До встречи!\n";
        exit(0);
    }
    // Если выбран вариант 1, магазин закрывается, игра продолжается.
}

void ShopSystem::displayShopItems(const Character& player, int roundNumber) {
    // Временная заглушка
    // Здесь будет отображение доступных предметов и улучшений
}

void ShopSystem::buyItem(Character& player, int itemIndex) {
    // Временная заглушка
    // Здесь будет логика покупки предмета
}

void ShopSystem::upgradeCharacterStat(Character& player, int statIndex) {
    // Временная заглушка
    // Здесь будет логика улучшения характеристик персонажа
} 