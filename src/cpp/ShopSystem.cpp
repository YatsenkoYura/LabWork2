#include "../h/ShopSystem.h"
#include <iostream>
#include <cstdlib>
#include <vector>
#include <ctime>
#include <random>
#include <algorithm>
#include "../h/InventorySystem.h"

ShopSystem::ShopSystem(UIManager& uiManager) : uiManager(uiManager) {
    std::srand(static_cast<unsigned int>(std::time(nullptr)));
    
    baseShopItems = {
        { "Малое зелье здоровья", "Восстанавливает 20 HP", 50, 20, ShopItem::HEALTH_POTION },
        { "Среднее зелье здоровья", "Восстанавливает 40 HP", 100, 40, ShopItem::HEALTH_POTION },
        { "Большое зелье здоровья", "Восстанавливает 80 HP", 200, 80, ShopItem::HEALTH_POTION },
        
        { "Малое зелье маны", "Восстанавливает 15 MP", 60, 15, ShopItem::MANA_POTION },
        { "Среднее зелье маны", "Восстанавливает 30 MP", 120, 30, ShopItem::MANA_POTION },
        { "Большое зелье маны", "Восстанавливает 60 MP", 240, 60, ShopItem::MANA_POTION },
        
        { "Зелье силы", "Увеличивает атаку на 10 на 3 хода", 150, 10, ShopItem::BUFF_POTION },
        { "Эликсир защиты", "Увеличивает защиту на 15 на 3 хода", 180, 15, ShopItem::BUFF_POTION },
        { "Зелье регенерации", "Восстанавливает 10 HP каждый ход в течение 3 ходов", 200, 10, ShopItem::REGEN_POTION },
        { "Зелье маны", "Восстанавливает 5 MP каждый ход в течение 3 ходов", 220, 5, ShopItem::REGEN_POTION },
        { "Эликсир ярости", "Увеличивает атаку на 20, но снижает защиту на 10 на 2 хода", 180, 20, ShopItem::BUFF_POTION },
        { "Зелье берсерка", "Увеличивает атаку на 15 и даёт +10 к максимальному здоровью на 3 хода", 230, 15, ShopItem::BUFF_POTION },
        { "Зелье хаоса", "Случайно увеличивает одну характеристику на 25 на 2 хода", 200, 25, ShopItem::BUFF_POTION },
        { "Зелье медитации", "Полностью восстанавливает ману, но пропускаешь ход", 180, 100, ShopItem::MANA_POTION }
    };
}

void ShopSystem::openShop(Character& player, int currentRound) {
    generateShopItems(currentRound);
    
    int initialGold = 100 * currentRound;
    player.setGold(initialGold);
    
    bool exitShop = false;
    
    while (!exitShop) {
        uiManager.clearScreen();
        
        std::cout << "МАГАЗИН\n";
        std::cout << "-----------------------\n";
        
        std::cout << "Игрок: " << player.getName() << "  Золото: " << player.getGold() << "\n";
        std::cout << "-----------------------\n";
        
        displayShopItems(player, currentRound);
        
        std::cout << (availableItems.size() + 1) << ". Улучшить броню - " << calculateUpgradeCost(player.getDefense(), currentRound) << " золота\n";
        std::cout << (availableItems.size() + 2) << ". Улучшить меч - " << calculateUpgradeCost(player.getAttack(), currentRound) << " золота\n";
        std::cout << "-----------------------\n";
        std::cout << "Q. Выйти из магазина\n";
        std::cout << "Ваш выбор: ";
        
        char choice = uiManager.getCharImmediate();
        std::cout << choice << std::endl;
        
        #ifdef _WIN32
        Sleep(500);
        #else
        usleep(500000);
        #endif
        
        if (choice >= '1' && choice <= '9') {
            int index = choice - '0';
            
            if (index <= static_cast<int>(availableItems.size())) {
                buyItem(player, index - 1);
            }
            else if (index == availableItems.size() + 1) {
                upgradeArmor(player, currentRound);
            }
            else if (index == availableItems.size() + 2) {
                upgradeWeapon(player, currentRound);
            }
        } else if (choice == 'q' || choice == 'Q') {
            std::cout << "Вы покидаете магазин...\n";
            
            #ifdef _WIN32
            Sleep(1000);
            #else
            usleep(1000000);
            #endif
            
            exitShop = true;
        } else {
            std::cout << "Неверный выбор.\n";
            
            #ifdef _WIN32
            Sleep(1000);
            #else
            usleep(1000000);
            #endif
        }
    }
}

void ShopSystem::generateShopItems(int roundNumber) {
    availableItems.clear();
    
    std::vector<size_t> indices(baseShopItems.size());
    for (size_t i = 0; i < indices.size(); ++i) {
        indices[i] = i;
    }
    
    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(indices.begin(), indices.end(), g);
    
    for (int i = 0; i < 4 && i < static_cast<int>(indices.size()); ++i) {
        availableItems.push_back(baseShopItems[indices[i]]);
        
        availableItems.back().price = static_cast<int>(availableItems.back().price * (1.0 + (roundNumber - 1) * 0.2));
        availableItems.back().value = static_cast<int>(availableItems.back().value * (1.0 + (roundNumber - 1) * 0.1));
    }
}

void ShopSystem::displayShopItems(const Character& player, int roundNumber) {
    for (size_t i = 0; i < availableItems.size(); ++i) {
        const auto& item = availableItems[i];
        std::cout << (i + 1) << ". " << item.name << " - " << item.price << " золота\n";
    }
}

void ShopSystem::buyItem(Character& player, int itemIndex) {
    if (itemIndex < 0 || itemIndex >= static_cast<int>(availableItems.size())) {
        std::cout << "Неверный индекс предмета!" << std::endl;
        return;
    }
    
    const auto& item = availableItems[itemIndex];
    
    if (player.getGold() < item.price) {
        std::cout << "Недостаточно золота!" << std::endl;
        
        #ifdef _WIN32
        Sleep(1500);
        #else
        usleep(1500000);
        #endif
        
        return;
    }
    
    applyItemEffect(player, item);
    
    player.setGold(player.getGold() - item.price);
    
    availableItems.erase(availableItems.begin() + itemIndex);
    
    #ifdef _WIN32
    Sleep(1500);
    #else
    usleep(1500000);
    #endif
}

void ShopSystem::applyItemEffect(Character& player, const ShopItem& item) {
    std::string description;
    
    InventorySystem inventorySystem(uiManager);
    
    switch (item.type) {
        case ShopItem::HEALTH_POTION:
            description = "Восстанавливает " + std::to_string(item.value) + " HP";
            inventorySystem.addItem(player, Item(item.name, description));
            break;
            
        case ShopItem::MANA_POTION:
            if (player.doesHaveMana()) {
                description = "Восстанавливает " + std::to_string(item.value) + " MP";
                inventorySystem.addItem(player, Item(item.name, description));
            } else {
                std::cout << "У вас нет маны! Вы не можете использовать зелья маны." << std::endl;
                player.setGold(player.getGold() + item.price);
                return;
            }
            break;
            
        case ShopItem::BUFF_POTION:
            inventorySystem.addItem(player, Item(item.name, item.description));
            break;
            
        case ShopItem::REGEN_POTION:
            inventorySystem.addItem(player, Item(item.name, item.description));
            break;
            
        default:
            std::cout << "Неизвестный тип предмета!" << std::endl;
            break;
    }
}

int ShopSystem::calculateUpgradeCost(int currentStat, int roundNumber) {
    return 100 + currentStat * 10 + roundNumber * 50;
}

void ShopSystem::upgradeArmor(Character& player, int roundNumber) {
    int cost = calculateUpgradeCost(player.getDefense(), roundNumber);
    
    if (player.getGold() < cost) {
        std::cout << "Недостаточно золота для улучшения брони!" << std::endl;
        
        #ifdef _WIN32
        Sleep(1500);
        #else
        usleep(1500000);
        #endif
        
        return;
    }
    
    player.boostDefense(5);
    
    player.setGold(player.getGold() - cost);
    
    std::cout << "Броня улучшена! Новое значение защиты: " << player.getDefense() << std::endl;
    
    #ifdef _WIN32
    Sleep(1500);
    #else
    usleep(1500000);
    #endif
}

void ShopSystem::upgradeWeapon(Character& player, int roundNumber) {
    int cost = calculateUpgradeCost(player.getAttack(), roundNumber);
    
    if (player.getGold() < cost) {
        std::cout << "Недостаточно золота для улучшения меча!" << std::endl;
        
        #ifdef _WIN32
        Sleep(1500);
        #else
        usleep(1500000);
        #endif
        
        return;
    }
    
    player.boostAttack(3);
    
    player.setGold(player.getGold() - cost);
    
    std::cout << "Меч улучшен! Новое значение атаки: " << player.getAttack() << std::endl;
    
    #ifdef _WIN32
    Sleep(1500);
    #else
    usleep(1500000);
    #endif
} 