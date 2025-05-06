#ifndef SCORE_MANAGER_H
#define SCORE_MANAGER_H

#include <string>
#include <vector>
#include <fstream>
#include <algorithm>
#include "UIManager.h"
#include "Character.h"  // Добавляем для доступа к данным персонажа

// Структура для хранения результата игрока
struct PlayerScore {
    std::string playerName;
    int score;
    int rounds;

    // Добавляем конструктор по умолчанию
    PlayerScore() : playerName(""), score(0), rounds(0) {}
    
    PlayerScore(const std::string& name, int s, int r) 
        : playerName(name), score(s), rounds(r) {}
    
    // Для сортировки по убыванию очков
    bool operator<(const PlayerScore& other) const {
        return score > other.score; // Сортировка по убыванию
    }
};

// Структура для сохранения игры
struct GameSaveData {
    std::string playerName;
    int playerHealth;
    int playerMaxHealth;
    int playerAttack;
    int playerDefense;
    int playerMana;
    int playerMaxMana;
    int playerGold;
    int currentRound;
    int currentScore;
    std::vector<std::string> inventory; // Названия предметов в инвентаре
    
    GameSaveData() {}
};

class ScoreManager {
public:
    ScoreManager(UIManager& uiManager);
    
    // Устанавливает и получает текущий счет
    void setScore(int score);
    int getScore() const;
    
    // Добавляет очки к текущему счету
    void addScore(int points);
    
    // Сохраняет результат игры
    bool saveScore(const std::string& playerName, int rounds);
    
    // Загружает таблицу лидеров
    std::vector<PlayerScore> loadLeaderboard();
    
    // Отображает таблицу лидеров на экране
    void displayLeaderboard();
    
    // Предлагает сохранить результат и обрабатывает ввод имени
    void processGameOver(int rounds);
    
    // Новые методы для сохранения и загрузки игры
    bool saveGame(const Character& player, int currentRound, const std::string& saveName);
    bool loadGame(Character& player, int& currentRound, std::string& playerName);
    void displaySaveGameMenu(const Character& player, int currentRound);
    void displayLoadGameMenu();
    bool hasSavedGames() const;
    
private:
    UIManager& uiManager;
    int currentScore;
    
    // Путь к файлу с таблицей лидеров
    const std::string leaderboardFile = "leaderboard.txt";
    
    // Путь к файлу с сохраненными играми
    const std::string saveGameFile = "savegames.dat";
    
    // Максимальное количество записей в таблице лидеров
    const int maxLeaderboardEntries = 5;
    
    // Максимальное количество сохранений
    const int maxSaveGames = 3;
};

#endif 