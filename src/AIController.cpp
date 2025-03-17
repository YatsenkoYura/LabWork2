#include "AIController.h"
#include "UIManager.h"
#include <iostream>
#include <cstdlib>
#include <random>
#include <algorithm>

AIController::AIController(UIManager& uiManager) : uiManager(uiManager) {
    // Инициализируем вектор доступных действий
    availableActions = {
        {EnemyAttackType::NORMAL_ATTACK, "Обычная атака", 5, 1},
        {EnemyAttackType::DODGE_ATTACK, "Атака с уклонением", 4, 2},
        {EnemyAttackType::STUN_ATTACK, "Оглушающая атака", 1, 3}
    };
    
    resetAvailableAttacks();
    
    // Инициализируем поведение для первого раунда
    attackInFirstRound = false;
    firstRoundAttackCounter = 0;
    firstRoundSpecialAttackLimit = 1;
    
    // Инициализируем последний тип атаки как SKIP_TURN
    lastAttackType = EnemyAttackType::SKIP_TURN;
}

void AIController::resetAvailableAttacks() {
    availableAttacks.clear();
}

void AIController::distributePointsBasedOnPlayerChoice(Character& enemy, int playerChoice) {
    switch (playerChoice) {
        case 1: // Игрок выбрал увеличить здоровье, враг увеличивает атаку
            enemy.boostAttack(3);
            std::cout << "Дэн увеличивает свою атаку на 3!" << std::endl;
            break;
        case 2: // Игрок выбрал увеличить атаку, враг увеличивает здоровье
            enemy.boostHealth(10);
            std::cout << "Дэн увеличивает свое здоровье на 10!" << std::endl;
            break;
        case 3: // Игрок выбрал увеличить ману, враг увеличивает свои очки действий (ману)
            // Отдельно обрабатываем ману, так как у врага она используется как очки действий
            // Допустим, враг получает 3 очка действий за каждое очко маны игрока
            enemy.boostMana(5);
            std::cout << "Дэн получает 5 дополнительных очков действий!" << std::endl;
            break;
        default:
            // Если игрок не сделал выбор, ИИ случайно выбирает характеристику
            std::random_device rd;
            std::mt19937 gen(rd());
            std::uniform_int_distribution<> distrib(1, 3);
            
            int aiChoice = distrib(gen);
            distributePointsBasedOnPlayerChoice(enemy, aiChoice);
            break;
    }
}

void AIController::buyAttacksForRound(Character& enemy, int round) {
    // Сбрасываем предыдущие атаки
    resetAvailableAttacks();
    
    // Получаем доступные очки действий (ману) противника
    int availablePoints = enemy.getMana();
    
    // Базовые затраты на атаки
    int dodgeAttackCost = 15;
    int stunAttackCost = 20;
    int normalAttackCost = 10;
    
    // Базовая сила атак
    int dodgePower = 10 + round * 2;  // Уменьшение шанса уклонения
    int stunPower = 1;                // Количество пропускаемых ходов
    int normalPower = enemy.getAttack();  // Обычный урон на основе атаки

    dodgePower += round * 2;
    normalPower += round * 2;
    
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
    
    // Вывод информации о купленных атаках (для отладки)
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
    // Если это предсказание, возвращаем случайную атаку из доступных, но не удаляем её
    if (predictionOnly) {
        if (!availableAttacks.empty()) {
            std::random_device rd;
            std::mt19937 gen(rd());
            std::uniform_int_distribution<> distrib(0, availableAttacks.size() - 1);
            // Запоминаем индекс выбранной атаки для возможной синхронизации позже
            int selectedIndex = distrib(gen);
            return availableAttacks[selectedIndex];
        } else {
            return {EnemyAttackType::SKIP_TURN, "Пропуск хода", 0, 0};
        }
    }
    
    // Если у нас нет доступных атак, пропускаем ход
    if (availableAttacks.empty()) {
        lastAttackType = EnemyAttackType::SKIP_TURN;
        return {EnemyAttackType::SKIP_TURN, "Пропуск хода", 0, 0};
    }
    
    // Если мы в первом раунде и установлен флаг атаки
    if (round == 1 && attackInFirstRound) {
        // Если это первая атака в первом раунде, применяем специальное поведение
        if (firstRoundAttackCounter < firstRoundSpecialAttackLimit) {
            firstRoundAttackCounter++;
            
            // Ищем оглушающую атаку в доступных
            std::vector<int> stunAttackIndices;
            for (size_t i = 0; i < availableAttacks.size(); ++i) {
                if (availableAttacks[i].type == EnemyAttackType::STUN_ATTACK) {
                    stunAttackIndices.push_back(i);
                }
            }
            
            // Если есть оглушающие атаки, используем одну из них
            if (!stunAttackIndices.empty()) {
                std::random_device rd;
                std::mt19937 gen(rd());
                std::uniform_int_distribution<> distrib(0, stunAttackIndices.size() - 1);
                int selectedIndex = stunAttackIndices[distrib(gen)];
                
                EnemyAttack selectedAttack = availableAttacks[selectedIndex];
                lastAttackType = selectedAttack.type;
                
                // Удаляем использованную атаку из списка доступных
                availableAttacks.erase(availableAttacks.begin() + selectedIndex);
                
                return selectedAttack;
            }
        } else {
            // После лимита атак сбрасываем счетчик и переходим к обычному поведению
            attackInFirstRound = false;
        }
    }
    
    // Проверяем, есть ли у противника очки действий
    int enemyActionPoints = enemy.getMana();
    
    if (enemyActionPoints <= 0) {
        // Если очков действий нет, пропускаем ход
        lastAttackType = EnemyAttackType::SKIP_TURN;
        return {EnemyAttackType::SKIP_TURN, "Пропуск хода", 0, 0};
    }
    
    // Собираем доступные атаки в зависимости от очков действия
    std::vector<int> availableIndices;
    for (size_t i = 0; i < availableAttacks.size(); ++i) {
        const auto& attack = availableAttacks[i];
        
        // Не добавляем оглушающую атаку, если предыдущая атака тоже была оглушающей
        if (attack.type == EnemyAttackType::STUN_ATTACK && lastAttackType == EnemyAttackType::STUN_ATTACK) {
            continue;
        }
        
        if (attack.cost <= enemyActionPoints) {
            availableIndices.push_back(i);
        }
    }
    
    // Если нет доступных атак, пропускаем ход
    if (availableIndices.empty()) {
        lastAttackType = EnemyAttackType::SKIP_TURN;
        return {EnemyAttackType::SKIP_TURN, "Пропуск хода", 0, 0};
    }
    
    // Выбираем случайную атаку из доступных
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distrib(0, availableIndices.size() - 1);
    
    int selectedIndex = availableIndices[distrib(gen)];
    EnemyAttack selectedAttack = availableAttacks[selectedIndex];
    
    // Вычитаем стоимость атаки из очков действия противника
    enemy.useMana(selectedAttack.cost);
    
    // Запоминаем тип последней атаки
    lastAttackType = selectedAttack.type;
    
    // Удаляем использованную атаку из списка доступных
    availableAttacks.erase(availableAttacks.begin() + selectedIndex);
    
    return selectedAttack;
}

void AIController::processEnemyTurn(Character& enemy, Character& player) {
    // Простая логика ИИ для противника
    // В зависимости от сложности и текущей ситуации ИИ выбирает действие
    
    // Например, если у противника мало здоровья, он может использовать зелье
    // Если у игрока мало здоровья, он может атаковать
    // Если у противника достаточно маны, он может использовать магию
    
    int action = rand() % 3; // 0 - атака, 1 - магия, 2 - предмет
    
    switch (action) {
        case 0:
            // Атака
            enemyAttack(enemy, player);
            break;
        case 1:
            // Магия (если достаточно маны)
            if (enemy.getMana() >= 10) {
                enemyUseMagic(enemy, player);
            } else {
                enemyAttack(enemy, player); // Если недостаточно маны, атакует
            }
            break;
        case 2:
            // Предмет (если есть в инвентаре)
            if (!enemy.getInventory().empty()) {
                enemyUseItem(enemy);
            } else {
                enemyAttack(enemy, player); // Если инвентарь пуст, атакует
            }
            break;
    }
}

void AIController::enemyAttack(Character& enemy, Character& player) {
    int damage = calculateDamage(enemy, player);
    player.takeDamage(damage);
    
    // Не выводим сообщения, просто обновляем экран
    uiManager.displayBattleScreen(player, enemy);
}

void AIController::enemyUseMagic(Character& enemy, Character& player) {
    // Логика использования магии противником
    int damage = calculateDamage(enemy, player) + 3; // Магия наносит дополнительный урон
    player.takeDamage(damage);
    
    // Не выводим сообщения, просто обновляем экран
    uiManager.displayBattleScreen(player, enemy);
}

void AIController::enemyUseItem(Character& enemy) {
    // Логика использования предмета противником
    // Например, восстановление здоровья
    
    // Не выводим сообщения, просто обновляем экран
    // uiManager.displayBattleScreen(player, enemy);
}

int AIController::calculateDamage(const Character& attacker, const Character& defender) {
    // Простая формула расчета урона
    int baseDamage = 8; // Базовый урон противника
    int damage = baseDamage - defender.getDefense() / 5;
    
    // Минимальный урон 1
    if (damage < 1) damage = 1;
    
    return damage;
} 