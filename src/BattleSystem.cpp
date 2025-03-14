#include "BattleSystem.h"
#include <iostream>
#include <cstdlib>

BattleSystem::BattleSystem(UIManager& uiManager) : uiManager(uiManager) {
}

bool BattleSystem::startBattle(Character& player, Character& enemy) {
    bool playerWon = false;
    bool battleInProgress = true;
    
    while (battleInProgress && player.isAlive() && enemy.isAlive()) {
        // Отображаем боевой экран
        uiManager.displayBattleScreen(player, enemy);
        
        // Ход игрока
        processPlayerTurn(player, enemy);
        
        // Проверяем, жив ли враг после хода игрока
        if (!enemy.isAlive()) {
            playerWon = true;
            battleInProgress = false;
            continue;
        }
        
        // Ход противника обрабатывается в GameEngine через AIController
    }
    
    return playerWon;
}

void BattleSystem::processPlayerTurn(Character& player, Character& enemy) {
    int choice = uiManager.getPlayerChoice();
    
    switch (choice) {
        case 1: // Атака оружием
            attackEnemy(player, enemy);
            break;
        case 2: // Использование магии
            useMagic(player, enemy);
            break;
        case 3: // Использование предмета
            useItem(player, enemy);
            break;
        case 4: // Информация о противнике
            showEnemyInfo(enemy);
            // После показа информации снова отображаем боевой экран
            uiManager.displayBattleScreen(player, enemy);
            break;
        case 5: // Выход из игры
            exitGame();
            break;
        default:
            // При некорректном вводе просто обновляем экран
            uiManager.displayBattleScreen(player, enemy);
            break;
    }
}

void BattleSystem::attackEnemy(Character& player, Character& enemy) {
    int damage = calculateDamage(player, enemy);
    enemy.takeDamage(damage);
}

void BattleSystem::useMagic(Character& player, Character& enemy) {
    uiManager.displayMagicOptions(player);
    int choice = uiManager.getPlayerChoice();
    
    switch(choice) {
        case 1: {
            int baseDamage = calculateDamage(player, enemy);
            int addedDamage = 5;
            int totalDamage = baseDamage + addedDamage;
            enemy.takeDamage(totalDamage);
            break;
        }
        case 2: {
            player.boostDefense(5);
            break;
        }
        case 3: {
            // Телепортация, эффект пока не реализован
            break;
        }
        case 4: {
            // Вернуться назад, ничего не делаем
            break;
        }
        default:
            break;
    }
}

void BattleSystem::useItem(Character& player, Character& enemy) {
    uiManager.displayInventory(player);
    int choice = uiManager.getPlayerChoice();
    
    // Обработка выбора предмета (реализация TBD)
}

void BattleSystem::showEnemyInfo(const Character& enemy) {
    // Создаем информационное окно о противнике
    clearScreen();
    int width = 80;
    std::string line(width, '-');
    
    std::cout << "+" << line << "+" << std::endl;
    
    std::string title = "ИНФОРМАЦИЯ О ПРОТИВНИКЕ";
    int padding = (width - title.size()) / 2;
    std::cout << "|" << std::string(padding, ' ') << title << std::string(width - title.size() - padding, ' ') << "|" << std::endl;
    
    std::cout << "+" << line << "+" << std::endl;
    
    std::cout << "| Имя: " << enemy.getName() << std::string(width - 7 - enemy.getName().size(), ' ') << "|" << std::endl;
    std::cout << "| Здоровье: " << enemy.getHealth() << std::string(width - 12 - std::to_string(enemy.getHealth()).size(), ' ') << "|" << std::endl;
    std::cout << "| Защита: " << enemy.getDefense() << std::string(width - 10 - std::to_string(enemy.getDefense()).size(), ' ') << "|" << std::endl;
    std::cout << "| Мана: " << enemy.getMana() << std::string(width - 8 - std::to_string(enemy.getMana()).size(), ' ') << "|" << std::endl;
    
    std::cout << "+" << line << "+" << std::endl;
    
    std::cout << "\nНажмите Enter для возврата...";
    std::cin.ignore();
    std::cin.get();
}

int BattleSystem::calculateDamage(const Character& attacker, const Character& defender) {
    // Простая формула расчета урона
    int baseDamage = 10; // Базовый урон
    int damage = baseDamage - defender.getDefense() / 5;
    
    // Минимальный урон 1
    if (damage < 1) damage = 1;
    
    return damage;
}

void BattleSystem::exitGame() {
    clearScreen();
    std::cout << "\n\n";
    std::cout << "Вы уверены, что хотите выйти из игры? (y/n): ";
    char choice;
    std::cin >> choice;
    
    if (choice == 'y' || choice == 'Y') {
        std::cout << "\nСпасибо за игру! До встречи!\n";
        exit(0); // Завершаем программу
    }
    // Если пользователь не подтвердил выход, возвращаемся к бою
}

void BattleSystem::clearScreen() {
    #ifdef _WIN32
    system("cls");
    #else
    system("clear");
    #endif
} 