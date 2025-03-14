#ifndef CHARACTER_H
#define CHARACTER_H

#include <string>
#include <vector>

class Item {
public:
    Item(const std::string& name = "") : name(name) {}
    std::string getName() const { return name; }
    
private:
    std::string name;
};

class Character {
public:
    Character(const std::string& name = "", int health = 100, int defense = 10, int mana = 50, bool hasMana = true)
        : name(name), health(health), defense(defense), mana(mana), hasMana(hasMana) {}
    
    std::string getName() const { return name; }
    int getHealth() const { return health; }
    int getDefense() const { return defense; }
    int getMana() const { return hasMana ? mana : 0; }
    bool doesHaveMana() const { return hasMana; }
    
    bool isAlive() const { return health > 0; }
    
    const std::vector<Item>& getInventory() const { return inventory; }
    
    void takeDamage(int damage) { health -= damage; if (health < 0) { health = 0; } }
    void boostDefense(int amount) { defense += amount; }
    
private:
    std::string name;
    int health;
    int defense;
    int mana;
    bool hasMana;
    std::vector<Item> inventory;
};

#endif // CHARACTER_H 