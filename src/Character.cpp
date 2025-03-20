#include "Character.h"

Character::Character() 
    : name(""), health(100), maxHealth(100), defense(10), 
      attack(10), mana(50), maxMana(50), gold(0), hasMana(true) {
}

Character::Character(const std::string& name, int hp, int def, int mp, bool hasMp, int atk)
    : name(name), health(hp), maxHealth(hp), defense(def), 
      attack(atk), mana(mp), maxMana(mp), gold(0), hasMana(hasMp) {
}

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

void Character::takeDamage(int amount) {
    health -= amount;
    if (health < 0) {
        health = 0;
    }
}

bool Character::useMana(int amount) {
    if (!hasMana || (amount > 0 && amount > mana)) {
        return false;
    }
    mana -= amount;
    if (mana > maxMana) {
        mana = maxMana;
    }
    return true;
}

void Character::heal(int amount) {
    health += amount;
    if (health > maxHealth) {
        health = maxHealth;
    }
}

void Character::boostAttack(int amount) {
    attack += amount;
}

void Character::boostDefense(int amount) {
    defense += amount;
}

void Character::boostHealth(int amount) {
    maxHealth += amount;
    health += amount;
}

void Character::boostMana(int amount) {
    if (hasMana) {
        maxMana += amount;
        mana += amount;
        
        if (mana > maxMana) {
            mana = maxMana;
        }
    }
}

void Character::setGold(int amount) {
    gold = amount;
}

void Character::addGold(int amount) {
    gold += amount;
}

bool Character::spendGold(int amount) {
    if (amount > gold) {
        return false;
    }
    gold -= amount;
    return true;
}

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

