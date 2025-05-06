#include "../h/InventorySystem.h"
#include "../h/PotionSystem.h"
#include "../h/BuffSystem.h"
#include <iostream>

InventorySystem::InventorySystem(UIManager& uiManager) : uiManager(uiManager) {
}

void InventorySystem::addItem(Character& character, const Item& item) {
    character.addToInventory(item);
    std::cout << "Предмет \"" << item.name << "\" добавлен в инвентарь." << std::endl;
}

void InventorySystem::removeItem(Character& character, int itemIndex) {
    const std::vector<Item>& inventory = character.getInventory();
    
    if (itemIndex >= 0 && itemIndex < static_cast<int>(inventory.size())) {
        std::string itemName = inventory[itemIndex].name;
        character.removeFromInventory(itemIndex);
        std::cout << "Предмет \"" << itemName << "\" удален из инвентаря." << std::endl;
    } else {
        std::cout << "Неверный индекс предмета!" << std::endl;
    }
}

void InventorySystem::useItem(Character& character, int itemIndex) {
    const std::vector<Item>& inventory = character.getInventory();
    
    if (itemIndex < 0 || itemIndex >= static_cast<int>(inventory.size())) {
        std::cout << "Неверный индекс предмета!" << std::endl;
        return;
    }
    
    const Item& item = inventory[itemIndex];
    
    // Создаем временные системы для обработки эффектов предметов
    BuffSystem buffSystem(uiManager);
    PotionSystem potionSystem(uiManager, buffSystem);
    
    // Используем предмет через PotionSystem
    bool potionUsed = potionSystem.usePotion(character, item);
    
    if (potionUsed) {
        // Удаляем предмет из инвентаря
        removeItem(character, itemIndex);
    }
} 