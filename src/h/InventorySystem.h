#ifndef INVENTORY_SYSTEM_H
#define INVENTORY_SYSTEM_H

#include "Character.h"
#include "UIManager.h"

/**
 * @file InventorySystem.h
 * @brief Declares the InventorySystem class, used for managing character inventories.
 */

/**
 * @class InventorySystem
 * @brief Handles inventory management for characters, including adding, removing, and using items.
 *
 * Provides an interface between the UI, characters, and the in-game logic for manipulating items,
 * supporting operations like item addition, consumption, and visual feedback.
 */
class InventorySystem {
public:
    /**
     * @brief Constructs an InventorySystem and links it to the UI manager.
     * @param uiManager Reference to the UI manager for displaying messages and prompts.
     */
    InventorySystem(UIManager& uiManager);

    /**
     * @brief Adds an item to the specified character's inventory.
     * @param character Reference to the character receiving the item.
     * @param item The item to add.
     *
     * If inventory limits exist, this function should enforce them.
     */
    void addItem(Character& character, const Item& item);

    /**
     * @brief Removes an item from the specified character's inventory by index.
     * @param character Reference to the character.
     * @param itemIndex Index of the item to remove (zero-based).
     *
     * Has no effect if the index is invalid.
     */
    void removeItem(Character& character, int itemIndex);

    /**
     * @brief Uses (consumes or equips) an item from the character's inventory.
     * @param character Reference to the character.
     * @param itemIndex Index of the item to use (zero-based).
     *
     * Behavior depends on item type. May call UI to display usage results.
     */
    void useItem(Character& character, int itemIndex);

private:
    UIManager& uiManager; /**< Reference to UI manager for user feedback and prompts. */
};

#endif