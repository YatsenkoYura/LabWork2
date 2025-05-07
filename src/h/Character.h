#ifndef CHARACTER_H
#define CHARACTER_H

#include <string>
#include <vector>

/**
 * @file Character.h
 * @brief Defines the data structures for game characters and their inventory items.
 */

/**
 * @struct Item
 * @brief Represents an individual inventory item held by a character.
 *
 * Used for consumables, weapons, quest items, and other objects carried by the player or enemy.
 */
struct Item {
    std::string name;         /**< The name of the item. */
    std::string description;  /**< A short textual description of the item. */

    /**
     * @brief Constructs a new Item with a name and description.
     * @param itemName The name of the item.
     * @param itemDesc The description of the item.
     */
    Item(const std::string& itemName, const std::string& itemDesc)
            : name(itemName), description(itemDesc) {}
};

/**
 * @class Character
 * @brief Represents a character in the game, such as the player or an enemy (Den).
 *
 * Contains full statistics, inventory, currency, and capability information for a game entity.
 * Supports damage, healing, upgrades, and inventory management.
 */
class Character {
public:
    /**
     * @brief Default constructor. Initializes an empty character.
     */
    Character();

    /**
     * @brief Constructs a new Character with given parameters.
     * @param name Name of the character.
     * @param hp Initial and maximum health points.
     * @param def Initial defense stat.
     * @param mp Initial and maximum mana points.
     * @param hasMp Whether the character uses mana or not.
     * @param atk Initial attack stat.
     */
    Character(const std::string& name, int hp, int def, int mp, bool hasMp, int atk);

    /**
     * @brief Gets the character's name.
     * @return Name of the character.
     */
    std::string getName() const;

    /**
     * @brief Reads the current health value.
     * @return Current health points.
     */
    int getHealth() const;

    /**
     * @brief Gets the maximum health points.
     * @return The character's max HP.
     */
    int getMaxHealth() const;

    /**
     * @brief Gets current defense stat.
     * @return Current defense value.
     */
    int getDefense() const;

    /**
     * @brief Gets current attack stat.
     * @return Current attack value.
     */
    int getAttack() const;

    /**
     * @brief Gets the current mana points.
     * @return Current mana value.
     */
    int getMana() const;

    /**
     * @brief Gets the maximum mana points.
     * @return Max mana value.
     */
    int getMaxMana() const;

    /**
     * @brief Gets the character's current gold amount.
     * @return Value of gold held.
     */
    int getGold() const;

    /**
     * @brief Returns true if the character uses mana (has spells, etc.).
     * @return True if the character has mana capacity, false otherwise.
     */
    bool doesHaveMana() const;

    /**
     * @brief Checks if the character is still alive (health > 0).
     * @return True if alive, false if defeated.
     */
    bool isAlive() const;

    /**
     * @brief Applies damage to the character, reducing current health.
     * @param amount Amount of damage to deal.
     */
    void takeDamage(int amount);

    /**
     * @brief Consumes mana for spell/ability use.
     * @param amount Amount of mana to spend.
     * @return True if enough mana was available and spent; false if insufficient mana.
     */
    bool useMana(int amount);

    /**
     * @brief Restores health to the character (up to max health).
     * @param amount Amount of health to restore.
     */
    void heal(int amount);

    /**
     * @brief Increases attack stat by specified amount.
     * @param amount Value to add to attack.
     */
    void boostAttack(int amount);

    /**
     * @brief Increases defense stat by specified amount.
     * @param amount Value to add to defense.
     */
    void boostDefense(int amount);

    /**
     * @brief Increases both current and max health by the specified amount.
     * @param amount Value to add to health stats.
     */
    void boostHealth(int amount);

    /**
     * @brief Increases both current and max mana by the specified amount.
     * @param amount Value to add to mana stats.
     */
    void boostMana(int amount);

    /**
     * @brief Sets the character's gold amount to a specific value.
     * @param amount New gold value.
     */
    void setGold(int amount);

    /**
     * @brief Adds gold to the character.
     * @param amount Gold to add.
     */
    void addGold(int amount);

    /**
     * @brief Attempts to spend a given amount of gold; fails if not enough gold.
     * @param amount Gold to be spent.
     * @return True if transaction successful, false otherwise.
     */
    bool spendGold(int amount);

    /**
     * @brief Provides read-only access to the character's inventory.
     * @return A reference to the vector of Item objects.
     */
    const std::vector<Item>& getInventory() const;

    /**
     * @brief Adds an item to inventory.
     * @param item The item to add.
     */
    void addToInventory(const Item& item);

    /**
     * @brief Removes an item from inventory by index.
     * @param index Index in inventory to remove item from.
     */
    void removeFromInventory(int index);

private:
    std::string name;            /**< Character's name (for UI, logs, etc). */
    int health;                  /**< Current health points (HP). */
    int maxHealth;               /**< Maximum health points. */
    int defense;                 /**< Current defense stat. */
    int attack;                  /**< Current attack stat. */
    int mana;                    /**< Current mana points. */
    int maxMana;                 /**< Maximum mana points. */
    int gold;                    /**< Amount of gold held by the character. */
    bool hasMana;                /**< True if the character uses mana. */
    std::vector<Item> inventory; /**< List of inventory items carried. */
};

#endif