[К содержанию](DocPage.md)

# Стек технологий

Основные инструменты разработки

| **Название**   | **Версия** | **Назначение**                                                               |
| -------------- | ---------- |------------------------------------------------------------------------------|
| CLion          | 2024.3.2   | IDE для разработки                                                           |
| gcc            | 11.4.0     | набор компиляторов и других утилит для разработки                            |
| cmake          | 3.2.6      | Инструмент для создания конфигурации проекта                                 |
| ninja          | 1.10.1     | Инструмент для быстрой сборки проекта                                         |
| git            | 2.34.1     | В качестве системы контроля версий                                           |
| lcov           | 1.14       | Для создания наглядных отчётов о покрытие кода                               |
| gdb            | 12.1       | Для отладки исходного кода                                                   |
| docker         | 2.35.0     | Инструмент создания контейнеров для стандартизации среды исполнения и сборки |
| Metrix++       | 1.8.1      | В качестве инструмента для сбора и мониторинга метрик проекта                |
| ninjatracing   | -          | В качестве инструмента для трассировки логов сборки проекта                  |
| doxygen        | 1.9.1      | Инструмент для генерации кодовой документации                                |
| GitHub Actions | -          | Для реализации СI/CD сценариев, хранения артефактов и исходного кода.        |

Библиотеки проекта:

| **Название**    | **Версия** | **Назначение**                                                                                                                                       |
|-----------------|------------|------------------------------------------------------------------------------------------------------------------------------------------------------|
| Drogon          | 1.9.10     | Фреймворк для создания веб-сервисов                                                                                                                  |
| OpenSSL         | 3.0.2      | Поставщик функций для организации безопасных соединений                                                                                              |
| jsoncpp         | 1.9.5      | Сериализация объектов в формат JSON                                                                                                                  |
| libpq           | 14.13      | Взаимодействие с PostgreSQL, зависимость libpqxx                                                                                                     |
| libpqxx         | 7.10.0     | Высокопроизводительный клиент для взаимодействия с PostgreSQL                                                                                        |
| AMQP-CPP        | 4.3.27     | Библиотека для организации асинхронного взаимодействия с RabbitMQ                                                                                    |
| hiredis         | 0.14.1     | Клиент для библиотеки Redis (интеграция Drogon)                                                                                                      |
| OpenMP          | 4.5        | Переносимая реализация параллелизма (может использоваться Eigen)                                                                                     |
| libcurl         | 7.81.0     | Библиотека для выполнения http запросов с использованием curl.                                                                                       |
| libfmt          | 8.1.1      | Библиотека для форматирования и вывода данных.                                                                                                       |
| scnlib          | 4.0.1      | Библиотека для считывания данных из форматированных строк.                                                                                           |
| Boost           | 1.74       | В проекте используются компоненты Boot Math - реализация процедуры численного интегрирования, Boost Asio – для организации io в асинхронных сервисах |
| Eigen           | 3.4.0      | Библиотека линейных преобразований. Поддерживает векторизацию кода, при этом представляя удобный интерфейс для работы.                               |
| matplotplusplus | 1.2.2      | Библиотека визуализации данных. Позволяет сохранять полученные графики в высоком разрешении. Оборачивает вызовы GNU plot.                            |
| qt6-base        | 6.2.4      | Для реализации пользовательского GUI                                                                                                                 |
| google test     | 1.15.0     | Фреймворк для создания тестовых сценариев и организации процедур проверки сложных утверждений.                                                       |
| cpptrace        | 1.0.1      | Библиотека для работы и вывода stacktrace                                                                                                            |
| *Wiremock       | ?          | Предполагается использовать в ходе будущего тестирования модулей REST                                                                                |

Сервисы в составе предлагаемой системы

|**Название**|**Версия**|**Назначение**|
|---|---|---|
|RabbitMQ|4.0|Данный брокер используется для хранения данных о задачах и распределения задач между узлами. Для доступа к REST API используется версия с плагином management|
|PostgreSQL|17.0|Данная БД применяется для хранения результатов промежуточных вычислений, пользовательских данных, а также общую информацию о состоянии системы. Для поддержки горизонтального масштабирования используется версия с плагином citus data|
|Redis|8.0|Данная БД применяется кеширования данных пользовательских запросов|

Прочие инструменты разработки:

|**Название**|**Версия**|**Назначение**|
|---|---|---|
|Docker Hub|-|Сервис для хранения docker изображений|
|Mermaid|-|Сервис для создания редактируемых и легко переносимых диаграмм|
|Drawio|-|Сервис для создания и редактирования диаграмм с большим числом разнородных элементов|
|Postman|11.4.0|Инструмент для тестирования некоторых REST сервисов|
