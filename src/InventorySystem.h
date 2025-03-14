#ifndef INVENTORY_SYSTEM_H
#define INVENTORY_SYSTEM_H

#include "Character.h"
#include "UIManager.h"

class InventorySystem {
public:
    InventorySystem(UIManager& uiManager);
    
    // Основные методы для работы с инвентарем
    void addItem(Character& character, const Item& item);
    void removeItem(Character& character, int itemIndex);
    void useItem(Character& character, int itemIndex);
    
private:
    UIManager& uiManager;
};

#endif // INVENTORY_SYSTEM_H 