#ifndef AI_CONTROLLER_H
#define AI_CONTROLLER_H

#include "Character.h"
#include "UIManager.h"

class AIController {
public:
    AIController(UIManager& uiManager);
    
    // Основной метод для хода ИИ
    void processEnemyTurn(Character& enemy, Character& player);
    
    // Действия ИИ
    void enemyAttack(Character& enemy, Character& player);
    void enemyUseMagic(Character& enemy, Character& player);
    void enemyUseItem(Character& enemy);
    
private:
    UIManager& uiManager;
    
    // Вспомогательные методы
    int calculateDamage(const Character& attacker, const Character& defender);
};

#endif // AI_CONTROLLER_H 