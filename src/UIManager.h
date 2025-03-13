#ifndef UIMANAGER_H
#define UIMANAGER_H

#include <iostream>
#include <string>

class UIManager {
public:
    UIManager();
    ~UIManager();
    void showScreen(const std::string& screenName);
    int getPlayerChoice();
    
    // Методы для обработки меню
    int processMainMenu();
    int processSettingsMenu();
};

#endif // UIMANAGER_H 