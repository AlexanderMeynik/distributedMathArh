# Разработка распределённой архитектуры для стохастических вычислений

Данный проект посвящён разработке дипломного проекта на тему "Разработка распределённой модели вычислений для моделирования системы диполей"


## Задачи
### Основные
- [ ] Привести архитектуру в формат source/lib/include;
  - [ ] Добавить include directories для включаемых файлов
  - [ ] Создать namespace для нужных модйлей
- [ ] Починить тестовые сценарии;
  - [ ] Уточнить процедуру сборки данных о покрытии кода;
  - [ ] Переместить реализации test case в сpp файлы для независимого запуска;
  - [ ] Конфигурация для запуска;
- [ ] Уточнить зависимости;
- [ ] Упаковать зависимости в docker образ;
- [ ] Создать workflow для сборки и тестирования проекта;
### По библиотекам и коду
- [ ] Использовать crow в вычислительных узлах
- [ ] Добавить библиотеку [drogon](https://github.com/drogonframework/drogon) для основного узла;
- [ ] Реализовать передачу данных о вычислениях через очередь RabbiMQ;
  - [ ] Клиент RabbitMQ для C++;
  - [ ] Протокол подключения новых узлов;
  - [ ] Конфигурация и запуск RabbitMQ(в контейнере);
- [ ] Переделать семантику вычислительного этапа;
  - [ ] Добавить парсер для вычислительных этапов;
  - [ ] Использовать executor подход(вместо variadic шаблонов) для организации исполнения;
  - [ ] Добавить соответствующие проверки в тестах;
- [ ] Добавить документацию Doxygen;
  - [ ] Уточнить её конфигурацию;
  - [ ] Добавить ссылки на источники и исследования;
- [ ] Заменить timer на более навороченный(например [отсюда](https://github.com/AlexanderMeynik/data_deduplication_service/blob/master/include/clockArray.h));
  - [ ] Изменить тесты для него
- [ ] Разобраться с тем где и как будем хранить данные для визуализаций(Мэши).
- [ ] Почитать и избавиться огт ненужных todo;
- [ ] Решить проблему с solve методами для dipoles
- [ ] Стандартизировать представления для генераторов