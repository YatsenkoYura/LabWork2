#ifndef AI_CONTROLLER_H
#define AI_CONTROLLER_H

#include "Character.h"
#include <vector>
#include <string>

// Forward declaration
class UIManager;

// Типы атак противника
enum class EnemyAttackType {
    NORMAL_ATTACK,   // Обычная атака
    DODGE_ATTACK,    // Атака с уклонением
    STUN_ATTACK,     // Оглушающая атака
    SKIP_TURN        // Пропуск хода
};

// Структура для описания атаки противника
struct EnemyAttack {
    EnemyAttackType type;
    std::string name;
    int power;
    int cost;
};

class AIController {
public:
    AIController(UIManager& uiManager);
    
    // Метод для распределения очков после победы игрока
    void distributePointsBasedOnPlayerChoice(Character& enemy, int playerChoice);
    
    // Метод для определения следующего действия противника
    EnemyAttack determineNextAction(Character& enemy, int round, bool predictionOnly = false);
    
    // Метод для покупки атак в начале раунда
    void buyAttacksForRound(Character& enemy, int round);
    
    // Добавляем объявления недостающих методов
    void processEnemyTurn(Character& enemy, Character& player);
    void enemyAttack(Character& enemy, Character& player);
    void enemyUseMagic(Character& enemy, Character& player);
    void enemyUseItem(Character& enemy);
    int calculateDamage(const Character& attacker, const Character& defender);
    
    // Установить специальное поведение для первого раунда
    void setFirstRoundBehavior();
    
    // Метод для получения статистики доступных атак
    std::vector<EnemyAttack> getAvailableAttacks() const {
        return availableAttacks;
    }
    
private:
    UIManager& uiManager;
    
    // Доступные атаки противника после покупки
    std::vector<EnemyAttack> availableAttacks;
    
    // Доступные действия противника
    std::vector<EnemyAttack> availableActions;
    
    // Поведение в первом раунде (false - пропускать ходы, true - атаковать)
    bool attackInFirstRound;
    
    // Счетчик для контроля атак в первом раунде
    int firstRoundAttackCounter;
    
    // Лимит для особых атак в первом раунде
    int firstRoundSpecialAttackLimit;
    
    // Последний тип атаки - для предотвращения последовательных оглушающих атак
    EnemyAttackType lastAttackType;
    
    // Метод для покупки определенного типа атак
    void buyAttackOfType(EnemyAttackType type, int count, int power, int manaCost, const std::string& name);
    
    // Сбросить список доступных атак
    void resetAvailableAttacks();
};

#endif // AI_CONTROLLER_H 