#ifndef SCORE_MANAGER_H
#define SCORE_MANAGER_H

#include <string>
#include <vector>
#include <fstream>
#include <algorithm>
#include "UIManager.h"
#include "Character.h"

/**
 * @file ScoreManager.h
 * @brief Manages leaderboard, player scores and simple save/load.
 */

/**
 * @struct PlayerScore
 * @brief Stores a player's name, score, and amount of finished rounds.
 */
struct PlayerScore {
    std::string playerName; ///< Player's name.
    int score;              ///< High score value.
    int rounds;             ///< Number of rounds reached.

    PlayerScore() : playerName(""), score(0), rounds(0) {}
    PlayerScore(const std::string& name, int s, int r)
            : playerName(name), score(s), rounds(r) {}
    bool operator<(const PlayerScore& other) const {
        return score > other.score;
    }
};

/**
 * @struct GameSaveData
 * @brief Stores data about a single saved game session.
 */
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
    std::vector<std::string> inventory;

    GameSaveData() {}
};

/**
 * @class ScoreManager
 * @brief Handles high scores, leaderboard display, and simple save/load system.
 */
class ScoreManager {
public:
    /// Constructor.
    ScoreManager(UIManager& uiManager);

    /// Set current score.
    void setScore(int score);
    /// Get current score.
    int getScore() const;
    /// Add points to score.
    void addScore(int points);

    /// Save player's score and rounds to leaderboard file.
    bool saveScore(const std::string& playerName, int rounds);

    /// Load all entries from leaderboard.
    std::vector<PlayerScore> loadLeaderboard();

    /// Show leaderboard via UI.
    void displayLeaderboard();

    /// Handle game over: save score and display leaderboard.
    void processGameOver(int rounds);

    /// Save current game state.
    bool saveGame(const Character& player, int currentRound, const std::string& saveName);
    /// Load previously saved game state.
    bool loadGame(Character& player, int& currentRound, std::string& playerName);

    /// Show menu for saving current game.
    void displaySaveGameMenu(const Character& player, int currentRound);
    /// Show menu and list for loading game.
    void displayLoadGameMenu();

    /// Return true if there are any save files.
    bool hasSavedGames() const;

private:
    UIManager& uiManager;      ///< Reference to UI.
    int currentScore;          ///< Current running score.

    const std::string leaderboardFile = "leaderboard.txt"; ///< Leaderboard file path.
    const std::string saveGameFile = "savegames.dat";      ///< Save game file path.
    const int maxLeaderboardEntries = 5; ///< Max number of scores shown.
    const int maxSaveGames = 3;          ///< Max number of saves.
};

#endif