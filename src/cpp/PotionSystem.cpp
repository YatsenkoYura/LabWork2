#include "../h/PotionSystem.h"
#include <iostream>
#include <string>
#include <ctime>

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

PotionSystem::PotionSystem(UIManager& uiManager, BuffSystem& buffSystem) 
    : uiManager(uiManager), buffSystem(buffSystem) {
}

bool PotionSystem::usePotion(Character& character, const Item& item) {
    const std::string& itemName = item.name;
    
    // Определяем тип зелья по названию
    if (itemName.find("зелье здоровья") != std::string::npos) {
        return useHealthPotion(character, item);
    } 
    else if (itemName.find("зелье маны") != std::string::npos || itemName == "Зелье медитации") {
        return useManaPotion(character, item);
    }
    else if (itemName == "Зелье силы" || itemName == "Эликсир защиты" || 
             itemName == "Эликсир ярости" || itemName == "Зелье берсерка" ||
             itemName == "Зелье хаоса") {
        return useBuffPotion(character, item);
    }
    else if (itemName == "Зелье регенерации" || itemName == "Зелье маны") {
        return useRegenPotion(character, item);
    }
    else {
        std::cout << "Неизвестный тип зелья!" << std::endl;
        return false;
    }
}

bool PotionSystem::useHealthPotion(Character& character, const Item& item) {
    int healAmount = extractValueFromDescription(item.description, "HP");
    
    if (healAmount > 0) {
        character.heal(healAmount);
        std::cout << character.getName() << " использует " << item.name 
                  << " и восстанавливает " << healAmount << " очков здоровья!" << std::endl;
        pauseForDialogue();
        return true;
    }
    
    return false;
}

bool PotionSystem::useManaPotion(Character& character, const Item& item) {
    if (!character.doesHaveMana()) {
        std::cout << "У персонажа нет маны!" << std::endl;
        pauseForDialogue();
        return false;
    }
    
    int manaAmount = 0;
    
    if (item.name == "Зелье медитации") {
        manaAmount = character.getMaxMana();
        
        // Специальный эффект: оглушение на 1 ход
        std::cout << character.getName() << " использует Зелье медитации и полностью восстанавливает ману, "
                  << "но пропустит следующий ход." << std::endl;
        
        // Здесь мы возвращаем true и true для последнего параметра, 
        // чтобы показать, что нужно установить оглушение
        return true;
    } else {
        manaAmount = extractValueFromDescription(item.description, "MP");
    }
    
    if (manaAmount > 0) {
        character.useMana(-manaAmount); // Отрицательное значение для восстановления
        std::cout << character.getName() << " использует " << item.name 
                  << " и восстанавливает " << manaAmount << " очков маны!" << std::endl;
        pauseForDialogue();
        return true;
    }
    
    return false;
}

bool PotionSystem::useBuffPotion(Character& character, const Item& item) {
    const std::string& itemName = item.name;
    
    if (itemName == "Зелье силы") {
        buffSystem.addBuff(character, itemName, 10, 0, 0, 0, 3);
        return true;
    }
    else if (itemName == "Эликсир защиты") {
        buffSystem.addBuff(character, itemName, 0, 15, 0, 0, 3);
        return true;
    }
    else if (itemName == "Эликсир ярости") {
        buffSystem.addBuff(character, itemName, 20, -10, 0, 0, 2);
        return true;
    }
    else if (itemName == "Зелье берсерка") {
        buffSystem.addBuff(character, itemName, 15, 0, 0, 0, 3);
        character.boostHealth(10);
        std::cout << "Максимальное здоровье увеличено на 10 на 3 хода." << std::endl;
        return true;
    }
    else if (itemName == "Зелье хаоса") {
        return useSpecialPotion(character, item);
    }
    
    return false;
}

bool PotionSystem::useRegenPotion(Character& character, const Item& item) {
    const std::string& itemName = item.name;
    
    if (itemName == "Зелье регенерации") {
        buffSystem.addBuff(character, itemName, 0, 0, 10, 0, 3);
        return true;
    }
    else if (itemName == "Зелье маны" && character.doesHaveMana()) {
        buffSystem.addBuff(character, itemName, 0, 0, 0, 5, 3);
        return true;
    }
    
    return false;
}

bool PotionSystem::useSpecialPotion(Character& character, const Item& item) {
    // Для зелья хаоса реализуем случайный эффект
    if (item.name == "Зелье хаоса") {
        std::srand(static_cast<unsigned int>(std::time(nullptr)));
        int randStat = std::rand() % 3;
        
        if (randStat == 0) {
            buffSystem.addBuff(character, item.name, 25, 0, 0, 0, 2);
            std::cout << "Случайный эффект: ваша атака увеличена на 25 на 2 хода." << std::endl;
        } else if (randStat == 1) {
            buffSystem.addBuff(character, item.name, 0, 25, 0, 0, 2);
            std::cout << "Случайный эффект: ваша защита увеличена на 25 на 2 хода." << std::endl;
        } else {
            buffSystem.addBuff(character, item.name, 0, 0, 25, 0, 2);
            character.heal(25);
            std::cout << "Случайный эффект: вы восстановили 25 здоровья и получите еще 25 в следующем ходу." << std::endl;
        }
        
        return true;
    }
    
    return false;
}

int PotionSystem::extractValueFromDescription(const std::string& description, const std::string& suffix) {
    size_t pos = description.find(suffix);
    if (pos == std::string::npos) {
        return 0;
    }
    
    // Ищем последний пробел перед суффиксом
    size_t valueStart = description.find_last_of(' ', pos) + 1;
    
    // Извлекаем подстроку с числом
    std::string valueStr = description.substr(valueStart, pos - valueStart);
    
    // Преобразуем в число
    try {
        return std::stoi(valueStr);
    } catch (...) {
        return 0;
    }
}

void PotionSystem::pauseForDialogue() const {
    // Диалоговая пауза (2 секунды)
    #ifdef _WIN32
    Sleep(2000);
    #else
    usleep(2000000);
    #endif
} 