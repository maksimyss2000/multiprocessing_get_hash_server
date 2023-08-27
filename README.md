# multiprocessing_get_hash_server

Задание 1 Сервис для подсчета хеш-сумм (состоит из сервера и клиента)

Веб-сервер реализует протокол HTTP 1.0. Требования:
    1. Сервер поддерживает до 1 000 параллельных подключений
    2. Реализован запрос POST, ответы 200 и 404, а также Content-type - application/json
    3. Сервер по запросу POST принимает данные, вычисляет две хеш-функции (ГОСТ Р 34.11-94 и SHA512) от этих данных и возвращает полученный результат
    4. На входе POST-запрос со следующим json-содержимым:
{ 
“data”: “hhadhskfhjdkslhfklashdfklhadshfklashdfklhasklfhashdfklhasfkjhasdkdhfkshdf”
}
где data - это данные, от которых необходимо вычислить хеш-функции; максимальный размер данных 1 024 байта.
    5. На выходе ответ со следующим json-содержимым:
{ 
“sha512”:
“4adfbbf00cd146301af5f0661b55d13439a14f67555b6dd03ad55ccf5efc85c812347f1050f2e481620d7a7ba588a5df5f30bc04f896903ae09217a6ae6d4abd”,
“gost”: “b75a0ac84397a0b2329316e333a58338a32a0ec2cc6e0e9843f9cfa80eb696f0”
}
где sha512 – SHA512 хеш-сумма от присланных данных,
gost – ГОСТ хеш-сумма от присланных данных.

Клиентское ПО для работы с сервером. Требования:
    1. Клиент считывает текстовые данные из потока ввода, упаковывает и отправляет на сервер
    2. Получает ответ от сервера и выводит полученные хеши данных

Задание 2 (Не обязательное)
Используя реализацию сервера и клиента на языке С из первого задания сделайте для них объектные обертки на языке С++.

Требования к присылаемым решениям:
    1. Язык программирования: 1 задание – С, 2 задание – С++
    2. ОС: Linux Debian x64
    3. Для подсчета хеш-сумм использовать библиотеки: mhash (https://github.com/Sembiance/mhash/blob/master/deps/mhash/doc/example.c) или gostengine для OpenSSL 1.1.1 (https://github.com/gost-engine/engine/blob/master/README.gost, собирать ветку openssl_1_1_1).
    4. Использование библиотек для работы с HTTP и json не допускается.
    5. Готовое задание должно быть передано ответным письмом в zip архиве.
    6. Для сборки программа не должна требовать настроек системы или нахождения определенных файлов в специфичном месте (используемые библиотеки считать уже установленными).
    7. Исходный код должен компилироваться средствами cmake или make с использованием gcc для работы в среде Linux. В папке с исходным кодом не должно быть мусора: неиспользуемых файлов исходных кодов или ресурсов, промежуточных файлов сборки и т.д.
