#include "AIController.h"
#include <iostream>
#include <cstdlib>

AIController::AIController(UIManager& uiManager) : uiManager(uiManager) {
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