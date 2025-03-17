#include "ShopSystem.h"
#include <iostream>
#include <cstdlib>
#include <limits>
#include <vector>
#include <iomanip>
#include <ctime>
#include <random>
#include <algorithm>

ShopSystem::ShopSystem(UIManager& uiManager) : uiManager(uiManager) {
    // Инициализируем генератор случайных чисел
    std::srand(static_cast<unsigned int>(std::time(nullptr)));
    
    // Предустановленные товары для магазина
    baseShopItems = {
        // Зелья здоровья
        { "Малое зелье здоровья", "Восстанавливает 20 HP", 50, 20, ShopItem::HEALTH_POTION },
        { "Среднее зелье здоровья", "Восстанавливает 40 HP", 100, 40, ShopItem::HEALTH_POTION },
        { "Большое зелье здоровья", "Восстанавливает 80 HP", 200, 80, ShopItem::HEALTH_POTION },
        
        // Зелья маны
        { "Малое зелье маны", "Восстанавливает 15 MP", 60, 15, ShopItem::MANA_POTION },
        { "Среднее зелье маны", "Восстанавливает 30 MP", 120, 30, ShopItem::MANA_POTION },
        { "Большое зелье маны", "Восстанавливает 60 MP", 240, 60, ShopItem::MANA_POTION },
        
        // Новые зелья с долгосрочными эффектами
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
    // Генерируем случайный набор товаров для текущего посещения магазина
    generateShopItems(currentRound);
    
    // Начальное состояние игрока
    int initialGold = 100 * currentRound;
    player.setGold(initialGold); // Временно устанавливаем начальное золото
    
    bool exitShop = false;
    
    while (!exitShop) {
        // Очищаем экран
        uiManager.clearScreen();
        
        // Простой заголовок магазина
        std::cout << "МАГАЗИН\n";
        std::cout << "-----------------------\n";
        
        // Информация об игроке
        std::cout << "Игрок: " << player.getName() << "  Золото: " << player.getGold() << "\n";
        std::cout << "-----------------------\n";
        
        // Отображаем доступные товары
        displayShopItems(player, currentRound);
        
        // Отображаем опции для улучшения
        std::cout << (availableItems.size() + 1) << ". Улучшить броню - " << calculateUpgradeCost(player.getDefense(), currentRound) << " золота\n";
        std::cout << (availableItems.size() + 2) << ". Улучшить меч - " << calculateUpgradeCost(player.getAttack(), currentRound) << " золота\n";
        std::cout << "-----------------------\n";
        std::cout << "Q. Выйти из магазина\n";
        std::cout << "Ваш выбор: ";
        
        char choice = uiManager.getCharImmediate();
        std::cout << choice << std::endl;
        
        // Небольшая задержка для видимости
        #ifdef _WIN32
        Sleep(500);
        #else
        usleep(500000);
        #endif
        
        if (choice >= '1' && choice <= '9') {
            int index = choice - '0';
            
            if (index <= static_cast<int>(availableItems.size())) {
                // Покупка зелья
                buyItem(player, index - 1);
            }
            else if (index == availableItems.size() + 1) {
                // Улучшить броню
                upgradeArmor(player, currentRound);
            }
            else if (index == availableItems.size() + 2) {
                // Улучшить меч
                upgradeWeapon(player, currentRound);
            }
        } else if (choice == 'q' || choice == 'Q') {
            // Выход из магазина и продолжение игры
            std::cout << "Вы покидаете магазин...\n";
            
            // Небольшая задержка перед выходом
            #ifdef _WIN32
            Sleep(1000);
            #else
            usleep(1000000);
            #endif
            
            exitShop = true;
        } else {
            std::cout << "Неверный выбор.\n";
            
            // Небольшая задержка для видимости сообщения
            #ifdef _WIN32
            Sleep(1000);
            #else
            usleep(1000000);
            #endif
        }
    }
}

void ShopSystem::generateShopItems(int roundNumber) {
    // Очищаем предыдущий список товаров
    availableItems.clear();
    
    // Выбираем 4 случайных зелья
    std::vector<size_t> indices(baseShopItems.size());
    for (size_t i = 0; i < indices.size(); ++i) {
        indices[i] = i;
    }
    
    // Перемешиваем индексы
    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(indices.begin(), indices.end(), g);
    
    // Выбираем первые 4 зелья
    for (int i = 0; i < 4 && i < static_cast<int>(indices.size()); ++i) {
        availableItems.push_back(baseShopItems[indices[i]]);
        
        // С каждым раундом цены и эффективность растут
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
    
    // Проверяем, хватает ли у игрока золота
    if (player.getGold() < item.price) {
        std::cout << "Недостаточно золота!" << std::endl;
        
        // Небольшая задержка для видимости сообщения
        #ifdef _WIN32
        Sleep(1500);
        #else
        usleep(1500000);
        #endif
        
        return;
    }
    
    // Применяем эффект предмета
    applyItemEffect(player, item);
    
    // Вычитаем стоимость предмета
    player.setGold(player.getGold() - item.price);
    
    // Удаляем купленный предмет из списка доступных
    availableItems.erase(availableItems.begin() + itemIndex);
    
    // Небольшая задержка для видимости сообщения
    #ifdef _WIN32
    Sleep(1500);
    #else
    usleep(1500000);
    #endif
}

void ShopSystem::applyItemEffect(Character& player, const ShopItem& item) {
    std::string description;
    
    switch (item.type) {
        case ShopItem::HEALTH_POTION:
            // Добавляем зелье здоровья в инвентарь игрока
            description = "Восстанавливает " + std::to_string(item.value) + " HP";
            player.addToInventory(Item(item.name, description));
            std::cout << item.name << " добавлено в инвентарь!" << std::endl;
            break;
            
        case ShopItem::MANA_POTION:
            if (player.doesHaveMana()) {
                // Добавляем зелье маны в инвентарь игрока
                description = "Восстанавливает " + std::to_string(item.value) + " MP";
                player.addToInventory(Item(item.name, description));
                std::cout << item.name << " добавлено в инвентарь!" << std::endl;
            } else {
                std::cout << "У вас нет маны! Вы не можете использовать зелья маны." << std::endl;
                player.setGold(player.getGold() + item.price); // Возвращаем деньги
                return;
            }
            break;
            
        case ShopItem::BUFF_POTION:
            // Добавляем усиливающее зелье в инвентарь
            player.addToInventory(Item(item.name, item.description));
            std::cout << item.name << " добавлено в инвентарь!" << std::endl;
            break;
            
        case ShopItem::REGEN_POTION:
            // Добавляем зелье регенерации в инвентарь
            player.addToInventory(Item(item.name, item.description));
            std::cout << item.name << " добавлено в инвентарь!" << std::endl;
            break;
            
        default:
            std::cout << "Неизвестный тип предмета!" << std::endl;
            break;
    }
}

int ShopSystem::calculateUpgradeCost(int currentStat, int roundNumber) {
    // Базовая стоимость улучшения + стоимость на основе текущей характеристики и раунда
    return 100 + currentStat * 10 + roundNumber * 50;
}

void ShopSystem::upgradeArmor(Character& player, int roundNumber) {
    int cost = calculateUpgradeCost(player.getDefense(), roundNumber);
    
    if (player.getGold() < cost) {
        std::cout << "Недостаточно золота для улучшения брони!" << std::endl;
        
        // Небольшая задержка для видимости сообщения
        #ifdef _WIN32
        Sleep(1500);
        #else
        usleep(1500000);
        #endif
        
        return;
    }
    
    // Улучшаем броню
    player.boostDefense(5);
    
    // Вычитаем стоимость
    player.setGold(player.getGold() - cost);
    
    std::cout << "Броня улучшена! Новое значение защиты: " << player.getDefense() << std::endl;
    
    // Небольшая задержка для видимости сообщения
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
        
        // Небольшая задержка для видимости сообщения
        #ifdef _WIN32
        Sleep(1500);
        #else
        usleep(1500000);
        #endif
        
        return;
    }
    
    // Улучшаем меч
    player.boostAttack(3);
    
    // Вычитаем стоимость
    player.setGold(player.getGold() - cost);
    
    std::cout << "Меч улучшен! Новое значение атаки: " << player.getAttack() << std::endl;
    
    // Небольшая задержка для видимости сообщения
    #ifdef _WIN32
    Sleep(1500);
    #else
    usleep(1500000);
    #endif
} 