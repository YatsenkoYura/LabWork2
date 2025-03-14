#ifndef UIMANAGER_H
#define UIMANAGER_H

#include <iostream>
#include <string>
#include "Character.h"
#include <iomanip>

#ifndef _WIN32
#include <termios.h>
#endif

class UIManager {
public:
    UIManager();
    ~UIManager();
    void showScreen(const std::string& screenName);
    int getPlayerChoice();
    
    // Методы для обработки меню
    int processMainMenu();
    int processSettingsMenu();
    
    // Отображение боевого экрана
    void displayBattleScreen(const Character& player, const Character& enemy);
    
    // Отображение меню магических способностей
    void displayMagicOptions(const Character& player);
    
    // Отображение инвентаря
    void displayInventory(const Character& player);
    
    // Отображение результата сражения
    void displayBattleResult(const std::string& message);
    
    // Очистка экрана
    void clearScreen();
    
    // Новый метод для получения одного символа без нажатия Enter
    char getCharImmediate();
    
private:
    // Вспомогательные методы для работы с терминалом
#ifndef _WIN32
    void setNonCanonicalMode();
    void restoreTerminalMode();
    
    // Сохранение исходных настроек терминала
    struct termios originalTermios;
#endif
};

#endif // UIMANAGER_H 