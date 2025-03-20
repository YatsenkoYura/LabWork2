#ifndef BUFF_SYSTEM_H
#define BUFF_SYSTEM_H

#include "Character.h"
#include "UIManager.h"
#include <string>
#include <vector>

// Структура для хранения активного эффекта
struct ActiveBuff {
    std::string name;
    int attackBonus;
    int defenseBonus;
    int healthRegen;
    int manaRegen;
    int duration;
    
    ActiveBuff(const std::string& n, int atk, int def, int hp, int mp, int dur)
        : name(n), attackBonus(atk), defenseBonus(def), 
          healthRegen(hp), manaRegen(mp), duration(dur) {}
};

class BuffSystem {
public:
    BuffSystem(UIManager& uiManager);
    
    // Добавление нового эффекта
    void addBuff(Character& character, const std::string& name, int attackBonus, int defenseBonus, 
                 int healthRegen, int manaRegen, int duration);
    
    // Обновление всех активных эффектов персонажа
    void updateBuffs(Character& character);
    
    // Удаление всех эффектов
    void clearBuffs();
    
    // Проверка наличия конкретного эффекта по имени
    bool hasBuff(const std::string& name) const;
    
private:
    UIManager& uiManager;
    std::vector<ActiveBuff> activeBuffs;
    
    // Вспомогательный метод для паузы
    void pauseForDialogue() const;
};

#endif 