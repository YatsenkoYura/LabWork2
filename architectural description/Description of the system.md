# Дуэль с Деном

## Описание игры

Дуэль с Деном — это текстовая пошаговая RPG игра, где игроки сражаются на магической арене с использованием своих навыков и магических способностей. Вам предстоит победить заклятого врага Дена, преодолевая его постоянно растущую мощь. Игра оформлена в стиле текстовой игры, где интерфейс реализован с помощью символов ASCII.

## Ключевые особенности

**Интерфейс ASCII**: Игра использует текстовое представление для отображения характеристик персонажей и управления.
**Магазин**: Доступен после каждой победы над Деном. В магазине можно приобретать улучшения, оружие и зелья, чтобы подготовиться к следующему раунду.
**Сражения до первого поражения**: Игра продолжается до тех пор, пока игрок не встретит своё первое поражение. При победе герои улучшаются, и начинается новый раунд.
**Система улучшений и инвентаря**: Позволяет игроку развивать своего персонажа и стратегически управлять ресурсами.

## Игровой процесс

1. **Сражение на арене**: 
Пошаговый бой с выбором действий — атака, защита, использование магии или предметов.
2. **После победы над Деном**:
Игрок может посетить магазин, чтобы купить улучшения и предметы.
Каждый раунд враг становится сильнее, увеличивая сложность игры.
3. **Улучшения**:
Игрок получает очки, которые можно использовать для улучшения характеристик.
Магазин предлагает новый ассортимент предметов перед каждым раундом.

## Боевая система

1. **Структура боя**:
   - Бой состоит из последовательных раундов
   - В каждом раунде игрок ходит первым
   - Бой продолжается до потери здоровья одним из участников
   
2. **Доступные действия игрока**:
   - Атака оружием
   - Использование предметов из инвентаря
   - Применение магии:
     * Атакующая магия
     * Защитная магия
     * Магия уклонения

3. **Искусственный интеллект Дена**:
   - Получает очки улучшений (point battle) в зависимости от номера раунда
   - Может использовать:
     * Обычную атаку
     * Атаку на защиту
     * Атаку на уклонение
     * Магические способности

## Компоненты системы

1. **Игровой движок**:
   - Управляет состояниями игры
   - Координирует взаимодействие между компонентами
   - Обрабатывает игровой цикл

2. **Пользовательский интерфейс**:
   - Отображает игровую информацию в текстовом формате
   - Обрабатывает пользовательский ввод
   - Показывает таблицу лидеров

3. **Система персонажей**:
   - Управляет характеристиками персонажей
   - Обрабатывает инвентарь
   - Отслеживает улучшения

4. **Система достижений**:
   - Ведёт учёт пройденных раундов
   - Сохраняет рекорды в таблице лидеров
   - Отображает достижения на стартовом экране

