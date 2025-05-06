#ifndef BATTLE_SYSTEM_H
#define BATTLE_SYSTEM_H

#include "Character.h"
#include "UIManager.h"
#include "AIController.h"
#include "BuffSystem.h"
#include "PotionSystem.h"
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

class UIManager;
class AIController;
class BuffSystem;
class PotionSystem;
struct EnemyAttack;

class BattleSystem {
public:
    BattleSystem(UIManager& uiManager, AIController& ai);
    
    void startBattle(Character& player, Character& enemy, int round);
    void attackEnemy(Character& player, Character& enemy);
    bool castMagic(Character& player, Character& enemy, int spellChoice);
    void useItem(Character& player, Character& enemy);
    void exitGame();
    
    void processPlayerTurn(Character& player, Character& enemy);
    void executeEnemyAction(Character& player, Character& enemy, const EnemyAttack& action, int playerLastChoice, int playerLastSpell = 0);
    void executePlayerTurn(Character& player, Character& enemy, int playerChoice, int& playerLastChoice, int& playerLastSpell, int currentRound);
    void displayBattleResult(const std::string& result);
    void pauseForDialogue() const;
    void updateBuffs(Character& player);
    
    int calculateDamage(const Character& attacker, const Character& defender);
    
    void setDialoguePauseDuration(int milliseconds);
    int getDialoguePauseDuration() const;
    
    void displayEnemyResponseToAttack(Character& enemy, bool successfulAttack, int damage);
    void displayEnemyResponseToDodge(Character& enemy, bool playerDodged, int playerLastSpell);
    void displayEnemyResponseToStun(Character& enemy, bool playerStunned);
    void displayEnemyAttackStats();
    void displayEnemyHint(const EnemyAttack& action);
    
private:
    UIManager& uiManager;
    AIController& aiController;
    BuffSystem buffSystem;
    PotionSystem potionSystem;
    
    bool playerStunned;
    int playerStunDuration;
    
    int dialoguePauseDuration;
    
    void showEnemyInfo(const Character& enemy);
    void useMagic(Character& player, Character& enemy, int spellChoice);
    void showEnemyInfo(const EnemyAttack& nextAction);
    void clearScreen();
};

#endif