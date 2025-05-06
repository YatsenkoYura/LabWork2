#include "../h/AIController.h"
#include "../h/UIManager.h"
#include "../h/InventorySystem.h"
#include "../h/PotionSystem.h"
#include "../h/BuffSystem.h"
#include <iostream>
#include <cstdlib>
#include <random>
#include <algorithm>

AIController::AIController(UIManager& uiManager) : uiManager(uiManager) {
    availableActions = {
        {EnemyAttackType::NORMAL_ATTACK, "Обычная атака", 5, 1},
        {EnemyAttackType::DODGE_ATTACK, "Атака с уклонением", 4, 2},
        {EnemyAttackType::STUN_ATTACK, "Оглушающая атака", 1, 3}
    };
    
    resetAvailableAttacks();
    
    attackInFirstRound = false;
    firstRoundAttackCounter = 0;
    firstRoundSpecialAttackLimit = 1;
    
    lastAttackType = EnemyAttackType::SKIP_TURN;
}

void AIController::resetAvailableAttacks() {
    availableAttacks.clear();
}

void AIController::distributePointsBasedOnPlayerChoice(Character& enemy, int playerChoice) {
    switch (playerChoice) {
        case 1:
            enemy.boostAttack(3);
            std::cout << "Дэн увеличивает свою атаку на 3!" << std::endl;
            break;
        case 2:
            enemy.boostHealth(10);
            std::cout << "Дэн увеличивает свое здоровье на 10!" << std::endl;
            break;
        case 3:
            enemy.boostMana(5);
            std::cout << "Дэн получает 5 дополнительных очков действий!" << std::endl;
            break;
        default:
            std::random_device rd;
            std::mt19937 gen(rd());
            std::uniform_int_distribution<> distrib(1, 3);
            
            int aiChoice = distrib(gen);
            distributePointsBasedOnPlayerChoice(enemy, aiChoice);
            break;
    }
}

void AIController::buyAttacksForRound(Character& enemy, int round) {
    resetAvailableAttacks();
    
    int availablePoints = enemy.getMana();
    
    int dodgeAttackCost = 10;
    int stunAttackCost = 15;
    int normalAttackCost = 5;
    
    int dodgePower = 8 + round * 3;
    int stunPower = 1;
    int normalPower = enemy.getAttack();

    normalPower += round;
    
    if (availablePoints >= stunAttackCost) {
        buyAttackOfType(EnemyAttackType::STUN_ATTACK, 1, stunPower, stunAttackCost, "Оглушающий удар");
        availablePoints -= stunAttackCost;
    }
    
    int dodgeAttacksCount = availablePoints / dodgeAttackCost;
    if (dodgeAttacksCount > 0) {
        buyAttackOfType(EnemyAttackType::DODGE_ATTACK, dodgeAttacksCount, dodgePower, dodgeAttackCost, "Размашистый удар");
        availablePoints -= dodgeAttacksCount * dodgeAttackCost;
    }
    int normalAttacksCount = availablePoints / normalAttackCost;
    if (normalAttacksCount > 0) {
        buyAttackOfType(EnemyAttackType::NORMAL_ATTACK, normalAttacksCount, normalPower, normalAttackCost, "Прямой удар");
        availablePoints -= normalAttacksCount * normalAttackCost;
    }
    
    if (availableAttacks.empty()) {
        EnemyAttack skipTurn = { EnemyAttackType::SKIP_TURN, "Пропуск хода", 0, 0 };
        availableAttacks.push_back(skipTurn);
    }
    
    std::cout << "Дэн готовит " << availableAttacks.size() << " действий на этот раунд." << std::endl;
}

void AIController::buyAttackOfType(EnemyAttackType type, int count, int power, int manaCost, const std::string& name) {
    for (int i = 0; i < count; ++i) {
        EnemyAttack attack = { type, name, power, manaCost };
        availableAttacks.push_back(attack);
    }
}

void AIController::setFirstRoundBehavior() {
    attackInFirstRound = true;
    firstRoundAttackCounter = 0;
}

EnemyAttack AIController::determineNextAction(Character& enemy, int round, bool predictionOnly) {
    if (availableAttacks.empty()) {
        lastAttackType = EnemyAttackType::SKIP_TURN;
        return {EnemyAttackType::SKIP_TURN, "Пропуск хода", 0, 0};
    }
    
    if (round == 1 && attackInFirstRound) {
        if (firstRoundAttackCounter < firstRoundSpecialAttackLimit) {
            // В первом раунде пытаемся использовать оглушающую атаку
            std::vector<int> stunAttackIndices;
            for (size_t i = 0; i < availableAttacks.size(); ++i) {
                if (availableAttacks[i].type == EnemyAttackType::STUN_ATTACK) {
                    stunAttackIndices.push_back(i);
                }
            }
            
            if (!stunAttackIndices.empty()) {
                std::random_device rd;
                std::mt19937 gen(rd());
                std::uniform_int_distribution<> distrib(0, stunAttackIndices.size() - 1);
                int selectedStunIndex = stunAttackIndices[distrib(gen)];
                
                EnemyAttack selectedAttack = availableAttacks[selectedStunIndex];
                
                if (!predictionOnly) {
                    firstRoundAttackCounter++;
                    lastAttackType = selectedAttack.type;
                    availableAttacks.erase(availableAttacks.begin() + selectedStunIndex);
                }
                
                return selectedAttack;
            }
        } else {
            attackInFirstRound = false;
        }
    }
    
    int enemyActionPoints = enemy.getMana();
    
    if (enemyActionPoints <= 0) {
        lastAttackType = EnemyAttackType::SKIP_TURN;
        return {EnemyAttackType::SKIP_TURN, "Пропуск хода", 0, 0};
    }
    
    // Находим все доступные атаки, которые враг может использовать
    std::vector<int> availableIndices;
    for (size_t i = 0; i < availableAttacks.size(); ++i) {
        const auto& attack = availableAttacks[i];
        
        if (attack.type == EnemyAttackType::STUN_ATTACK && lastAttackType == EnemyAttackType::STUN_ATTACK) {
            continue;
        }
        
        if (attack.cost <= enemyActionPoints) {
            availableIndices.push_back(i);
        }
    }
    
    if (availableIndices.empty()) {
        lastAttackType = EnemyAttackType::SKIP_TURN;
        return {EnemyAttackType::SKIP_TURN, "Пропуск хода", 0, 0};
    }
    
    // Используем одно и то же зерно для генератора случайных чисел,
    // чтобы predictionOnly и реальное использование дали один и тот же результат
    std::random_device rd;
    unsigned seed = rd();
    std::mt19937 gen(seed);
    std::uniform_int_distribution<> distrib(0, availableIndices.size() - 1);
    
    int randomIndex = distrib(gen);
    int selectedIndex = availableIndices[randomIndex];
    EnemyAttack selectedAttack = availableAttacks[selectedIndex];
    
    if (!predictionOnly) {
        lastAttackType = selectedAttack.type;
        availableAttacks.erase(availableAttacks.begin() + selectedIndex);
    }
    
    return selectedAttack;
}

void AIController::processEnemyTurn(Character& enemy, Character& player) {
    
    
    int action = rand() % 3;
    
    switch (action) {
        case 0:
            enemyAttack(enemy, player);
            break;
        case 1:
            if (enemy.getMana() >= 10) {
                enemyUseMagic(enemy, player);
            } else {
                enemyAttack(enemy, player);
            }
            break;
        case 2:
            if (!enemy.getInventory().empty()) {
                enemyUseItem(enemy);
            } else {
                enemyAttack(enemy, player);
            }
            break;
    }
}

void AIController::enemyAttack(Character& enemy, Character& player) {
    int damage = calculateDamage(enemy, player);
    player.takeDamage(damage);
    
    uiManager.displayBattleScreen(player, enemy);
}

void AIController::enemyUseMagic(Character& enemy, Character& player) {
    int damage = calculateDamage(enemy, player) + 3;
    player.takeDamage(damage);
    
    uiManager.displayBattleScreen(player, enemy);
}

void AIController::enemyUseItem(Character& enemy) {
    if (enemy.getInventory().empty()) {
        return;
    }
    
    // Создаем временные экземпляры необходимых систем
    BuffSystem buffSystem(uiManager);
    PotionSystem potionSystem(uiManager, buffSystem);
    
    // Выбираем случайный предмет из инвентаря
    int itemIndex = rand() % enemy.getInventory().size();
    const Item& selectedItem = enemy.getInventory()[itemIndex];
    
    // Используем предмет через PotionSystem
    bool potionUsed = potionSystem.usePotion(enemy, selectedItem);
    
    if (potionUsed) {
        // Удаляем предмет из инвентаря
        InventorySystem inventorySystem(uiManager);
        inventorySystem.removeItem(enemy, itemIndex);
    }
}

int AIController::calculateDamage(const Character& attacker, const Character& defender) {
    int baseDamage = 8;
    int damage = baseDamage - defender.getDefense() / 5;
    
    if (damage < 1) damage = 1;
    
    return damage;
} 