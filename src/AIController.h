#ifndef AI_CONTROLLER_H
#define AI_CONTROLLER_H

#include "Character.h"
#include <vector>
#include <string>

class UIManager;

enum class EnemyAttackType {
    NORMAL_ATTACK,
    DODGE_ATTACK,
    STUN_ATTACK,
    SKIP_TURN
};

struct EnemyAttack {
    EnemyAttackType type;
    std::string name;
    int power;
    int cost;
};

class AIController {
public:
    AIController(UIManager& uiManager);
    
    void distributePointsBasedOnPlayerChoice(Character& enemy, int playerChoice);
    
    EnemyAttack determineNextAction(Character& enemy, int round, bool predictionOnly = false);
    
    void buyAttacksForRound(Character& enemy, int round);
    
    void processEnemyTurn(Character& enemy, Character& player);
    void enemyAttack(Character& enemy, Character& player);
    void enemyUseMagic(Character& enemy, Character& player);
    void enemyUseItem(Character& enemy);
    int calculateDamage(const Character& attacker, const Character& defender);
    
    void setFirstRoundBehavior();
    
    std::vector<EnemyAttack> getAvailableAttacks() const {
        return availableAttacks;
    }
    
private:
    UIManager& uiManager;
    
    std::vector<EnemyAttack> availableAttacks;
    
    std::vector<EnemyAttack> availableActions;
    
    bool attackInFirstRound;
    
    int firstRoundAttackCounter;
    
    int firstRoundSpecialAttackLimit;
    
    EnemyAttackType lastAttackType;
    
    void buyAttackOfType(EnemyAttackType type, int count, int power, int manaCost, const std::string& name);
    
    void resetAvailableAttacks();
};

#endif