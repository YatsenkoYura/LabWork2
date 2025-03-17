#ifndef BATTLE_SYSTEM_H
#define BATTLE_SYSTEM_H

#include "Character.h"
#include "UIManager.h"
#include "AIController.h"
#include <string>
#include <vector>
#include <iostream>
#include <random>
#include <chrono>
#include <thread>

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

// Forward declarations
class UIManager;
class AIController;
struct EnemyAttack;

// Структура для отслеживания активных баффов игрока
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

class BattleSystem {
public:
    BattleSystem(UIManager& uiManager, AIController& ai);
    
    // Методы для управления боем
    void startBattle(Character& player, Character& enemy, int round);
    void attackEnemy(Character& player, Character& enemy);
    bool castMagic(Character& player, Character& enemy, int spellChoice);
    void useItem(Character& player, Character& enemy);
    void exitGame();
    
    // Методы для управления ходами
    void processPlayerTurn(Character& player, Character& enemy);
    void executeEnemyAction(Character& player, Character& enemy, const EnemyAttack& action, int playerLastChoice, int playerLastSpell = 0);
    void executePlayerTurn(Character& player, Character& enemy, int playerChoice, int& playerLastChoice, int& playerLastSpell, int currentRound);
    void displayBattleResult(const std::string& result);
    void pauseForDialogue() const;
    void updateBuffs(Character& player);
    
    // Методы для расчетов урона и т.д.
    int calculateDamage(const Character& attacker, const Character& defender);
    
    // Методы для управления настройками пауз
    void setDialoguePauseDuration(int milliseconds);
    int getDialoguePauseDuration() const;
    
    // Методы для отображения фраз противника
    void displayEnemyResponseToAttack(Character& enemy, bool successfulAttack, int damage);
    void displayEnemyResponseToDodge(Character& enemy, bool playerDodged, int playerLastSpell);
    void displayEnemyResponseToStun(Character& enemy, bool playerStunned);
    void displayEnemyAttackStats();
    void displayEnemyHint(const EnemyAttack& action);
    
private:
    UIManager& uiManager;
    AIController& aiController;
    
    // Состояние игрока
    bool playerStunned;
    int playerStunDuration;
    
    // Длительность пауз для диалогов (в миллисекундах)
    int dialoguePauseDuration;
    
    // Список активных баффов игрока
    std::vector<ActiveBuff> activeBuffs;
    
    // Вспомогательные методы
    void showEnemyInfo(const Character& enemy);
    void useMagic(Character& player, Character& enemy, int spellChoice);
    void showEnemyInfo(const EnemyAttack& nextAction);
    void clearScreen();
};

#endif // BATTLE_SYSTEM_H 