#include "../h/BuffSystem.h"
#include <iostream>

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

BuffSystem::BuffSystem(UIManager& uiManager) : uiManager(uiManager) {
}

void BuffSystem::addBuff(Character& character, const std::string& name, int attackBonus, 
                        int defenseBonus, int healthRegen, int manaRegen, int duration) {
    activeBuffs.push_back(ActiveBuff(name, attackBonus, defenseBonus, healthRegen, manaRegen, duration));
    
    if (attackBonus != 0) {
        character.boostAttack(attackBonus);
    }
    
    if (defenseBonus != 0) {
        character.boostDefense(defenseBonus);
    }
    
    std::cout << "Эффект \"" << name << "\" применен к " << character.getName() << "!" << std::endl;
    
    if (attackBonus > 0) {
        std::cout << "Атака увеличена на " << attackBonus << " на " << duration << " ход(ов)." << std::endl;
    } else if (attackBonus < 0) {
        std::cout << "Атака уменьшена на " << -attackBonus << " на " << duration << " ход(ов)." << std::endl;
    }
    
    if (defenseBonus > 0) {
        std::cout << "Защита увеличена на " << defenseBonus << " на " << duration << " ход(ов)." << std::endl;
    } else if (defenseBonus < 0) {
        std::cout << "Защита уменьшена на " << -defenseBonus << " на " << duration << " ход(ов)." << std::endl;
    }
    
    if (healthRegen > 0) {
        std::cout << "Восстановление " << healthRegen << " HP каждый ход в течение " << duration << " ход(ов)." << std::endl;
    }
    
    if (manaRegen > 0 && character.doesHaveMana()) {
        std::cout << "Восстановление " << manaRegen << " MP каждый ход в течение " << duration << " ход(ов)." << std::endl;
    }
}

void BuffSystem::updateBuffs(Character& character) {
    if (activeBuffs.empty()) {
        return;
    }
    
    std::cout << "\nАктивные эффекты:" << std::endl;
    
    for (auto it = activeBuffs.begin(); it != activeBuffs.end(); ) {
        if (it == activeBuffs.end()) {
            break;
        }
        
        if (it->healthRegen > 0) {
            character.heal(it->healthRegen);
            std::cout << "Эффект \"" << it->name << "\" восстанавливает " << it->healthRegen << " HP!" << std::endl;
        }
        
        if (it->manaRegen > 0 && character.doesHaveMana()) {
            character.useMana(-it->manaRegen);
            std::cout << "Эффект \"" << it->name << "\" восстанавливает " << it->manaRegen << " MP!" << std::endl;
        }
        
        it->duration--;
        
        if (it->duration <= 0) {
            std::cout << "Эффект \"" << it->name << "\" закончился!" << std::endl;
            
            if (it->attackBonus != 0) {
                character.boostAttack(-it->attackBonus);
            }
            
            if (it->defenseBonus != 0) {
                character.boostDefense(-it->defenseBonus);
            }
            
            it = activeBuffs.erase(it);
        } else {
            std::cout << "Эффект \"" << it->name << "\" действует еще " << it->duration << " ход(ов)." << std::endl;
            ++it;
        }
    }
    
    pauseForDialogue();
}

void BuffSystem::clearBuffs() {
    activeBuffs.clear();
}

bool BuffSystem::hasBuff(const std::string& name) const {
    for (const auto& buff : activeBuffs) {
        if (buff.name == name) {
            return true;
        }
    }
    return false;
}

void BuffSystem::pauseForDialogue() const {
    #ifdef _WIN32
    Sleep(2000);
    #else
    usleep(2000000);
    #endif
} 