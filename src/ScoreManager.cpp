#include "ScoreManager.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <limits>

ScoreManager::ScoreManager(UIManager& uiManager) 
    : uiManager(uiManager), currentScore(0) {
}

void ScoreManager::setScore(int score) {
    currentScore = score;
}

int ScoreManager::getScore() const {
    return currentScore;
}

void ScoreManager::addScore(int points) {
    currentScore += points;
}

bool ScoreManager::saveScore(const std::string& playerName, int rounds) {
    if (playerName.empty()) {
        return false; // Если игрок не ввел имя, не сохраняем результат
    }
    
    // Загружаем существующую таблицу лидеров
    std::vector<PlayerScore> leaderboard = loadLeaderboard();
    
    // Добавляем новый результат
    leaderboard.push_back(PlayerScore(playerName, currentScore, rounds));
    
    // Сортируем результаты (оператор < определен для сортировки по убыванию)
    std::sort(leaderboard.begin(), leaderboard.end());
    
    // Если записей больше максимального количества, удаляем лишние
    if (leaderboard.size() > maxLeaderboardEntries) {
        leaderboard.resize(maxLeaderboardEntries);
    }
    
    // Сохраняем обновленную таблицу лидеров в файл
    std::ofstream file(leaderboardFile);
    if (!file.is_open()) {
        return false;
    }
    
    for (const auto& entry : leaderboard) {
        file << entry.playerName << "," << entry.score << "," << entry.rounds << std::endl;
    }
    
    file.close();
    return true;
}

std::vector<PlayerScore> ScoreManager::loadLeaderboard() {
    std::vector<PlayerScore> leaderboard;
    
    std::ifstream file(leaderboardFile);
    if (!file.is_open()) {
        return leaderboard; // Возвращаем пустой вектор, если файл не найден
    }
    
    std::string line;
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string name;
        std::string scoreStr;
        std::string roundsStr;
        
        // Формат строки: имя,очки,раунды
        if (std::getline(iss, name, ',') && 
            std::getline(iss, scoreStr, ',') && 
            std::getline(iss, roundsStr)) {
            
            try {
                int score = std::stoi(scoreStr);
                int rounds = std::stoi(roundsStr);
                leaderboard.push_back(PlayerScore(name, score, rounds));
            } catch (const std::exception& e) {
                // Пропускаем некорректные строки
                continue;
            }
        }
    }
    
    file.close();
    
    // Сортируем по убыванию очков
    std::sort(leaderboard.begin(), leaderboard.end());
    
    return leaderboard;
}

void ScoreManager::displayLeaderboard() {
    std::vector<PlayerScore> leaderboard = loadLeaderboard();
    
    #ifdef _WIN32
    system("cls");
    #else
    system("clear");
    #endif
    
    std::cout << "\n\n";
    std::cout << "              ТАБЛИЦА ЛИДЕРОВ              " << std::endl;
    std::cout << "------------------------------------------------" << std::endl;
    
    if (leaderboard.empty()) {
        std::cout << "\nТаблица лидеров пуста. Будьте первым!" << std::endl;
    } else {
        // Выводим заголовки
        std::cout << std::left 
                  << std::setw(5) << "Место" 
                  << std::setw(20) << "Имя" 
                  << std::setw(10) << "Очки" 
                  << std::setw(10) << "Раунды" 
                  << std::endl;
        std::cout << "------------------------------------------------" << std::endl;
        
        // Выводим данные лидеров
        for (size_t i = 0; i < leaderboard.size(); ++i) {
            const auto& entry = leaderboard[i];
            std::cout << std::left 
                      << std::setw(5) << (i + 1) 
                      << std::setw(20) << entry.playerName 
                      << std::setw(10) << entry.score 
                      << std::setw(10) << entry.rounds 
                      << std::endl;
        }
    }
    
    std::cout << "\nНажмите любую клавишу для возврата в главное меню...";
    uiManager.getCharImmediate();
}

void ScoreManager::processGameOver(int rounds) {
    #ifdef _WIN32
    system("cls");
    #else
    system("clear");
    #endif
    
    std::cout << "\n\n";
    std::cout << "               ИГРА ОКОНЧЕНА               " << std::endl;
    std::cout << "------------------------------------------------" << std::endl;
    std::cout << "\nВаш счет: " << currentScore << std::endl;
    std::cout << "Пройдено раундов: " << rounds << std::endl;
    
    std::cout << "\nХотите сохранить свой результат? Введите имя (или нажмите Enter для отмены): ";
    std::string playerName;
    std::getline(std::cin, playerName);
    
    if (!playerName.empty()) {
        if (saveScore(playerName, rounds)) {
            std::cout << "\nРезультат успешно сохранен!" << std::endl;
        } else {
            std::cout << "\nНе удалось сохранить результат." << std::endl;
        }
        
        #ifdef _WIN32
        Sleep(2000);
        #else
        usleep(2000000);
        #endif
    }
    
    // После сохранения (или отказа от сохранения) показываем таблицу лидеров
    displayLeaderboard();
}

bool ScoreManager::saveGame(const Character& player, int currentRound, const std::string& saveName) {
    // Создаем структуру с данными для сохранения
    GameSaveData saveData;
    saveData.playerName = saveName;
    saveData.playerHealth = player.getHealth();
    saveData.playerMaxHealth = player.getMaxHealth();
    saveData.playerAttack = player.getAttack();
    saveData.playerDefense = player.getDefense();
    saveData.playerMana = player.getMana();
    saveData.playerMaxMana = player.getMaxMana();
    saveData.playerGold = player.getGold();
    saveData.currentRound = currentRound;
    saveData.currentScore = currentScore;
    
    // Сохраняем названия предметов в инвентаре
    const auto& inventory = player.getInventory();
    for (const auto& item : inventory) {
        saveData.inventory.push_back(item.name);
    }
    
    // Загружаем существующие сохранения
    std::vector<GameSaveData> saveGames;
    std::ifstream inFile(saveGameFile, std::ios::binary);
    if (inFile.is_open()) {
        // Читаем количество сохранений
        int saveCount = 0;
        inFile.read(reinterpret_cast<char*>(&saveCount), sizeof(saveCount));
        
        for (int i = 0; i < saveCount; i++) {
            GameSaveData existingSave;
            
            // Чтение имени (размер строки + данные)
            int nameLength = 0;
            inFile.read(reinterpret_cast<char*>(&nameLength), sizeof(nameLength));
            existingSave.playerName.resize(nameLength);
            inFile.read(&existingSave.playerName[0], nameLength);
            
            // Чтение основных параметров
            inFile.read(reinterpret_cast<char*>(&existingSave.playerHealth), sizeof(existingSave.playerHealth));
            inFile.read(reinterpret_cast<char*>(&existingSave.playerMaxHealth), sizeof(existingSave.playerMaxHealth));
            inFile.read(reinterpret_cast<char*>(&existingSave.playerAttack), sizeof(existingSave.playerAttack));
            inFile.read(reinterpret_cast<char*>(&existingSave.playerDefense), sizeof(existingSave.playerDefense));
            inFile.read(reinterpret_cast<char*>(&existingSave.playerMana), sizeof(existingSave.playerMana));
            inFile.read(reinterpret_cast<char*>(&existingSave.playerMaxMana), sizeof(existingSave.playerMaxMana));
            inFile.read(reinterpret_cast<char*>(&existingSave.playerGold), sizeof(existingSave.playerGold));
            inFile.read(reinterpret_cast<char*>(&existingSave.currentRound), sizeof(existingSave.currentRound));
            inFile.read(reinterpret_cast<char*>(&existingSave.currentScore), sizeof(existingSave.currentScore));
            
            // Чтение инвентаря
            int inventorySize = 0;
            inFile.read(reinterpret_cast<char*>(&inventorySize), sizeof(inventorySize));
            
            for (int j = 0; j < inventorySize; j++) {
                int itemNameLength = 0;
                inFile.read(reinterpret_cast<char*>(&itemNameLength), sizeof(itemNameLength));
                
                std::string itemName;
                itemName.resize(itemNameLength);
                inFile.read(&itemName[0], itemNameLength);
                
                existingSave.inventory.push_back(itemName);
            }
            
            saveGames.push_back(existingSave);
        }
        
        inFile.close();
    }
    
    // Проверяем, есть ли уже сохранение с таким именем
    bool saveExists = false;
    for (size_t i = 0; i < saveGames.size(); i++) {
        if (saveGames[i].playerName == saveName) {
            saveGames[i] = saveData; // Заменяем существующее сохранение
            saveExists = true;
            break;
        }
    }
    
    // Если сохранения с таким именем нет, добавляем новое
    if (!saveExists) {
        saveGames.push_back(saveData);
    }
    
    // Если сохранений больше максимума, удаляем самое старое
    if (saveGames.size() > maxSaveGames) {
        saveGames.erase(saveGames.begin());
    }
    
    // Записываем все сохранения в файл
    std::ofstream outFile(saveGameFile, std::ios::binary);
    if (!outFile.is_open()) {
        return false;
    }
    
    // Записываем количество сохранений
    int saveCount = static_cast<int>(saveGames.size());
    outFile.write(reinterpret_cast<const char*>(&saveCount), sizeof(saveCount));
    
    for (const auto& save : saveGames) {
        // Запись имени (размер строки + данные)
        int nameLength = static_cast<int>(save.playerName.size());
        outFile.write(reinterpret_cast<const char*>(&nameLength), sizeof(nameLength));
        outFile.write(save.playerName.c_str(), nameLength);
        
        // Запись основных параметров
        outFile.write(reinterpret_cast<const char*>(&save.playerHealth), sizeof(save.playerHealth));
        outFile.write(reinterpret_cast<const char*>(&save.playerMaxHealth), sizeof(save.playerMaxHealth));
        outFile.write(reinterpret_cast<const char*>(&save.playerAttack), sizeof(save.playerAttack));
        outFile.write(reinterpret_cast<const char*>(&save.playerDefense), sizeof(save.playerDefense));
        outFile.write(reinterpret_cast<const char*>(&save.playerMana), sizeof(save.playerMana));
        outFile.write(reinterpret_cast<const char*>(&save.playerMaxMana), sizeof(save.playerMaxMana));
        outFile.write(reinterpret_cast<const char*>(&save.playerGold), sizeof(save.playerGold));
        outFile.write(reinterpret_cast<const char*>(&save.currentRound), sizeof(save.currentRound));
        outFile.write(reinterpret_cast<const char*>(&save.currentScore), sizeof(save.currentScore));
        
        // Запись инвентаря
        int inventorySize = static_cast<int>(save.inventory.size());
        outFile.write(reinterpret_cast<const char*>(&inventorySize), sizeof(inventorySize));
        
        for (const auto& itemName : save.inventory) {
            int itemNameLength = static_cast<int>(itemName.size());
            outFile.write(reinterpret_cast<const char*>(&itemNameLength), sizeof(itemNameLength));
            outFile.write(itemName.c_str(), itemNameLength);
        }
    }
    
    outFile.close();
    return true;
}

bool ScoreManager::loadGame(Character& player, int& currentRound, std::string& playerName) {
    std::ifstream inFile(saveGameFile, std::ios::binary);
    if (!inFile.is_open()) {
        return false;
    }
    
    // Отображаем меню загрузки и получаем выбор пользователя
    displayLoadGameMenu();
    int choice = uiManager.getPlayerChoice();
    
    if (choice <= 0) {
        inFile.close();
        return false;
    }
    
    // Считываем количество сохранений
    int saveCount = 0;
    inFile.read(reinterpret_cast<char*>(&saveCount), sizeof(saveCount));
    
    if (choice > saveCount) {
        inFile.close();
        return false;
    }
    
    // Пропускаем ненужные сохранения
    for (int i = 1; i < choice; i++) {
        GameSaveData skipSave;
        
        // Пропускаем имя
        int nameLength = 0;
        inFile.read(reinterpret_cast<char*>(&nameLength), sizeof(nameLength));
        inFile.seekg(nameLength, std::ios::cur);
        
        // Пропускаем основные параметры
        inFile.seekg(sizeof(skipSave.playerHealth) + 
                     sizeof(skipSave.playerMaxHealth) + 
                     sizeof(skipSave.playerAttack) + 
                     sizeof(skipSave.playerDefense) + 
                     sizeof(skipSave.playerMana) + 
                     sizeof(skipSave.playerMaxMana) + 
                     sizeof(skipSave.playerGold) + 
                     sizeof(skipSave.currentRound) + 
                     sizeof(skipSave.currentScore), std::ios::cur);
        
        // Пропускаем инвентарь
        int inventorySize = 0;
        inFile.read(reinterpret_cast<char*>(&inventorySize), sizeof(inventorySize));
        
        for (int j = 0; j < inventorySize; j++) {
            int itemNameLength = 0;
            inFile.read(reinterpret_cast<char*>(&itemNameLength), sizeof(itemNameLength));
            inFile.seekg(itemNameLength, std::ios::cur);
        }
    }
    
    // Читаем выбранное сохранение
    GameSaveData loadData;
    
    // Читаем имя
    int nameLength = 0;
    inFile.read(reinterpret_cast<char*>(&nameLength), sizeof(nameLength));
    loadData.playerName.resize(nameLength);
    inFile.read(&loadData.playerName[0], nameLength);
    
    // Читаем основные параметры
    inFile.read(reinterpret_cast<char*>(&loadData.playerHealth), sizeof(loadData.playerHealth));
    inFile.read(reinterpret_cast<char*>(&loadData.playerMaxHealth), sizeof(loadData.playerMaxHealth));
    inFile.read(reinterpret_cast<char*>(&loadData.playerAttack), sizeof(loadData.playerAttack));
    inFile.read(reinterpret_cast<char*>(&loadData.playerDefense), sizeof(loadData.playerDefense));
    inFile.read(reinterpret_cast<char*>(&loadData.playerMana), sizeof(loadData.playerMana));
    inFile.read(reinterpret_cast<char*>(&loadData.playerMaxMana), sizeof(loadData.playerMaxMana));
    inFile.read(reinterpret_cast<char*>(&loadData.playerGold), sizeof(loadData.playerGold));
    inFile.read(reinterpret_cast<char*>(&loadData.currentRound), sizeof(loadData.currentRound));
    inFile.read(reinterpret_cast<char*>(&loadData.currentScore), sizeof(loadData.currentScore));
    
    // Читаем инвентарь
    int inventorySize = 0;
    inFile.read(reinterpret_cast<char*>(&inventorySize), sizeof(inventorySize));
    
    std::vector<std::string> loadedItems;
    for (int j = 0; j < inventorySize; j++) {
        int itemNameLength = 0;
        inFile.read(reinterpret_cast<char*>(&itemNameLength), sizeof(itemNameLength));
        
        std::string itemName;
        itemName.resize(itemNameLength);
        inFile.read(&itemName[0], itemNameLength);
        
        loadedItems.push_back(itemName);
    }
    
    inFile.close();
    
    // Применяем загруженные данные к игроку
    // Создаем нового персонажа с загруженными параметрами
    player = Character("Герой", 
                      loadData.playerMaxHealth, 
                      loadData.playerDefense, 
                      loadData.playerMaxMana, 
                      true, 
                      loadData.playerAttack);
                      
    player.takeDamage(loadData.playerMaxHealth - loadData.playerHealth); // Устанавливаем текущее здоровье
    player.setGold(loadData.playerGold);

    // Восстанавливаем инвентарь
    for (const auto& itemName : loadedItems) {
        std::string description = "Предмет из сохраненной игры";
        player.addToInventory(Item(itemName, description));
    }
    
    // Устанавливаем текущий раунд и счет
    currentRound = loadData.currentRound;
    currentScore = loadData.currentScore;
    
    // Возвращаем имя игрока
    playerName = loadData.playerName;
    
    return true;
}

void ScoreManager::displaySaveGameMenu(const Character& player, int currentRound) {
    uiManager.clearScreen();
    std::cout << "\n\n";
    std::cout << "            СОХРАНЕНИЕ ИГРЫ            " << std::endl;
    std::cout << "----------------------------------------" << std::endl;
    std::cout << "\nТекущие характеристики персонажа:" << std::endl;
    std::cout << "Здоровье: " << player.getHealth() << "/" << player.getMaxHealth() << std::endl;
    std::cout << "Атака: " << player.getAttack() << std::endl;
    std::cout << "Защита: " << player.getDefense() << std::endl;
    std::cout << "Мана: " << player.getMana() << "/" << player.getMaxMana() << std::endl;
    std::cout << "Золото: " << player.getGold() << std::endl;
    std::cout << "Текущий раунд: " << currentRound << std::endl;
    std::cout << "Очки: " << currentScore << std::endl;
    
    std::cout << "\nВведите название для сохранения (или Enter для отмены): ";
    std::string saveName;
    std::getline(std::cin, saveName);
    
    if (!saveName.empty()) {
        if (saveGame(player, currentRound, saveName)) {
            std::cout << "\nИгра успешно сохранена!" << std::endl;
        } else {
            std::cout << "\nНе удалось сохранить игру." << std::endl;
        }
        
        #ifdef _WIN32
        Sleep(2000);
        #else
        usleep(2000000);
        #endif
    }
}

void ScoreManager::displayLoadGameMenu() {
    uiManager.clearScreen();
    std::cout << "\n\n";
    std::cout << "            ЗАГРУЗКА ИГРЫ            " << std::endl;
    std::cout << "--------------------------------------" << std::endl;
    
    std::ifstream inFile(saveGameFile, std::ios::binary);
    if (!inFile.is_open()) {
        std::cout << "\nСохранений не найдено." << std::endl;
        std::cout << "\nНажмите любую клавишу для возврата в главное меню...";
        uiManager.getCharImmediate();
        return;
    }
    
    // Считываем количество сохранений
    int saveCount = 0;
    inFile.read(reinterpret_cast<char*>(&saveCount), sizeof(saveCount));
    
    if (saveCount == 0) {
        std::cout << "\nСохранений не найдено." << std::endl;
        inFile.close();
        std::cout << "\nНажмите любую клавишу для возврата в главное меню...";
        uiManager.getCharImmediate();
        return;
    }
    
    std::cout << "\nДоступные сохранения:" << std::endl;
    
    for (int i = 0; i < saveCount; i++) {
        GameSaveData save;
        
        // Читаем имя
        int nameLength = 0;
        inFile.read(reinterpret_cast<char*>(&nameLength), sizeof(nameLength));
        save.playerName.resize(nameLength);
        inFile.read(&save.playerName[0], nameLength);
        
        // Читаем раунд и счет, пропуская остальные данные
        inFile.seekg(sizeof(save.playerHealth) + 
                     sizeof(save.playerMaxHealth) + 
                     sizeof(save.playerAttack) + 
                     sizeof(save.playerDefense) + 
                     sizeof(save.playerMana) + 
                     sizeof(save.playerMaxMana) + 
                     sizeof(save.playerGold), std::ios::cur);
                     
        inFile.read(reinterpret_cast<char*>(&save.currentRound), sizeof(save.currentRound));
        inFile.read(reinterpret_cast<char*>(&save.currentScore), sizeof(save.currentScore));
        
        // Пропускаем инвентарь
        int inventorySize = 0;
        inFile.read(reinterpret_cast<char*>(&inventorySize), sizeof(inventorySize));
        
        for (int j = 0; j < inventorySize; j++) {
            int itemNameLength = 0;
            inFile.read(reinterpret_cast<char*>(&itemNameLength), sizeof(itemNameLength));
            inFile.seekg(itemNameLength, std::ios::cur);
        }
        
        // Выводим информацию о сохранении
        std::cout << (i + 1) << ". " << save.playerName 
                  << " (Раунд: " << save.currentRound 
                  << ", Очки: " << save.currentScore << ")" << std::endl;
    }
    
    inFile.close();
    
    std::cout << "\n0. Отмена" << std::endl;
    std::cout << "\nВыберите сохранение (0-" << saveCount << "): ";
}

bool ScoreManager::hasSavedGames() const {
    std::ifstream inFile(saveGameFile, std::ios::binary);
    if (!inFile.is_open()) {
        return false;
    }
    
    int saveCount = 0;
    inFile.read(reinterpret_cast<char*>(&saveCount), sizeof(saveCount));
    inFile.close();
    
    return (saveCount > 0);
} 