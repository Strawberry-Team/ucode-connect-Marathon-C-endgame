# ucode-endgame

# Механики
- У нас вертикальный уровень, где персонаж движется только вверх по платформам.
- Персонаж может собирать какие-то сущности (монетки), которые могут:
    - положительно
        - увеличение высоты прыжка (100 -> 200)
        - двойной или тройной прыжок
    - негативно
        - "лава" поднимается быстрее
    - Бафы или дебафы (монетки) пропадают после использования
- Персонаж может умереть, если будет касание с "лавой" каким-то объектом, который постепенно поднимается от начала карты
- Платформы могут быть:
    - статичные
    - динамические - двигаются в стороны
    - исчезающие - если персонаж стоит на них больше N секунд, могут ломаться от кол-во интеграций с платформой.

# Сеттинг
- Главный персонаж - обезьянка, которая собирает бананы и убегает от льва.
- Главный персонаж - искатель сокровищ, который взял главное сокровище из гробницы. Активировалась ловушка и пошла лава. Ему нужно убежать.