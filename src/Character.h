#ifndef CHARACTER_H
#define CHARACTER_H

#include <string>
#include <vector>

// Структура для предметов инвентаря
struct Item {
    std::string name;
    std::string description;
    
    Item(const std::string& itemName, const std::string& itemDesc) 
        : name(itemName), description(itemDesc) {}
};

class Character {
public:
    // Конструктор персонажа
    Character();
    Character(const std::string& name, int hp, int def, int mp, bool hasMp, int atk);
    
    // Методы для получения информации о персонаже
    std::string getName() const;
    int getHealth() const;
    int getMaxHealth() const;
    int getDefense() const;
    int getAttack() const;
    int getMana() const;
    int getMaxMana() const;
    int getGold() const;
    bool doesHaveMana() const;
    bool isAlive() const;
    
    // Методы для изменения состояния персонажа
    void takeDamage(int amount);
    bool useMana(int amount);
    void heal(int amount);
    
    // Методы для улучшения характеристик
    void boostAttack(int amount);
    void boostDefense(int amount);
    void boostHealth(int amount);
    void boostMana(int amount);
    
    // Методы для работы с золотом
    void setGold(int amount);
    void addGold(int amount);
    bool spendGold(int amount);
    
    // Методы для работы с инвентарем
    const std::vector<Item>& getInventory() const;
    void addToInventory(const Item& item);
    void removeFromInventory(int index);
    
private:
    std::string name;
    int health;
    int maxHealth;
    int defense;
    int attack;
    int mana;
    int maxMana;
    int gold;
    bool hasMana;
    std::vector<Item> inventory;
};

#endif // CHARACTER_H 