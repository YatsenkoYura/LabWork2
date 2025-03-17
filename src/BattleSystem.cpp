#include "BattleSystem.h"
#include "UIManager.h"
#include "AIController.h"
#include <iostream>
#include <cstdlib>
#include <vector>
#include <random>

BattleSystem::BattleSystem(UIManager& uiManager, AIController& ai) 
    : uiManager(uiManager), aiController(ai), playerStunned(false), playerStunDuration(0), dialoguePauseDuration(2000), activeBuffs() {
}

// Метод для создания паузы заданной длительности
void BattleSystem::pauseForDialogue() const {
    #ifdef _WIN32
    Sleep(dialoguePauseDuration);
    #else
    usleep(dialoguePauseDuration * 1000); // usleep принимает микросекунды
    #endif
}

// Методы для управления настройками пауз
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
    
    // Сбрасываем состояния оглушения
    playerStunned = false;
    playerStunDuration = 0;
    
    // Очищаем список активных баффов в начале боя
    activeBuffs.clear();
    
    // Подготовка для прогнозирования действий Дэна
    EnemyAttack nextEnemyAction;
    bool hasNextAction = false;
    
    // Для отслеживания последнего выбранного игроком действия и заклинания
    int playerLastChoice = 0;
    int playerLastSpell = 0; // Добавляем переменную для отслеживания выбранного заклинания

    // Если это первый раунд, устанавливаем специальное поведение для Дэна
    if (round == 1) {
        aiController.setFirstRoundBehavior();
    }
    
    // Отображаем информацию о начале боя
    uiManager.clearScreen();
    std::cout << "\n\n";
    std::cout << "           НАЧАЛО БОЯ: РАУНД " << round << "           " << std::endl;
    std::cout << "------------------------------------------------" << std::endl << std::endl;
    
    // Добавляем приветственную фразу Дэна в начале боя
    std::vector<std::string> introductoryPhrases = {
        "Ну что, готов проверить, насколько ты слаб? *ухмыляется*",
        "Наконец-то достойный соперник... Или нет? *оценивающе смотрит*",
        "Давненько я не разминался. Надеюсь, ты продержишься дольше минуты! *разминает плечи*",
        "Тренировки в подземелье не прошли зря. Покажи, на что ты способен! *принимает боевую стойку*",
        "Я вижу в твоих глазах страх. Правильно, бойся! *насмешливо улыбается*"
    };
    
    // Выбор случайной приветственной фразы
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distrib(0, introductoryPhrases.size() - 1);
    
    // Выводим приветственную фразу Дэна более наглядно
    std::cout << "--------------------------------------------------------\n";
    std::cout << enemy.getName() << ": \"" << introductoryPhrases[distrib(gen)] << "\"\n";
    std::cout << "--------------------------------------------------------\n";
    pauseForDialogue(); // Делаем паузу для чтения
    
    // Нажмите любую клавишу для продолжения
    std::cout << "\nНажмите любую клавишу для начала боя...";
    uiManager.getCharImmediate();
    
    // Определяем первое действие Дэна для подсказки сразу после начала боя
    nextEnemyAction = aiController.determineNextAction(enemy, round, true); // true - только предсказание
    hasNextAction = true; // Устанавливаем флаг, что у нас есть подсказка
    
    while (battleContinues) {
        // Отображаем боевой экран с информацией о персонажах
        uiManager.displayBattleScreen(player, enemy);
        
        // Отображаем статистику доступных атак Дэна
        displayEnemyAttackStats();
        
        // Если у нас есть следующее действие Дэна, показываем подсказку
        if (hasNextAction) {
            displayEnemyHint(nextEnemyAction);
        }
        
        // Проверяем, не оглушен ли игрок
        if (playerStunned) {
            std::cout << "\nВы оглушены и пропускаете ход!" << std::endl;
            
            // Уменьшаем длительность оглушения
            playerStunDuration--;
            if (playerStunDuration <= 0) {
                playerStunned = false;
            }
            
            // Пропускаем ход игрока, переходим к ходу противника
        } else {
            // Ход игрока
            int choice = uiManager.getPlayerChoice();
            playerLastChoice = choice; // Сохраняем выбор игрока
            
            switch(choice) {
                case 1: // Обычная атака
                    {
                        // Атака игрока
                        int playerDamage = calculateDamage(player, enemy);
                        enemy.takeDamage(playerDamage);
                        std::cout << "\nВы атакуете " << enemy.getName() << "!" << std::endl;
                        
                        // Пауза после сообщения об атаке
                        pauseForDialogue();
                        
                        // Проверяем, жив ли враг после атаки
                        if (!enemy.isAlive()) {
                            std::cout << "Вы победили " << enemy.getName() << "!" << std::endl;
                            pauseForDialogue();
                            battleContinues = false;
                            continue;
                        }
                    }
                    break;
                    
                case 2: { // Использование магии
                    uiManager.displayMagicOptions(player);
                    int spellChoice = uiManager.getPlayerChoice();
                    playerLastChoice = 2; // Сохраняем, что игрок использовал магию
                    playerLastSpell = spellChoice; // Сохраняем конкретное выбранное заклинание
                    
                    if (spellChoice >= 1 && spellChoice <= 3) {
                        // Игрок использует магию
                        bool magicUsed = castMagic(player, enemy, spellChoice);
                        
                        // Если магия не была использована или враг погиб, пропускаем ход противника
                        if (!magicUsed) {
                            continue;
                        }
                        
                        // Проверяем, жив ли враг после магии
        if (!enemy.isAlive()) {
                            std::cout << "Вы победили " << enemy.getName() << "!" << std::endl;
                            pauseForDialogue();
                            battleContinues = false;
                            continue;
                        }
                    } else {
                        // Если игрок выбрал возврат, пропускаем ход противника
            continue;
                    }
                    break;
                }
                    
                case 3: // Использование предмета
                    useItem(player, enemy);
                    continue; // Пропускаем ход противника, т.к. предмет не найден
                    
                case 4: // Выход из игры
                    exitGame();
                    continue; // Если игрок не вышел, продолжаем без хода противника
                    
                default:
                    std::cout << "Недопустимый выбор. Попробуйте еще раз." << std::endl;
                    pauseForDialogue();
                    continue; // Пропускаем ход противника при неправильном вводе
            }
        }
        
        // Ход противника (если мы дошли до этого места, значит ход игрока был успешным)
        if (enemy.isAlive()) {
            // Определяем следующее действие противника
            EnemyAttack enemyAction;
            
            // Если у нас уже есть спрогнозированное действие, используем его,
            // но при этом всё равно удаляем из списка доступных атак
            if (hasNextAction) {
                // Запоминаем тип и мощность атаки из предсказания
                EnemyAttackType attackType = nextEnemyAction.type;
                std::string attackName = nextEnemyAction.name;
                int attackPower = nextEnemyAction.power;
                int attackCost = nextEnemyAction.cost;
                
                // Сбрасываем флаг, что у нас есть подсказка
                hasNextAction = false;
                
                // Вызываем determineNextAction уже без флага предсказания,
                // чтобы атака была удалена из списка доступных
                enemyAction = aiController.determineNextAction(enemy, round);
                
                // Проверяем, соответствует ли настоящая атака предсказанной
                // Если нет, используем предсказанную атаку и вручную тратим ману
                if (enemyAction.type != attackType) {
                    enemyAction = {attackType, attackName, attackPower, attackCost};
                    enemy.useMana(attackCost);
                }
            } else {
                enemyAction = aiController.determineNextAction(enemy, round);
            }
            
            // Выполняем действие
            executeEnemyAction(player, enemy, enemyAction, playerLastChoice, playerLastSpell);
            
            // Проверяем, жив ли игрок после хода противника
            if (!player.isAlive()) {
                std::cout << "Вы проиграли! " << enemy.getName() << " победил вас!" << std::endl;
                pauseForDialogue();
                battleContinues = false;
            } else {
                // Определяем следующее действие противника для подсказки
                nextEnemyAction = aiController.determineNextAction(enemy, round, true); // true - только предсказание
                hasNextAction = true;
            }
            
            // Обновляем активные баффы в конце каждого хода
            updateBuffs(player);
        }
    }
    
    // Покажем конечное сообщение
    std::string result = (player.isAlive()) ? "Вы победили!" : "Вы проиграли!";
    uiManager.displayBattleResult(result);
}

void BattleSystem::executeEnemyAction(Character& player, Character& enemy, const EnemyAttack& action, int playerLastChoice, int playerLastSpell) {
    // Фразы для различных типов атак
    // Фразы Дэна при обычной атаке
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
    
    // Фразы Дэна при атаке с уклонением
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
    
    // Фразы Дэна при удачном оглушении игрока
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
    
    // Фразы Дэна при успешной блокировке оглушения игроком
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
    
    // Фразы Дэна при успешном уклонении игрока от его атаки
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
    
    // Фразы Дэна при пропуске хода
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
    
    // Выбор случайной фразы в зависимости от типа атаки
    std::random_device rd;
    std::mt19937 gen(rd());
    
    switch (action.type) {
        case EnemyAttackType::NORMAL_ATTACK:
            {
                // Выбор случайной фразы для обычной атаки
                std::uniform_int_distribution<> distrib(0, normalAttackPhrases.size() - 1);
                std::cout << enemy.getName() << ": \"" << normalAttackPhrases[distrib(gen)] << "\"" << std::endl;
                
                // Пауза с коротким таймером для фразы
                #ifdef _WIN32
                Sleep(dialoguePauseDuration / 2);
                #else
                usleep((dialoguePauseDuration / 2) * 1000);
                #endif
                
                // Обычная атака, наносит урон
                int damage = action.power;
                player.takeDamage(damage);
                std::cout << enemy.getName() << " атакует вас!" << std::endl;
                
                // Добавляем паузу после атаки
                pauseForDialogue();
            }
            break;
            
        case EnemyAttackType::DODGE_ATTACK:
            {
                // Атака на уклонение - наносит двойной урон, если игрок не использовал телепортацию
                // Проверяем, использовал ли игрок именно телепортацию (заклинание 3), а не любую магию
                bool dodged = (playerLastChoice == 2 && playerLastSpell == 3 && player.useMana(-20));
                
                if (dodged) {
                    // Фразы Дэна при успешном уклонении игрока с помощью телепортации
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
                    
                    // Выбор фразы при успешном использовании телепортации
                    std::uniform_int_distribution<> distrib(0, playerTeleportedPhrases.size() - 1);
                    std::cout << enemy.getName() << ": \"" << playerTeleportedPhrases[distrib(gen)] << "\"" << std::endl;
                    
                    // Пауза с коротким таймером для фразы
                    #ifdef _WIN32
                    Sleep(dialoguePauseDuration / 2);
                    #else
                    usleep((dialoguePauseDuration / 2) * 1000);
                    #endif
                    
                    // Игрок успешно уклонился
                    int damage = action.power / 4; // Сниженный урон
                    player.takeDamage(damage);
                    std::cout << enemy.getName() << " пытается сбить вас с ног, но ваша телепортация срабатывает!" << std::endl;
                } else {
                    // Общие фразы насмешки без упоминания телепортации, когда игрок не уклонился
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
                    
                    // Выбор фразы при неудачном уклонении игрока
                    std::uniform_int_distribution<> distrib(0, playerFailedDodgePhrases.size() - 1);
                    std::cout << enemy.getName() << ": \"" << playerFailedDodgePhrases[distrib(gen)] << "\" *смеется*" << std::endl;
                    
                    // Пауза с коротким таймером для фразы
                    #ifdef _WIN32
                    Sleep(dialoguePauseDuration / 2);
                    #else
                    usleep((dialoguePauseDuration / 2) * 1000);
                    #endif
                    
                    // Игрок не уклонился - получает двойной урон
                    int damage = action.power * 2;
                    player.takeDamage(damage);
                    std::cout << enemy.getName() << " застигает вас врасплох!" << std::endl;
                }
                
                // Добавляем паузу после атаки
                pauseForDialogue();
            }
            break;
            
        case EnemyAttackType::STUN_ATTACK:
            {
                // Атака-оглушение - можно заблокировать только ледяным щитом
                bool blocked = (playerLastChoice == 2 && player.useMana(-10)); // Проверяем, использовал ли игрок щит
                
                if (blocked) {
                    // Выбор фразы при успешном блокировании оглушения
                    std::uniform_int_distribution<> distrib(0, playerBlockedStunPhrases.size() - 1);
                    std::cout << enemy.getName() << ": \"" << playerBlockedStunPhrases[distrib(gen)] << "\"" << std::endl;
                    
                    // Пауза с коротким таймером для фразы
                    #ifdef _WIN32
                    Sleep(dialoguePauseDuration / 2);
                    #else
                    usleep((dialoguePauseDuration / 2) * 1000);
                    #endif
                    
                    // Игрок успешно заблокировал оглушение
                    int damage = action.power;
                    player.takeDamage(damage);
                    std::cout << enemy.getName() << " пытается оглушить вас, но ледяной щит блокирует эффект!" << std::endl;
                } else {
                    // Выбор фразы при успешном оглушении
                    std::uniform_int_distribution<> distrib(0, stunAttackPhrases.size() - 1);
                    std::cout << enemy.getName() << ": \"" << stunAttackPhrases[distrib(gen)] << "\" *смеется над вами*" << std::endl;
                    
                    // Пауза с коротким таймером для фразы
                    #ifdef _WIN32
                    Sleep(dialoguePauseDuration / 2);
                    #else
                    usleep((dialoguePauseDuration / 2) * 1000);
                    #endif
                    
                    // Игрок оглушен
                    int damage = action.power * 5;
                    player.takeDamage(damage);
                    playerStunned = true;
                    playerStunDuration = action.power;
                    std::cout << enemy.getName() << " оглушает вас мощным ударом!" << std::endl;
                    std::cout << "Вы оглушены и пропустите " << playerStunDuration << " ход(а)." << std::endl;
                }
                
                // Добавляем паузу после атаки
                pauseForDialogue();
            }
            break;
            
        case EnemyAttackType::SKIP_TURN:
            // Пропуск хода (нет действий)
            // Выбор случайной фразы
            std::uniform_int_distribution<> distrib(0, skipTurnPhrases.size() - 1);
            std::cout << enemy.getName() << ": \"" << skipTurnPhrases[distrib(gen)] << "\"" << std::endl;
            
            std::cout << enemy.getName() << " пропускает ход." << std::endl;
            
            // Добавляем паузу после фразы
            pauseForDialogue();
            break;
    }
}

void BattleSystem::processPlayerTurn(Character& player, Character& enemy) {
    // Отображаем доступные действия после боевого экрана
    std::cout << "Доступные действия:" << std::endl;
    std::cout << "1. Атаковать оружием" << std::endl;
    std::cout << "2. Использовать магию" << std::endl;
    std::cout << "3. Использовать предмет" << std::endl;
    std::cout << "4. Информация о враге" << std::endl;
    std::cout << "5. Выйти из игры" << std::endl << std::endl;
    
    std::cout << "Ваш выбор (1-5): ";
    
    int choice = uiManager.getPlayerChoice();
    
    switch(choice) {
        case 1: // Обычная атака
            attackEnemy(player, enemy);
            break;
        case 2: { // Использование магии - добавляем блок скоупа
            uiManager.displayMagicOptions(player);
            int spellChoice = uiManager.getPlayerChoice();
            if (spellChoice >= 1 && spellChoice <= 3) {
                useMagic(player, enemy, spellChoice);
            }
            break;
        }
        case 3: // Использование предмета
            useItem(player, enemy);
            break;
        case 4: // Информация о враге
            showEnemyInfo(enemy);
            break;
        case 5: // Выход из игры
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
    
    // Фразы Дэна в ответ на атаку игрока
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
    
    // Выбор случайной фразы
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distrib(0, enemyResponseToAttack.size() - 1);
    
    // Выводим фразу противника
    std::cout << "\n" << enemy.getName() << ": \"" << enemyResponseToAttack[distrib(gen)] << "\"" << std::endl;
    
    // Добавляем паузу после фразы
    pauseForDialogue();
}

void BattleSystem::useMagic(Character& player, Character& enemy, int spellChoice) {
    // Проверяем, есть ли у игрока мана
    if (!player.doesHaveMana() || player.getMana() <= 0) {
        std::cout << "У вас недостаточно маны!" << std::endl;
        pauseForDialogue();
        return;
    }
    
    switch (spellChoice) {
        case 1: // Огненный шар
            if (player.useMana(15)) {
                std::cout << "Вы создаете огненный шар!" << std::endl;
                // Используем атаку персонажа для увеличения урона
                int damage = 15 + player.getAttack() / 2;
                std::cout << "Огненный шар наносит " << damage << " урона!" << std::endl;
                enemy.takeDamage(damage);
                pauseForDialogue();
            } else {
                std::cout << "Недостаточно маны для огненного шара!" << std::endl;
                pauseForDialogue();
            }
            break;
            
        case 2: // Ледяной щит
            if (player.useMana(10)) {
                std::cout << "Вы создаете ледяной щит вокруг себя!" << std::endl;
                std::cout << "Ваша защита увеличена на 10!" << std::endl;
                player.boostDefense(10);
                pauseForDialogue();
            } else {
                std::cout << "Недостаточно маны для ледяного щита!" << std::endl;
                pauseForDialogue();
            }
            break;
            
        case 3: // Телепортация
            if (player.useMana(20)) {
                std::cout << "Вы телепортируетесь, избегая вражеской атаки!" << std::endl;
                // Добавляем лечение 20 HP при телепортации
                std::cout << "Телепортация восстанавливает вам 20 здоровья!" << std::endl;
                player.heal(20);
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

// Метод использования предмета
void BattleSystem::useItem(Character& player, Character& enemy) {
    // Получаем инвентарь игрока
    const auto& inventory = player.getInventory();
    
    // Проверяем, есть ли у игрока предметы
    if (inventory.empty()) {
        std::cout << "У вас нет предметов для использования." << std::endl;
        pauseForDialogue();
        return;
    }
    
    // Отображаем инвентарь
    std::cout << "\nДоступные предметы:" << std::endl;
    for (size_t i = 0; i < inventory.size(); i++) {
        std::cout << (i + 1) << ". " << inventory[i].name 
                  << " - " << inventory[i].description << std::endl;
    }
    std::cout << (inventory.size() + 1) << ". Вернуться назад" << std::endl;
    
    // Получаем выбор игрока
    int choice = uiManager.getPlayerChoice();
    
    // Проверяем, что выбор валидный
    if (choice >= 1 && choice <= static_cast<int>(inventory.size())) {
        // Индекс выбранного предмета
        int itemIndex = choice - 1;
        const Item& selectedItem = inventory[itemIndex];
        
        // Применяем эффект предмета
        if (selectedItem.name.find("зелье здоровья") != std::string::npos) {
            // Извлекаем число из описания
            std::string valueStr = selectedItem.description.substr(
                selectedItem.description.find_last_of(' ') + 1, 
                selectedItem.description.find("HP") - selectedItem.description.find_last_of(' ') - 1
            );
            int healValue = std::stoi(valueStr);
            
            // Применяем эффект зелья здоровья
            player.heal(healValue);
            std::cout << "Вы использовали " << selectedItem.name << " и восстановили " 
                      << healValue << " здоровья!" << std::endl;
        }
        else if (selectedItem.name.find("зелье маны") != std::string::npos) {
            // Извлекаем число из описания
            std::string valueStr = selectedItem.description.substr(
                selectedItem.description.find_last_of(' ') + 1, 
                selectedItem.description.find("MP") - selectedItem.description.find_last_of(' ') - 1
            );
            int manaValue = std::stoi(valueStr);
            
            // Применяем эффект зелья маны
            player.useMana(-manaValue); // Отрицательное значение восстанавливает ману
            std::cout << "Вы использовали " << selectedItem.name << " и восстановили " 
                      << manaValue << " маны!" << std::endl;
        }
        else if (selectedItem.name == "Зелье силы") {
            // Добавляем бафф атаки
            activeBuffs.push_back(ActiveBuff(selectedItem.name, 10, 0, 0, 0, 3));
            player.boostAttack(10);
            std::cout << "Вы использовали Зелье силы! Ваша атака увеличена на 10 на 3 хода." << std::endl;
        }
        else if (selectedItem.name == "Эликсир защиты") {
            // Добавляем бафф защиты
            activeBuffs.push_back(ActiveBuff(selectedItem.name, 0, 15, 0, 0, 3));
            player.boostDefense(15);
            std::cout << "Вы использовали Эликсир защиты! Ваша защита увеличена на 15 на 3 хода." << std::endl;
        }
        else if (selectedItem.name == "Зелье регенерации") {
            // Добавляем бафф регенерации здоровья
            activeBuffs.push_back(ActiveBuff(selectedItem.name, 0, 0, 10, 0, 3));
            std::cout << "Вы использовали Зелье регенерации! Вы будете восстанавливать 10 HP каждый ход в течение 3 ходов." << std::endl;
        }
        else if (selectedItem.name == "Зелье маны") {
            // Добавляем бафф регенерации маны
            activeBuffs.push_back(ActiveBuff(selectedItem.name, 0, 0, 0, 5, 3));
            std::cout << "Вы использовали Зелье маны! Вы будете восстанавливать 5 MP каждый ход в течение 3 ходов." << std::endl;
        }
        else if (selectedItem.name == "Эликсир ярости") {
            // Добавляем бафф атаки с дебаффом защиты
            activeBuffs.push_back(ActiveBuff(selectedItem.name, 20, -10, 0, 0, 2));
            player.boostAttack(20);
            player.boostDefense(-10);
            std::cout << "Вы использовали Эликсир ярости! Ваша атака увеличена на 20, но защита снижена на 10 на 2 хода." << std::endl;
        }
        else if (selectedItem.name == "Зелье берсерка") {
            // Добавляем бафф атаки и здоровья
            activeBuffs.push_back(ActiveBuff(selectedItem.name, 15, 0, 0, 0, 3));
            player.boostAttack(15);
            player.boostHealth(10);
            std::cout << "Вы использовали Зелье берсерка! Ваша атака увеличена на 15, а максимальное здоровье на 10 на 3 хода." << std::endl;
        }
        else if (selectedItem.name == "Зелье хаоса") {
            // Случайный бафф
            std::random_device rd;
            std::mt19937 gen(rd());
            std::uniform_int_distribution<> distrib(0, 2);
            int randStat = distrib(gen);
            
            if (randStat == 0) {
                activeBuffs.push_back(ActiveBuff(selectedItem.name, 25, 0, 0, 0, 2));
                player.boostAttack(25);
                std::cout << "Вы использовали Зелье хаоса! Случайный эффект: ваша атака увеличена на 25 на 2 хода." << std::endl;
            } else if (randStat == 1) {
                activeBuffs.push_back(ActiveBuff(selectedItem.name, 0, 25, 0, 0, 2));
                player.boostDefense(25);
                std::cout << "Вы использовали Зелье хаоса! Случайный эффект: ваша защита увеличена на 25 на 2 хода." << std::endl;
            } else {
                activeBuffs.push_back(ActiveBuff(selectedItem.name, 0, 0, 25, 0, 2));
                player.heal(25);
                std::cout << "Вы использовали Зелье хаоса! Случайный эффект: вы восстановили 25 здоровья и получите еще 25 в следующем ходу." << std::endl;
            }
        }
        else if (selectedItem.name == "Зелье медитации") {
            // Полное восстановление маны, но пропуск хода
            player.useMana(-player.getMaxMana()); // Полностью восстанавливаем ману
            playerStunned = true;
            playerStunDuration = 1;
            std::cout << "Вы использовали Зелье медитации! Ваша мана полностью восстановлена, но вы пропустите следующий ход." << std::endl;
        }
        else {
            std::cout << "Этот предмет пока не реализован." << std::endl;
        }
        
        // Удаляем использованный предмет из инвентаря
        player.removeFromInventory(itemIndex);
        
        pauseForDialogue();
        return;
    }
    else if (choice == inventory.size() + 1) {
        // Возврат в меню боя
        return;
    }
    else {
        std::cout << "Неверный выбор. Попробуйте снова." << std::endl;
        pauseForDialogue();
    }
}

// Метод для обновления баффов в конце хода
void BattleSystem::updateBuffs(Character& player) {
    // Безопасная проверка перед обработкой баффов
    try {
        // Проверяем, существует ли вектор и не пустой ли он
        if (activeBuffs.empty()) {
            return;
        }
        
        std::cout << "\nАктивные эффекты:" << std::endl;
        
        // Проходим по всем активным баффам
        for (auto it = activeBuffs.begin(); it != activeBuffs.end(); ) {
            // Дополнительная проверка валидности итератора
            if (it == activeBuffs.end()) {
                break;
            }
            
            // Применяем эффекты регенерации
            if (it->healthRegen > 0) {
                player.heal(it->healthRegen);
                std::cout << "Эффект " << it->name << " восстанавливает " << it->healthRegen << " HP!" << std::endl;
            }
            
            if (it->manaRegen > 0 && player.doesHaveMana()) {
                player.useMana(-it->manaRegen);
                std::cout << "Эффект " << it->name << " восстанавливает " << it->manaRegen << " MP!" << std::endl;
            }
            
            // Уменьшаем длительность
            it->duration--;
            
            // Если бафф закончился, отменяем его эффект и удаляем
            if (it->duration <= 0) {
                std::cout << "Эффект " << it->name << " закончился!" << std::endl;
                
                // Отменяем баффы атаки и защиты
                if (it->attackBonus != 0) {
                    player.boostAttack(-it->attackBonus);
                }
                
                if (it->defenseBonus != 0) {
                    player.boostDefense(-it->defenseBonus);
                }
                
                // Удаляем бафф из списка
                it = activeBuffs.erase(it);
            } else {
                std::cout << "Эффект " << it->name << " действует еще " << it->duration << " ход(ов)." << std::endl;
                ++it;
            }
        }
        
        pauseForDialogue();
    } catch (const std::exception& e) {
        // В случае ошибки просто завершаем функцию без падения
        std::cout << "Ошибка при обработке эффектов." << std::endl;
        return;
    }
}

void BattleSystem::showEnemyInfo(const Character& enemy) {
    std::cout << "\nИнформация о противнике:\n" << std::endl;
    std::cout << "Имя: " << enemy.getName() << std::endl;
    std::cout << "Здоровье: " << enemy.getHealth() << std::endl;
    std::cout << "Атака: " << enemy.getAttack() << std::endl;
    std::cout << "Очки действий: " << enemy.getMana() << std::endl;
    
    // Небольшая задержка перед возвратом
    pauseForDialogue();
}

int BattleSystem::calculateDamage(const Character& attacker, const Character& defender) {
    // Базовая формула урона: базовый урон + атака атакующего
    int baseDamage = 10;
    int rawDamage = baseDamage + attacker.getAttack();
    
    // Новая механика брони: 
    // Броня снижает урон на процент: каждые 10 единиц защиты снижают урон на 10%
    // Но не более чем на 70% от исходного урона
    double defensePercent = defender.getDefense() * 0.01; // 1% за каждую единицу защиты
    
    // Ограничиваем максимальное снижение урона до 70%
    if (defensePercent > 0.7) defensePercent = 0.7;
    
    // Вычисляем итоговый урон с учетом брони
    int finalDamage = static_cast<int>(rawDamage * (1.0 - defensePercent));
    
    // Минимальный урон всегда 5
    if (finalDamage < 5) finalDamage = 5;
    
    return finalDamage;
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

// Метод для применения магии с возвратом результата успешности
bool BattleSystem::castMagic(Character& player, Character& enemy, int spellChoice) {
    // Проверяем, есть ли у игрока мана
    if (!player.doesHaveMana() || player.getMana() <= 0) {
        std::cout << "У вас недостаточно маны!" << std::endl;
        pauseForDialogue();
        return false;
    }
    
    // Фразы Дэна в ответ на магию игрока
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
    
    // Выбор случайной фразы
    std::random_device rd;
    std::mt19937 gen(rd());
    
    switch (spellChoice) {
        case 1: // Огненный шар
            if (player.useMana(15)) {
                // Используем атаку персонажа для увеличения урона
                int damage = 15 + player.getAttack() / 2;
                enemy.takeDamage(damage);
                
                std::cout << "Вы создаете огненный шар и запускаете его в противника!" << std::endl;
                
                // Выводим фразу противника
                std::uniform_int_distribution<> distrib(0, enemyResponseToFireball.size() - 1);
                std::cout << "\n" << enemy.getName() << ": \"" << enemyResponseToFireball[distrib(gen)] << "\"" << std::endl;
                
                // Добавляем паузу после фразы
                pauseForDialogue();
                
                return true;
            } else {
                std::cout << "Недостаточно маны для огненного шара!" << std::endl;
                pauseForDialogue();
                return false;
            }
            break;
            
        case 2: // Ледяной щит
            if (player.useMana(10)) {
                player.boostDefense(10);
                
                std::cout << "Вы создаете ледяной щит вокруг себя!" << std::endl;
                
                // Выводим фразу противника
                std::uniform_int_distribution<> distrib(0, enemyResponseToIceShield.size() - 1);
                std::cout << "\n" << enemy.getName() << ": \"" << enemyResponseToIceShield[distrib(gen)] << "\"" << std::endl;
                
                // Добавляем паузу после фразы
                pauseForDialogue();
                
                return true;
            } else {
                std::cout << "Недостаточно маны для ледяного щита!" << std::endl;
                pauseForDialogue();
                return false;
            }
            break;
            
        case 3: // Телепортация
            if (player.useMana(20)) {
                player.heal(20);
                
                std::cout << "Вы телепортируетесь, избегая вражеской атаки и восстанавливая силы!" << std::endl;
                
                // Выводим фразу противника
                std::uniform_int_distribution<> distrib(0, enemyResponseToTeleport.size() - 1);
                std::cout << "\n" << enemy.getName() << ": \"" << enemyResponseToTeleport[distrib(gen)] << "\"" << std::endl;
                
                // Добавляем паузу после фразы
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
    
    return false; // Не должны дойти сюда, но на всякий случай
}

// Добавляем новый метод для отображения статистики атак
void BattleSystem::displayEnemyAttackStats() {
    // Получаем доступные атаки от AIController
    std::vector<EnemyAttack> availableAttacks = aiController.getAvailableAttacks();
    
    // Счетчики для каждого типа атак
    int normalAttackCount = 0;
    int dodgeAttackCount = 0;
    int stunAttackCount = 0;
    
    // Подсчитываем количество каждого типа атак
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
    
    // Отображаем статистику с улучшенным форматированием
    std::cout << "\n------------------------------------------------" << std::endl;
    std::cout << "Оставшиеся атаки Дэна:" << std::endl;
    std::cout << "  • Обычные атаки: " << normalAttackCount << std::endl;
    std::cout << "  • Атаки с уклонением: " << dodgeAttackCount << std::endl;
    std::cout << "  • Оглушающие атаки: " << stunAttackCount << std::endl;
    std::cout << "------------------------------------------------" << std::endl;
}

// Метод для отображения намека на действие противника
void BattleSystem::displayEnemyHint(const EnemyAttack& nextAction) {
    // Фразы-намеки на действия Дэна в следующем ходу
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
    
    // Выбор случайной фразы-намека в зависимости от типа действия
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
    
    // Добавляем паузу после намека, чтобы игрок успел его прочитать
    pauseForDialogue();
} 