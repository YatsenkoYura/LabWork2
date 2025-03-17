#include "InventorySystem.h"
#include <iostream>

InventorySystem::InventorySystem(UIManager& uiManager) : uiManager(uiManager) {
}

void InventorySystem::addItem(Character& character, const Item& item) {
    // Временная заглушка
    // Здесь будет логика добавления предмета в инвентарь
}

void InventorySystem::removeItem(Character& character, int itemIndex) {
    // Временная заглушка
    // Здесь будет логика удаления предмета из инвентаря
}

void InventorySystem::useItem(Character& character, int itemIndex) {
    // Временная заглушка
    // Здесь будет логика использования предмета из инвентаря
} 