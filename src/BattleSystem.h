#ifndef BATTLE_SYSTEM_H
#define BATTLE_SYSTEM_H

#include "Character.h"
#include "UIManager.h"

class BattleSystem {
public:
    BattleSystem(UIManager& uiManager);
    
    // Основные методы боевой системы
    bool startBattle(Character& player, Character& enemy);
    void processPlayerTurn(Character& player, Character& enemy);
    
    // Действия игрока
    void attackEnemy(Character& player, Character& enemy);
    void useMagic(Character& player, Character& enemy);
    void useItem(Character& player, Character& enemy);
    void showEnemyInfo(const Character& enemy);
    void exitGame();
    
private:
    UIManager& uiManager;
    
    // Вспомогательные методы
    int calculateDamage(const Character& attacker, const Character& defender);
    void clearScreen();
};

#endif // BATTLE_SYSTEM_H 