#include "Character.h"

// Конструкторы
Character::Character() 
    : name(""), health(100), maxHealth(100), defense(10), 
      attack(10), mana(50), maxMana(50), gold(0), hasMana(true) {
}

Character::Character(const std::string& name, int hp, int def, int mp, bool hasMp, int atk)
    : name(name), health(hp), maxHealth(hp), defense(def), 
      attack(atk), mana(mp), maxMana(mp), gold(0), hasMana(hasMp) {
}

// Методы для получения информации о персонаже
std::string Character::getName() const {
    return name;
}

int Character::getHealth() const {
    return health;
}

int Character::getMaxHealth() const {
    return maxHealth;
}

int Character::getDefense() const {
    return defense;
}

int Character::getAttack() const {
    return attack;
}

int Character::getMana() const {
    return mana;
}

int Character::getMaxMana() const {
    return maxMana;
}

int Character::getGold() const {
    return gold;
}

bool Character::doesHaveMana() const {
    return hasMana;
}

bool Character::isAlive() const {
    return health > 0;
}

// Методы для изменения состояния персонажа
void Character::takeDamage(int amount) {
    health -= amount;
    if (health < 0) {
        health = 0;
    }
}

bool Character::useMana(int amount) {
    if (!hasMana || (amount > 0 && amount > mana)) {
        return false; // Недостаточно маны или нет маны вообще
    }
    mana -= amount; // Отрицательное значение восстанавливает ману
    if (mana > maxMana) {
        mana = maxMana; // Не больше максимума
    }
    return true;
}

void Character::heal(int amount) {
    health += amount;
    if (health > maxHealth) {
        health = maxHealth; // Не больше максимума
    }
}

// Методы для улучшения характеристик
void Character::boostAttack(int amount) {
    attack += amount;
}

void Character::boostDefense(int amount) {
    defense += amount;
}

void Character::boostHealth(int amount) {
    maxHealth += amount;
    health += amount; // Увеличиваем и текущее здоровье
}

void Character::boostMana(int amount) {
    if (hasMana) {
        maxMana += amount;
        mana += amount; // Увеличиваем и текущую ману
        
        // Проверка, чтобы мана не превышала максимум
        if (mana > maxMana) {
            mana = maxMana;
        }
    }
}

// Методы для работы с золотом
void Character::setGold(int amount) {
    gold = amount;
}

void Character::addGold(int amount) {
    gold += amount;
}

bool Character::spendGold(int amount) {
    if (amount > gold) {
        return false; // Недостаточно золота
    }
    gold -= amount;
    return true;
}

// Методы для работы с инвентарем
const std::vector<Item>& Character::getInventory() const {
    return inventory;
}

void Character::addToInventory(const Item& item) {
    inventory.push_back(item);
}

void Character::removeFromInventory(int index) {
    if (index >= 0 && index < static_cast<int>(inventory.size())) {
        inventory.erase(inventory.begin() + index);
    }
}

// Реализация методов класса Character, если потребуется 