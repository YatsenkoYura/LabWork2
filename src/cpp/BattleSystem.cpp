#include "../h/BattleSystem.h"
#include "../h/UIManager.h"
#include "../h/AIController.h"
#include "../h/InventorySystem.h"
#include <iostream>
#include <cstdlib>
#include <vector>
#include <random>

BattleSystem::BattleSystem(UIManager& uiManager, AIController& ai) 
    : uiManager(uiManager), 
      aiController(ai), 
      buffSystem(uiManager),
      potionSystem(uiManager, buffSystem),
      playerStunned(false), 
      playerStunDuration(0), 
      dialoguePauseDuration(2000) {
}

void BattleSystem::pauseForDialogue() const {
    #ifdef _WIN32
    Sleep(dialoguePauseDuration);
    #else
    usleep(dialoguePauseDuration * 1000);
    #endif
}

void BattleSystem::setDialoguePauseDuration(int milliseconds) {
    if (milliseconds >= 0) {
        dialoguePauseDuration = milliseconds;
    }
}

int BattleSystem::getDialoguePauseDuration() const {
    return dialoguePauseDuration;
}

void BattleSystem::startBattle(Character& player, Character& enemy, int round) {
    bool battleContinues = true;
    
    playerStunned = false;
    playerStunDuration = 0;
    
    buffSystem.clearBuffs();
    
    EnemyAttack nextEnemyAction;
    bool hasNextAction = false;
    
    int playerLastChoice = 0;
    int playerLastSpell = 0;

    if (round == 1) {
        aiController.setFirstRoundBehavior();
    }
    
    uiManager.clearScreen();
    std::cout << "\n\n";
    std::cout << "           НАЧАЛО БОЯ: РАУНД " << round << "           " << std::endl;
    std::cout << "------------------------------------------------" << std::endl << std::endl;
    
    std::vector<std::string> introductoryPhrases = {
        "Ну что, готов проверить, насколько ты слаб? *ухмыляется*",
        "Наконец-то достойный соперник... Или нет? *оценивающе смотрит*",
        "Давненько я не разминался. Надеюсь, ты продержишься дольше минуты! *разминает плечи*",
        "Тренировки в подземелье не прошли зря. Покажи, на что ты способен! *принимает боевую стойку*",
        "Я вижу в твоих глазах страх. Правильно, бойся! *насмешливо улыбается*"
    };
    
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distrib(0, introductoryPhrases.size() - 1);
    
    std::cout << "--------------------------------------------------------\n";
    std::cout << enemy.getName() << ": \"" << introductoryPhrases[distrib(gen)] << "\"\n";
    std::cout << "--------------------------------------------------------\n";
    pauseForDialogue();
    
    std::cout << "\nНажмите любую клавишу для начала боя...";
    uiManager.getCharImmediate();
    
    nextEnemyAction = aiController.determineNextAction(enemy, round, true);
    hasNextAction = true;
    
    while (battleContinues) {
        uiManager.displayBattleScreen(player, enemy);
        
        displayEnemyAttackStats();
        
        if (hasNextAction) {
            displayEnemyHint(nextEnemyAction);
        }
        
        if (playerStunned) {
            std::cout << "\nВы оглушены и пропускаете ход!" << std::endl;
            
            playerStunDuration--;
            if (playerStunDuration <= 0) {
                playerStunned = false;
            }
            
        } else {
            int choice = uiManager.getPlayerChoice();
            playerLastChoice = choice;
            
            switch(choice) {
                case 1:
                    {
                        int playerDamage = calculateDamage(player, enemy);
                        enemy.takeDamage(playerDamage);
                        std::cout << "\nВы атакуете " << enemy.getName() << "!" << std::endl;
                        
                        pauseForDialogue();
                        
                        if (!enemy.isAlive()) {
                            std::cout << "Вы победили " << enemy.getName() << "!" << std::endl;
                            pauseForDialogue();
                            battleContinues = false;
                            continue;
                        }
                    }
                    break;
                    
                case 2: {
                    uiManager.displayMagicOptions(player);
                    int spellChoice = uiManager.getPlayerChoice();
                    playerLastChoice = 2;
                    playerLastSpell = spellChoice;
                    
                    if (spellChoice >= 1 && spellChoice <= 3) {
                        bool magicUsed = castMagic(player, enemy, spellChoice);
                        
                        if (!magicUsed) {
                            continue;
                        }
                        
                        if (!enemy.isAlive()) {
                            std::cout << "Вы победили " << enemy.getName() << "!" << std::endl;
                            pauseForDialogue();
                            battleContinues = false;
                            continue;
                        }
                    } else {
                        continue;
                    }
                    break;
                }
                    
                case 3:
                    useItem(player, enemy);
                    continue;
                    
                case 4:
                    exitGame();
                    continue;
                    
                default:
                    std::cout << "Недопустимый выбор. Попробуйте еще раз." << std::endl;
                    pauseForDialogue();
                    continue;
            }
        }
        
        if (enemy.isAlive()) {
            EnemyAttack enemyAction;
            
            if (hasNextAction) {
                enemyAction = nextEnemyAction;
                hasNextAction = false;
                
                aiController.determineNextAction(enemy, round, false);
            } else {
                enemyAction = aiController.determineNextAction(enemy, round, false);
            }
            
            executeEnemyAction(player, enemy, enemyAction, playerLastChoice, playerLastSpell);
            
            if (!player.isAlive()) {
                std::cout << "Вы проиграли! " << enemy.getName() << " победил вас!" << std::endl;
                pauseForDialogue();
                battleContinues = false;
            } else {
                nextEnemyAction = aiController.determineNextAction(enemy, round, true);
                hasNextAction = true;
            }
            
            updateBuffs(player);
        }
    }
    
    std::string result = (player.isAlive()) ? "Вы победили!" : "Вы проиграли!";
    uiManager.displayBattleResult(result);
}

void BattleSystem::executeEnemyAction(Character& player, Character& enemy, const EnemyAttack& action, int playerLastChoice, int playerLastSpell) {
    std::vector<std::string> normalAttackPhrases = {
        "Получай! *наносит прямой удар*",
        "Этот удар ты почувствуешь! *проводит резкую атаку*",
        "Мой меч жаждет твоей крови! *выполняет быстрый замах*",
        "Давай проверим твою защиту! *наносит сильный удар*",
        "Слишком медленно! *мгновенно сокращает дистанцию*",
        "Никуда не денешься! *преследует противника*",
        "Сила важнее техники! *вкладывает вес в удар*",
        "Это только разминка! *выполняет серию быстрых ударов*",
        "Чувствуешь разницу в нашей силе? *атакует с яростью*",
        "Защищайся, если сможешь! *наступает с мощной атакой*"
    };
    
    std::vector<std::string> dodgeAttackPhrases = {
        "Слишком предсказуемо! *легко уходит от вашей атаки*",
        "Я читаю твои движения как открытую книгу! *уклоняется с усмешкой*",
        "Даже не близко! *совершает изящный маневр*",
        "Ты слишком медленный для меня! *двигается как тень*",
        "Вот так надо двигаться! *демонстрирует превосходную технику*",
        "Не попал! *насмехается над вашими попытками*",
        "Мои рефлексы отточены годами! *молниеносно уклоняется*",
        "Впечатляюще... для новичка! *издевательски смеется*",
        "Наблюдай и учись! *выполняет невероятно сложное уклонение*",
        "Ты никогда меня не поймаешь! *скользит вокруг вас*"
    };
    
    std::vector<std::string> stunAttackPhrases = {
        "Спокойной ночи! *наносит сокрушительный удар*",
        "Отдохни немного! *атакует в уязвимую точку*",
        "Не вставай, тебе будет больно! *нацеливается на голову*",
        "Как тебе такое? *бьет с невероятной силой*",
        "Это называется техникой! *проводит точный удар*",
        "Не всё так просто, правда? *улыбается, видя вас оглушенным*",
        "Неожиданно, да? *наблюдает за вашим замешательством*",
        "Вот это я называю ударом! *выглядит довольным*",
        "Больно? Это только начало! *готовится к следующей атаке*",
        "Ты еще долго будешь это чувствовать! *смотрит на вас сверху вниз*"
    };
    
    std::vector<std::string> playerBlockedStunPhrases = {
        "Неплохо... для новичка! *недовольно хмурится*",
        "Так ты все-таки кое-что умеешь! *выглядит заинтересованным*",
        "Счастливый случай! *отходит назад*",
        "Не думай, что это повторится! *меняет тактику*",
        "Щит? Серьезно? *выглядит разочарованным*",
        "Хм, интересно... *оценивает вашу защиту*",
        "Редко кто блокирует этот удар! *удивленно поднимает бровь*",
        "Везучий! В следующий раз не получится! *обещает с угрозой*",
        "Хорошо сработано... но недостаточно! *готовится к новой атаке*",
        "Впечатляет! Но мой арсенал только начинается! *широко улыбается*"
    };
    
    std::vector<std::string> playerDodgedPhrases = {
        "Шустрый, ничего не скажешь! *выглядит раздраженно*",
        "Повезло! *пытается скрыть удивление*",
        "Телепортация? Нечестно! *возмущается*",
        "А ты не так прост! *перестраивает тактику*",
        "Интересный трюк... *изучает вашу технику*",
        "Я всё равно тебя достану! *замахивается для новой атаки*",
        "Думаешь, это поможет? *смеётся*",
        "Нельзя вечно бегать! *преследует вас*",
        "Неожиданно... но бесполезно! *атакует снова*",
        "Учишься на ходу? Похвально! *кивает с одобрением*"
    };
    
    std::vector<std::string> skipTurnPhrases = {
        "Нужно перевести дыхание... *тяжело дышит*",
        "Я позволю тебе атаковать. *отступает*",
        "Мне нужно немного времени. *перегруппировывается*",
        "Не думай, что я слаб! *готовится к следующему ходу*",
        "Я приберегу силы для решающего удара. *восстанавливает силы*",
        "Даже лучшим бойцам нужна передышка. *вытирает пот со лба*",
        "Следующий ход будет твоим последним! *собирается с силами*",
        "Момент тишины перед бурей... *медитирует*",
        "Отдыхай, пока можешь! *восстанавливает энергию*",
        "Стратегическое отступление... ненадолго. *переводит дыхание*"
    };
    
    std::random_device rd;
    std::mt19937 gen(rd());
    
    switch (action.type) {
        case EnemyAttackType::NORMAL_ATTACK:
            {
                std::uniform_int_distribution<> distrib(0, normalAttackPhrases.size() - 1);
                std::cout << enemy.getName() << ": \"" << normalAttackPhrases[distrib(gen)] << "\"" << std::endl;
                
                #ifdef _WIN32
                Sleep(dialoguePauseDuration / 2);
                #else
                usleep((dialoguePauseDuration / 2) * 1000);
                #endif
                
                int damage = action.power;
                player.takeDamage(damage);
                std::cout << enemy.getName() << " атакует вас!" << std::endl;
                
                pauseForDialogue();
            }
            break;
            
        case EnemyAttackType::DODGE_ATTACK:
            {
                bool dodged = (playerLastChoice == 2 && playerLastSpell == 3 && player.useMana(-20));
                
                if (dodged) {
                    std::vector<std::string> playerTeleportedPhrases = {
                        "Телепортация? Умный ход... *удивленно поднимает бровь*",
                        "Так это и есть твоя магия? Неплохо... *внимательно следит за вами*",
                        "Это заклинание телепортации! Редкое умение... *выглядит впечатленным*",
                        "Хм, телепортация! Интересная способность... *изучает ваше перемещение*",
                        "Так ты можешь исчезать и появляться? Необычно... *пытается предугадать ваше движение*",
                        "Мгновенное перемещение! Впечатляет... *перестраивает тактику*",
                        "Я слышал о таких заклинаниях... Но редко видел их в действии! *с интересом наблюдает*",
                        "Телепортация против моих атак? Умно... *признает эффективность*",
                        "Телепортация! Вот почему ты избежал моего удара! *понимает ваш трюк*",
                        "Твоя телепортация действительно работает против меня... *неохотно признает*"
                    };
                    
                    std::uniform_int_distribution<> distrib(0, playerTeleportedPhrases.size() - 1);
                    std::cout << enemy.getName() << ": \"" << playerTeleportedPhrases[distrib(gen)] << "\"" << std::endl;
                    
                    #ifdef _WIN32
                    Sleep(dialoguePauseDuration / 2);
                    #else
                    usleep((dialoguePauseDuration / 2) * 1000);
                    #endif
                    
                    int damage = action.power / 4;
                    player.takeDamage(damage);
                    std::cout << enemy.getName() << " пытается сбить вас с ног, но ваша телепортация срабатывает!" << std::endl;
                } else {
                    std::vector<std::string> playerFailedDodgePhrases = {
                        "Слишком медленно! *легко обходит вашу защиту*",
                        "Я читаю твои движения как открытую книгу! *наносит точный удар*",
                        "Даже не близко к защите! *успешно атакует*",
                        "Ты слишком предсказуем! *обманным маневром проводит атаку*",
                        "Вот так надо атаковать! *демонстрирует превосходную технику*",
                        "Не смог защититься? Ожидаемо! *насмехается над вашими попытками*",
                        "Мои рефлексы отточены годами! *проводит неожиданную атаку*",
                        "Впечатляюще... для новичка! *издевательски смеется*",
                        "Наблюдай и учись, как работает мастер! *выполняет сложную атаку*",
                        "Ты никогда не сможешь меня обмануть! *безошибочно находит брешь в защите*"
                    };
                    
                    std::uniform_int_distribution<> distrib(0, playerFailedDodgePhrases.size() - 1);
                    std::cout << enemy.getName() << ": \"" << playerFailedDodgePhrases[distrib(gen)] << "\" *смеется*" << std::endl;
                    
                    #ifdef _WIN32
                    Sleep(dialoguePauseDuration / 2);
                    #else
                    usleep((dialoguePauseDuration / 2) * 1000);
                    #endif
                    
                    int damage = action.power * 2;
                    player.takeDamage(damage);
                    std::cout << enemy.getName() << " застигает вас врасплох!" << std::endl;
                }
                
                pauseForDialogue();
            }
            break;
            
        case EnemyAttackType::STUN_ATTACK:
            {
                bool blocked = (playerLastChoice == 2 && player.useMana(-10));
                
                if (blocked) {
                    std::uniform_int_distribution<> distrib(0, playerBlockedStunPhrases.size() - 1);
                    std::cout << enemy.getName() << ": \"" << playerBlockedStunPhrases[distrib(gen)] << "\"" << std::endl;
                    
                    #ifdef _WIN32
                    Sleep(dialoguePauseDuration / 2);
                    #else
                    usleep((dialoguePauseDuration / 2) * 1000);
                    #endif
                    
                    int damage = action.power;
                    player.takeDamage(damage);
                    std::cout << enemy.getName() << " пытается оглушить вас, но ледяной щит блокирует эффект!" << std::endl;
                } else {
                    std::uniform_int_distribution<> distrib(0, stunAttackPhrases.size() - 1);
                    std::cout << enemy.getName() << ": \"" << stunAttackPhrases[distrib(gen)] << "\" *смеется над вами*" << std::endl;
                    
                    #ifdef _WIN32
                    Sleep(dialoguePauseDuration / 2);
                    #else
                    usleep((dialoguePauseDuration / 2) * 1000);
                    #endif
                    
                    int damage = action.power * 5;
                    player.takeDamage(damage);
                    playerStunned = true;
                    playerStunDuration = action.power;
                    std::cout << enemy.getName() << " оглушает вас мощным ударом!" << std::endl;
                    std::cout << "Вы оглушены и пропустите " << playerStunDuration << " ход(а)." << std::endl;
                }
                
                pauseForDialogue();
            }
            break;
            
        case EnemyAttackType::SKIP_TURN:
            std::uniform_int_distribution<> distrib(0, skipTurnPhrases.size() - 1);
            std::cout << enemy.getName() << ": \"" << skipTurnPhrases[distrib(gen)] << "\"" << std::endl;
            
            std::cout << enemy.getName() << " пропускает ход." << std::endl;
            
            pauseForDialogue();
            break;
    }
}

void BattleSystem::processPlayerTurn(Character& player, Character& enemy) {
    std::cout << "Доступные действия:" << std::endl;
    std::cout << "1. Атаковать оружием" << std::endl;
    std::cout << "2. Использовать магию" << std::endl;
    std::cout << "3. Использовать предмет" << std::endl;
    std::cout << "4. Информация о враге" << std::endl;
    std::cout << "5. Выйти из игры" << std::endl << std::endl;
    
    std::cout << "Ваш выбор (1-5): ";
    
    int choice = uiManager.getPlayerChoice();
    
    switch(choice) {
        case 1:
            attackEnemy(player, enemy);
            break;
        case 2: {
            uiManager.displayMagicOptions(player);
            int spellChoice = uiManager.getPlayerChoice();
            if (spellChoice >= 1 && spellChoice <= 3) {
                useMagic(player, enemy, spellChoice);
            }
            break;
        }
        case 3:
            useItem(player, enemy);
            break;
        case 4:
            showEnemyInfo(enemy);
            break;
        case 5:
            exitGame();
            break;
        default:
            std::cout << "Недопустимый выбор. Попробуйте еще раз." << std::endl;
            pauseForDialogue();
            break;
    }
}

void BattleSystem::attackEnemy(Character& player, Character& enemy) {
    int damage = calculateDamage(player, enemy);
    enemy.takeDamage(damage);
    
    std::cout << "Вы атакуете " << enemy.getName() << "!" << std::endl;
    
    std::vector<std::string> enemyResponseToAttack = {
        "Это всё, на что ты способен? *отряхивает пыль с плеча*",
        "Ты называешь это ударом? *насмешливо смотрит*",
        "Я даже не почувствовал! *делает вид, что зевает*",
        "Мои тренировки не прошли даром... *блокирует большую часть удара*",
        "Ха, в следующий раз попробуй сильнее! *потирает место удара*",
        "Это щекотка, а не атака! *смеется над вашей попыткой*",
        "Твой меч ощущается как зубочистка! *демонстративно отряхивается*",
        "Моя бабушка била сильнее! *показывает, что ему не больно*",
        "И ради этого ты так долго готовился? *смотрит с разочарованием*",
        "Смешная попытка, но недостаточно! *принимает боевую стойку*"
    };
    
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distrib(0, enemyResponseToAttack.size() - 1);
    
    std::cout << "\n" << enemy.getName() << ": \"" << enemyResponseToAttack[distrib(gen)] << "\"" << std::endl;
    
    pauseForDialogue();
}

void BattleSystem::useMagic(Character& player, Character& enemy, int spellChoice) {
    if (!player.doesHaveMana() || player.getMana() <= 0) {
        std::cout << "У вас недостаточно маны!" << std::endl;
        pauseForDialogue();
        return;
    }
    
    switch (spellChoice) {
        case 1:
            if (player.useMana(10)) {
                std::cout << "Вы создаете огненный шар!" << std::endl;
                int damage = 15 + player.getAttack();
                std::cout << "Огненный шар наносит " << damage << " урона!" << std::endl;
                enemy.takeDamage(damage);
                pauseForDialogue();
            } else {
                std::cout << "Недостаточно маны для огненного шара!" << std::endl;
                pauseForDialogue();
            }
            break;
            
        case 2:
            if (player.useMana(8)) {
                std::cout << "Вы создаете ледяной щит вокруг себя!" << std::endl;
                std::cout << "Ваша защита увеличена на 8!" << std::endl;
                player.boostDefense(8);
                pauseForDialogue();
            } else {
                std::cout << "Недостаточно маны для ледяного щита!" << std::endl;
                pauseForDialogue();
            }
            break;
            
        case 3:
            if (player.useMana(12)) {
                std::cout << "Вы телепортируетесь, избегая вражеской атаки!" << std::endl;
                std::cout << "Телепортация восстанавливает вам 15 здоровья!" << std::endl;
                player.heal(15);
                pauseForDialogue();
            } else {
                std::cout << "Недостаточно маны для телепортации!" << std::endl;
                pauseForDialogue();
            }
            break;
            
        default:
            std::cout << "Неизвестное заклинание!" << std::endl;
            pauseForDialogue();
            break;
    }
}

void BattleSystem::useItem(Character& player, Character& enemy) {
    const auto& inventory = player.getInventory();
    
    if (inventory.empty()) {
        std::cout << "У вас нет предметов для использования." << std::endl;
        pauseForDialogue();
        return;
    }
    
    std::cout << "\nДоступные предметы:" << std::endl;
    for (size_t i = 0; i < inventory.size(); i++) {
        std::cout << (i + 1) << ". " << inventory[i].name 
                  << " - " << inventory[i].description << std::endl;
    }
    std::cout << (inventory.size() + 1) << ". Вернуться назад" << std::endl;
    
    int choice = uiManager.getPlayerChoice();
    
    if (choice >= 1 && choice <= static_cast<int>(inventory.size())) {
        int itemIndex = choice - 1;
        const Item& selectedItem = inventory[itemIndex];
        
        // Проверяем, является ли предмет зельем медитации
        bool isMeditationPotion = (selectedItem.name == "Зелье медитации");
        
        // Используем систему зелий вместо прямой логики
        bool potionUsed = potionSystem.usePotion(player, selectedItem);
        
        // Если это было зелье медитации, устанавливаем оглушение
        if (potionUsed && isMeditationPotion) {
            playerStunned = true;
            playerStunDuration = 1;
        }
        
        if (potionUsed) {
            // Удаляем предмет из инвентаря
            InventorySystem inventorySystem(uiManager);
            inventorySystem.removeItem(player, itemIndex);
        }
        
        pauseForDialogue();
        return;
    }
    else if (choice == inventory.size() + 1) {
        return;
    }
    else {
        std::cout << "Неверный выбор. Попробуйте снова." << std::endl;
        pauseForDialogue();
    }
}

void BattleSystem::updateBuffs(Character& player) {
    // Используем BuffSystem вместо прямой логики обновления эффектов
    buffSystem.updateBuffs(player);
}

void BattleSystem::showEnemyInfo(const Character& enemy) {
    std::cout << "\nИнформация о противнике:\n" << std::endl;
    std::cout << "Имя: " << enemy.getName() << std::endl;
    std::cout << "Здоровье: " << enemy.getHealth() << std::endl;
    std::cout << "Атака: " << enemy.getAttack() << std::endl;
    std::cout << "Очки действий: " << enemy.getMana() << std::endl;
    
    pauseForDialogue();
}

int BattleSystem::calculateDamage(const Character& attacker, const Character& defender) {
    int baseDamage = attacker.getAttack();
    int defenderDefense = defender.getDefense();
    
    // Усиливаем базовый урон для более быстрой битвы
    int damage = baseDamage * 1.5 - defenderDefense;
    
    // Добавляем случайный элемент для неожиданных исходов
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> damageVariation(-2, 5);
    damage += damageVariation(gen);
    
    return (damage < 1) ? 1 : damage;
}

void BattleSystem::exitGame() {
    std::cout << "Вы уверены, что хотите выйти? (y/n): ";
    char choice = uiManager.getCharImmediate();
    
    if (choice == 'y' || choice == 'Y') {
        std::cout << "\nСпасибо за игру!" << std::endl;
        exit(0);
    }
}

void BattleSystem::clearScreen() {
    #ifdef _WIN32
    system("cls");
    #else
    system("clear");
    #endif
}

bool BattleSystem::castMagic(Character& player, Character& enemy, int spellChoice) {
    if (!player.doesHaveMana() || player.getMana() <= 0) {
        std::cout << "У вас недостаточно маны!" << std::endl;
        pauseForDialogue();
        return false;
    }
    
    std::vector<std::string> enemyResponseToFireball = {
        "Ты думаешь огонь меня остановит? Наивно! *отряхивает пепел*",
        "Неплохой фокус, но мне не жарко! *гасит пламя*",
        "Огненные шары? Как оригинально... *уворачивается от части атаки*",
        "Твоя магия лишь раззадоривает меня! *сквозь пламя улыбается*",
        "Огонь против меня? Весьма неразумно. *тушит тлеющую одежду*",
        "Огонь питает мой гнев! *выходит из пламени сильнее*",
        "Мне приходилось бывать и в более горячих ситуациях! *стряхивает искры*",
        "Этим пламенем ты только разжег мой боевой дух! *атмосфера накаляется*",
        "Впечатляюще, но недостаточно горячо! *проходит сквозь огонь*",
        "Я танцевал в вулканах, а ты меня огоньком пугаешь? *смеется*"
    };
    
    std::vector<std::string> enemyResponseToIceShield = {
        "Прячешься за льдом? Боишься меня? *оценивает барьер*",
        "Щит из льда не спасёт тебя надолго. *готовится к следующей атаке*",
        "Думаешь, эта преграда меня остановит? *стучит по льду*",
        "Интересная тактика... Посмотрим, насколько эффективная. *ищет слабое место*",
        "Даже за стеной льда ты не в безопасности! *примеряется для удара*",
        "Я растоплю твой ледяной барьер одним ударом! *готовится к атаке*",
        "Холод только делает меня сильнее! *не обращает внимания на мороз*",
        "Этот лед — не преграда, а хрупкая иллюзия защиты. *обходит вас*",
        "Ты можешь прятаться, но не убежишь! *изучает ваш щит*",
        "Зима придет и для твоего щита! *усмехается с холодным взглядом*"
    };
    
    std::vector<std::string> enemyResponseToTeleport = {
        "Бегство? Какая трусость! *ищет вас взглядом*",
        "Ты можешь бежать, но не спрячешься! *поворачивается в вашу сторону*",
        "Телепортация? Впечатляюще, но бесполезно. *улыбается, увидев вас*",
        "Не думал, что будешь убегать... *готовится к следующей атаке*",
        "Даже магия перемещения не спасёт тебя от меня! *следит за вашим перемещением*",
        "Быстрые ноги — но куда они тебя приведут? *впечатлен, но не смущен*",
        "Исчезаешь? Это искусство трусов! *презрительно смотрит*",
        "Я найду тебя в любом уголке арены! *внимательно следит за вами*",
        "Пространство не защитит тебя от моего гнева! *сужает глаза*",
        "Твои трюки с пространством меня только забавляют! *приближается к вам*"
    };
    
    std::random_device rd;
    std::mt19937 gen(rd());
    
    switch (spellChoice) {
        case 1:
            if (player.useMana(15)) {
                int damage = 15 + player.getAttack() / 2;
                enemy.takeDamage(damage);
                
                std::cout << "Вы создаете огненный шар и запускаете его в противника!" << std::endl;
                
                std::uniform_int_distribution<> distrib(0, enemyResponseToFireball.size() - 1);
                std::cout << "\n" << enemy.getName() << ": \"" << enemyResponseToFireball[distrib(gen)] << "\"" << std::endl;
                
                pauseForDialogue();
                
                return true;
            } else {
                std::cout << "Недостаточно маны для огненного шара!" << std::endl;
                pauseForDialogue();
                return false;
            }
            break;
            
        case 2:
            if (player.useMana(10)) {
                player.boostDefense(10);
                
                std::cout << "Вы создаете ледяной щит вокруг себя!" << std::endl;
                
                std::uniform_int_distribution<> distrib(0, enemyResponseToIceShield.size() - 1);
                std::cout << "\n" << enemy.getName() << ": \"" << enemyResponseToIceShield[distrib(gen)] << "\"" << std::endl;
                
                pauseForDialogue();
                
                return true;
            } else {
                std::cout << "Недостаточно маны для ледяного щита!" << std::endl;
                pauseForDialogue();
                return false;
            }
            break;
            
        case 3:
            if (player.useMana(20)) {
                player.heal(20);
                
                std::cout << "Вы телепортируетесь, избегая вражеской атаки и восстанавливая силы!" << std::endl;
                
                std::uniform_int_distribution<> distrib(0, enemyResponseToTeleport.size() - 1);
                std::cout << "\n" << enemy.getName() << ": \"" << enemyResponseToTeleport[distrib(gen)] << "\"" << std::endl;
                
                pauseForDialogue();
                
                return true;
            } else {
                std::cout << "Недостаточно маны для телепортации!" << std::endl;
                pauseForDialogue();
                return false;
            }
            break;
            
        default:
            std::cout << "Неизвестное заклинание!" << std::endl;
            pauseForDialogue();
            return false;
            break;
    }
    
    return false;
}

void BattleSystem::displayEnemyAttackStats() {
    std::vector<EnemyAttack> availableAttacks = aiController.getAvailableAttacks();
    
    int normalAttackCount = 0;
    int dodgeAttackCount = 0;
    int stunAttackCount = 0;
    
    for (const auto& attack : availableAttacks) {
        switch (attack.type) {
            case EnemyAttackType::NORMAL_ATTACK:
                normalAttackCount++;
                break;
            case EnemyAttackType::DODGE_ATTACK:
                dodgeAttackCount++;
                break;
            case EnemyAttackType::STUN_ATTACK:
                stunAttackCount++;
                break;
            default:
                break;
        }
    }
    
    std::cout << "\n------------------------------------------------" << std::endl;
    std::cout << "Оставшиеся атаки Дэна:" << std::endl;
    std::cout << "  • Обычные атаки: " << normalAttackCount << std::endl;
    std::cout << "  • Атаки с уклонением: " << dodgeAttackCount << std::endl;
    std::cout << "  • Оглушающие атаки: " << stunAttackCount << std::endl;
    std::cout << "------------------------------------------------" << std::endl;
}

void BattleSystem::displayEnemyHint(const EnemyAttack& nextAction) {
    std::vector<std::string> normalAttackHints = {
        "Дэн собирается с силами для простой атаки.",
        "Дэн принимает стандартную боевую стойку для атаки.",
        "Дэн готовится нанести прямой удар.",
        "Дэн примеряется для обычной атаки.",
        "Дэн сжимает кулаки, готовясь к стандартному удару.",
        "Дэн смотрит прямо на вас, готовясь атаковать в лоб.",
        "Вы видите, как Дэн готовит обычный, но мощный удар.",
        "Дэн поднимает оружие для прямой атаки.",
        "Выражение лица Дэна говорит о подготовке к стандартному выпаду.",
        "Дэн оценивает ваши слабые места для простой, но эффективной атаки."
    };
    
    std::vector<std::string> dodgeAttackHints = {
        "Дэн начинает странно двигаться вокруг вас. Похоже, он готовит обманный маневр.",
        "Дэн улыбается и кружит вокруг вас. Кажется, он готовит неожиданную атаку.",
        "Дэн делает обманные движения. Будьте готовы к хитрой атаке с уклонением.",
        "Дэн словно танцует вокруг вас. Телепортация может помочь уклониться от его атаки.",
        "Дэн фокусируется на вашем положении. Телепортация сейчас была бы кстати.",
        "Дэн делает ложные выпады, явно готовясь к атаке с внезапной сменой направления.",
        "Вы замечаете, как Дэн совершает странные боковые шаги. Телепортация будет хорошим выбором.",
        "Дэн смотрит не в ваши глаза, а на ваши ноги — классический признак атаки с обманом.",
        "Дэн снижает центр тяжести, готовясь к быстрому маневру. Телепортация может спасти вас.",
        "Дэн перебирает ногами по кругу, как хищник перед внезапным броском."
    };
    
    std::vector<std::string> stunAttackHints = {
        "Дэн замахивается для особенно мощного удара. Ледяной щит может защитить вас от оглушения.",
        "Дэн концентрирует силу для оглушающей атаки. Подумайте об использовании щита.",
        "Дэн поднимает руку для удара в голову. Без щита вы можете быть оглушены.",
        "Дэн готовит сильный удар, который может вас оглушить. Защита сейчас крайне важна.",
        "Дэн примеряется для удара в уязвимую точку. Ледяной щит будет очень кстати.",
        "Дэн сосредотачивается на вашей голове. Ледяной щит может предотвратить оглушение.",
        "Вы видите, как Дэн напрягает мышцы для особо сильного удара. Щит спасет вас от оглушения.",
        "Глаза Дэна сузились, он целится в определенную точку. Ледяной щит необходим сейчас.",
        "Дэн меняет хват на оружии — классический признак подготовки к оглушающему удару.",
        "Дэн делает глубокий вдох, концентрируясь на мощном ударе. Без щита вы будете оглушены."
    };
    
    std::vector<std::string> skipTurnHints = {
        "Дэн выглядит уставшим. Похоже, он пропустит следующий ход.",
        "Дэн тяжело дышит. Он явно истощен и будет восстанавливаться.",
        "Дэн отступает назад. Возможно, у вас есть шанс атаковать без ответа.",
        "Дэн выглядит ослабленным. Сейчас подходящий момент для активных действий.",
        "Дэн замедляется, собираясь с силами. У вас есть преимущество на следующий ход.",
        "Дэн потирает ушибленные места. Похоже, он временно не боеспособен.",
        "Вы замечаете, как Дэн тяжело опирается на одну ногу. Он выглядит изможденным.",
        "Дэн останавливается, чтобы перевести дыхание. Идеальное время для вашей атаки.",
        "Движения Дэна становятся медленными и неуклюжими. Он явно собирается отдохнуть.",
        "Дэн сбрасывает напряжение в мышцах. Он не готов к следующей атаке."
    };
    
    std::random_device rd;
    std::mt19937 gen(rd());
    
    std::cout << "\n";
    
    switch (nextAction.type) {
        case EnemyAttackType::NORMAL_ATTACK: {
            std::uniform_int_distribution<> distrib(0, normalAttackHints.size() - 1);
            std::cout << normalAttackHints[distrib(gen)] << std::endl;
            break;
        }
        case EnemyAttackType::DODGE_ATTACK: {
            std::uniform_int_distribution<> distrib(0, dodgeAttackHints.size() - 1);
            std::cout << dodgeAttackHints[distrib(gen)] << std::endl;
            break;
        }
        case EnemyAttackType::STUN_ATTACK: {
            std::uniform_int_distribution<> distrib(0, stunAttackHints.size() - 1);
            std::cout << stunAttackHints[distrib(gen)] << std::endl;
            break;
        }
        case EnemyAttackType::SKIP_TURN: {
            std::uniform_int_distribution<> distrib(0, skipTurnHints.size() - 1);
            std::cout << skipTurnHints[distrib(gen)] << std::endl;
            break;
        }
    }
    
    std::cout << "\n";
    
    pauseForDialogue();
}

void BattleSystem::executePlayerTurn(Character& player, Character& enemy, int playerChoice, int& playerLastChoice, int& playerLastSpell, int currentRound) {
    playerLastChoice = playerChoice;
    
    if (playerChoice == 1) {
        // обычная атака
        int playerDamage = calculateDamage(player, enemy);
        enemy.takeDamage(playerDamage);
        
        displayEnemyResponseToAttack(enemy, true, playerDamage);
        
    } else if (playerChoice == 2) {
        // магия
        uiManager.displayMagicOptions(player);
        int spellChoice = uiManager.getPlayerChoice();
        
        playerLastSpell = spellChoice;
        
        if (castMagic(player, enemy, spellChoice)) {
            displayEnemyResponseToDodge(enemy, (spellChoice == 3), playerLastSpell);
        }
        
    } else if (playerChoice == 3) {
        // предметы
        useItem(player, enemy);
        
    } else {
        // неверный ввод
        std::cout << "Неверный выбор! Вы пропускаете ход." << std::endl;
        pauseForDialogue();
    }
    
    EnemyAttack nextAction = aiController.determineNextAction(enemy, currentRound);
    executeEnemyAction(player, enemy, nextAction, playerLastChoice, playerLastSpell);
}

void BattleSystem::displayEnemyResponseToAttack(Character& enemy, bool successfulAttack, int damage) {
    std::vector<std::string> enemyResponseToAttack = {
        "Это всё, на что ты способен? *отряхивает пыль с плеча*",
        "Ты называешь это ударом? *насмешливо смотрит*",
        "Я даже не почувствовал! *делает вид, что зевает*",
        "Мои тренировки не прошли даром... *блокирует большую часть удара*",
        "Ха, в следующий раз попробуй сильнее! *потирает место удара*",
        "Это щекотка, а не атака! *смеется над вашей попыткой*",
        "Твой меч ощущается как зубочистка! *демонстративно отряхивается*",
        "Моя бабушка била сильнее! *показывает, что ему не больно*",
        "И ради этого ты так долго готовился? *смотрит с разочарованием*",
        "Смешная попытка, но недостаточно! *принимает боевую стойку*"
    };
    
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distrib(0, enemyResponseToAttack.size() - 1);
    
    std::cout << "\nВы нанесли " << damage << " урона!" << std::endl;
    std::cout << enemy.getName() << ": \"" << enemyResponseToAttack[distrib(gen)] << "\"" << std::endl;
    
    pauseForDialogue();
}

void BattleSystem::displayEnemyResponseToDodge(Character& enemy, bool playerDodged, int playerLastSpell) {
    if (playerDodged) {
        std::vector<std::string> enemyResponseToTeleport = {
            "Бегство? Какая трусость! *ищет вас взглядом*",
            "Ты можешь бежать, но не спрячешься! *поворачивается в вашу сторону*",
            "Телепортация? Впечатляюще, но бесполезно. *улыбается, увидев вас*",
            "Не думал, что будешь убегать... *готовится к следующей атаке*",
            "Даже магия перемещения не спасёт тебя от меня! *следит за вашим перемещением*",
            "Быстрые ноги — но куда они тебя приведут? *впечатлен, но не смущен*",
            "Исчезаешь? Это искусство трусов! *презрительно смотрит*",
            "Я найду тебя в любом уголке арены! *внимательно следит за вами*",
            "Пространство не защитит тебя от моего гнева! *сужает глаза*",
            "Твои трюки с пространством меня только забавляют! *приближается к вам*"
        };
        
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> distrib(0, enemyResponseToTeleport.size() - 1);
        
        std::cout << enemy.getName() << ": \"" << enemyResponseToTeleport[distrib(gen)] << "\"" << std::endl;
        pauseForDialogue();
    }
}

void BattleSystem::displayEnemyResponseToStun(Character& enemy, bool playerStunned) {
    if (playerStunned) {
        std::vector<std::string> stunSuccessResponses = {
            "Сладких снов! *смеется над вашей беспомощностью*",
            "Наслаждайся отдыхом! *готовится к следующей атаке*",
            "Ты даже защититься толком не можешь! *смотрит с презрением*",
            "Как это? Голова кружится? *насмешливо улыбается*",
            "Вот это я понимаю - эффективный удар! *гордо выпрямляется*",
            "Теперь ты знаешь, что такое настоящая боль! *наблюдает за вашим замешательством*",
            "И это всё? Я ожидал большего сопротивления! *разочарованно качает головой*",
            "Не волнуйся, это ненадолго... твоя боль только начинается! *зловеще ухмыляется*",
            "Мне даже стало немного жаль тебя. Ключевое слово - немного! *смеется*",
            "Вот так легко развеялись все твои надежды! *торжествующе смотрит*"
        };
        
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> distrib(0, stunSuccessResponses.size() - 1);
        
        std::cout << enemy.getName() << ": \"" << stunSuccessResponses[distrib(gen)] << "\"" << std::endl;
        pauseForDialogue();
    } else {
        std::vector<std::string> stunFailedResponses = {
            "Как тебе удалось устоять? *выглядит удивленным*",
            "Интересно... обычно этот удар сбивает с ног! *внимательно изучает вас*",
            "Должно быть, у тебя крепкая голова! *хмурится*",
            "Я недооценил тебя, но это временно! *перестраивает тактику*",
            "Необычная стойкость... *задумчиво смотрит*",
            "В следующий раз я ударю сильнее! *обещает с угрозой*",
            "Щит? Или просто удача? В любом случае, она скоро закончится! *сжимает кулаки*",
            "Мало кто выдерживает этот удар... *проявляет уважение*",
            "Возможно, ты не так слаб, как кажешься! *оценивающе смотрит*",
            "Впечатляюще... но это тебе не поможет! *готовится к новой атаке*"
        };
        
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> distrib(0, stunFailedResponses.size() - 1);
        
        std::cout << enemy.getName() << ": \"" << stunFailedResponses[distrib(gen)] << "\"" << std::endl;
        pauseForDialogue();
    }
} 