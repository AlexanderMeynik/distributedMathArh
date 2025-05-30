[На основную страницу](../README.md)


## Основные
- [ ] починить все баги с реализацией процедур на openmp
- [x] Привести архитектуру в формат source/lib/include;
    - [x] Добавить include directories для включаемых файлов
    - [x] Создать namespace для нужных моделей
- [x] Починить тестовые сценарии;
    - [x] Уточнить процедуру сборки данных о покрытии кода;
    - [x] Переместить реализации test case в сpp файлы для независимого запуска;
    - [x] Конфигурация для запуска;
    - [ ] Добавить больше тестов для непокрытого функционала
- [x] Уточнить зависимости;
- [x] Упаковать зависимости в docker образ;
    - [ ] Создать несколько образов для запуска разных частей приложения;
    - [x] Оптимизировать workflow для сборки docker
- [x] Создать workflow для сборки и тестирования проекта;
    - [x] Добавить сбор покрытия кода
    - [x] Добавить инструменты для оценки семантической сложности кода

### По библиотекам и коду

- [x] Добавить библиотеку [drogon](https://github.com/drogonframework/drogon) для основного(и вычислительного) узла;
- [x] Реализовать передачу данных о вычислениях через очередь RabbiMQ;
    - [x] Реализовать логику взаимодействия RabbitMQ;
    - [x] Протокол подключения новых узлов;
    - [x] Конфигурация и запуск RabbitMQ(в контейнере);
    - [x] Соединить все компоненты воедино для создания сервисов
- [ ] Создать описание для API элементов:
    - [ ] Головной узел
    - [ ] Вычислительный узел(почти готов)
- [ ] Реализировать ER диаграмму
    - [x] Для PostgreSQL
    - [ ] Для редиса
- [ ] Реализовать простой кеш-сервер
    - [ ] Что будем хранить
    - [ ] Через что лучше запрашивать(напрямую, или через mainNode)
    - [ ] Стратегия кеширования
- [ ] Переделать семантику вычислительного этапа;
    - [x] Добавить парсер для вычислительных этапов;
    - [x] Починить segfault при работе openmp с бенчмарками и выч этапом
    - [x] Добавить соответствующие проверки в тестах;
- [x] Добавить документацию Doxygen;
    - [x] Уточнить её конфигурацию;
    - [x] Добавить ссылки на источники и исследования;
- [x] Разобраться с тем где и как будем хранить данные для визуализаций(Мэши).
- [x] Почитать и избавиться от ненужных todo;
- [x] Решить проблему с solve методами для dipoles
    - [x] адаптировать MeshProcessor2
    - [x] Протестировать новую реализацию см. [SpeedTests.cpp](computationalNode%2Ftest%2Fmath_core%2FSpeedTests.cpp)
    - [x] Отделить рисование графиков от основной реализации на узле
    - [x] Сократить размер boost зависимостей(150 мб - много)(run bcp to extract gaus kronrod method)
    - [x] Добавить релиз для извлечённых из boost процедур численного интегрирования
- [x] Стандартизировать представления для генераторов
    - [x] создать функцию для генерации разных вариантов конфигураций(массив векторов, 1 вектор);
    - [x] Починить grids для диполей
      #todo dependecy tree https://cmake.org/cmake/help/latest/module/CMakeGraphVizOptions.html
    - or https://embeddeduse.com/2022/03/01/visualising-module-dependencies-with-cmake-and-graphviz/
