#ifndef POTION_SYSTEM_H
#define POTION_SYSTEM_H

#include "Character.h"
#include "UIManager.h"
#include "BuffSystem.h"

// Перечисление типов зелий
enum class PotionType {
    HEALTH_POTION,
    MANA_POTION,
    BUFF_POTION,
    REGEN_POTION,
    SPECIAL_POTION
};

class PotionSystem {
public:
    PotionSystem(UIManager& uiManager, BuffSystem& buffSystem);
    
    // Применение зелья к персонажу
    bool usePotion(Character& character, const Item& item);
    
    // Вспомогательные методы для разных типов зелий
    bool useHealthPotion(Character& character, const Item& item);
    bool useManaPotion(Character& character, const Item& item);
    bool useBuffPotion(Character& character, const Item& item);
    bool useRegenPotion(Character& character, const Item& item);
    bool useSpecialPotion(Character& character, const Item& item);
    
private:
    UIManager& uiManager;
    BuffSystem& buffSystem;
    
    // Получить числовое значение из описания предмета
    int extractValueFromDescription(const std::string& description, const std::string& suffix);
    
    // Вспомогательный метод для паузы
    void pauseForDialogue() const;
};

#endif 