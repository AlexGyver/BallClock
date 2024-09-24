[![latest](https://img.shields.io/github/v/release/GyverLibs/WiFiConnector.svg?color=brightgreen)](https://github.com/GyverLibs/WiFiConnector/releases/latest/download/WiFiConnector.zip)
[![PIO](https://badges.registry.platformio.org/packages/gyverlibs/library/WiFiConnector.svg)](https://registry.platformio.org/libraries/gyverlibs/WiFiConnector)
[![Foo](https://img.shields.io/badge/Website-AlexGyver.ru-blue.svg?style=flat-square)](https://alexgyver.ru/)
[![Foo](https://img.shields.io/badge/%E2%82%BD%24%E2%82%AC%20%D0%9F%D0%BE%D0%B4%D0%B4%D0%B5%D1%80%D0%B6%D0%B0%D1%82%D1%8C-%D0%B0%D0%B2%D1%82%D0%BE%D1%80%D0%B0-orange.svg?style=flat-square)](https://alexgyver.ru/support_alex/)
[![Foo](https://img.shields.io/badge/README-ENGLISH-blueviolet.svg?style=flat-square)](https://github-com.translate.goog/GyverLibs/WiFiConnector?_x_tr_sl=ru&_x_tr_tl=en)  

[![Foo](https://img.shields.io/badge/ПОДПИСАТЬСЯ-НА%20ОБНОВЛЕНИЯ-brightgreen.svg?style=social&logo=telegram&color=blue)](https://t.me/GyverLibs)

# WiFiConnector
Асинхронное подключение к WiFi с созданием точки доступа при таймауте подключения

### Совместимость
ESP8266, ESP32

## Содержание
- [Использование](#usage)
- [Версии](#versions)
- [Установка](#install)
- [Баги и обратная связь](#feedback)

<a id="usage"></a>

## Использование
Использовать в случаях, когда пароль от WiFi хранится "где-то". При передаче пустой строки будет запущена точка доступа. Если подключиться не получится - также развернётся точка доступа для веб-интерфейса и прочего.

### Описание классов
```cpp
// имя AP, пароль AP, таймаут в секундах, отключать AP при успешном подключении к STA (иначе AP_STA)
WiFiConnectorClass(const String& APname = "ESP AP",
                    const String& APpass = "",
                    uint16_t timeout = 60,
                    bool closeAP = true);

// установить имя AP
void setName(const String& name);

// установить пароль AP
void setPass(const String& pass);

// установить таймаут в секундах
void setTimeout(uint16_t timeout);

// автоматически отключать AP при подключении к STA (умолч. вкл)
void closeAP(bool close);

// подключить обработчик успешного подключения
void onConnect(ConnectorCallback cb);

// подключить обработчик ошибки подключения, вызовется после старта AP
void onError(ConnectorCallback cb);

// подключиться. Вернёт false если ssid не задан, будет запущена AP
bool connect(const String& ssid, const String& pass = emptyString);

// вызывать в loop. Вернёт true при смене состояния
bool tick();

// состояние подключения. true - подключен, false - запущена АР
bool connected();

// в процессе подключения
bool connecting();
```

### Примеры
```cpp
#include <Arduino.h>
#include <WiFiConnector.h>

void setup() {
    WiFiConnector.connect("SSID", "PASS");

    WiFiConnector.onConnect([]() {
        Serial.print("Connected. Local IP: ");
        Serial.println(WiFi.localIP());
    });
    WiFiConnector.onError([]() {
        Serial.print("WiFi error. AP IP: ");
        Serial.println(WiFi.softAPIP());
    });
}

void loop() {
    WiFiConnector.tick();
}
```

<a id="versions"></a>

## Версии
- v1.0

<a id="install"></a>
## Установка
- Библиотеку можно найти по названию **WiFiConnector** и установить через менеджер библиотек в:
    - Arduino IDE
    - Arduino IDE v2
    - PlatformIO
- [Скачать библиотеку](https://github.com/GyverLibs/WiFiConnector/archive/refs/heads/main.zip) .zip архивом для ручной установки:
    - Распаковать и положить в *C:\Program Files (x86)\Arduino\libraries* (Windows x64)
    - Распаковать и положить в *C:\Program Files\Arduino\libraries* (Windows x32)
    - Распаковать и положить в *Документы/Arduino/libraries/*
    - (Arduino IDE) автоматическая установка из .zip: *Скетч/Подключить библиотеку/Добавить .ZIP библиотеку…* и указать скачанный архив
- Читай более подробную инструкцию по установке библиотек [здесь](https://alexgyver.ru/arduino-first/#%D0%A3%D1%81%D1%82%D0%B0%D0%BD%D0%BE%D0%B2%D0%BA%D0%B0_%D0%B1%D0%B8%D0%B1%D0%BB%D0%B8%D0%BE%D1%82%D0%B5%D0%BA)
### Обновление
- Рекомендую всегда обновлять библиотеку: в новых версиях исправляются ошибки и баги, а также проводится оптимизация и добавляются новые фичи
- Через менеджер библиотек IDE: найти библиотеку как при установке и нажать "Обновить"
- Вручную: **удалить папку со старой версией**, а затем положить на её место новую. "Замену" делать нельзя: иногда в новых версиях удаляются файлы, которые останутся при замене и могут привести к ошибкам!

<a id="feedback"></a>

## Баги и обратная связь
При нахождении багов создавайте **Issue**, а лучше сразу пишите на почту [alex@alexgyver.ru](mailto:alex@alexgyver.ru)  
Библиотека открыта для доработки и ваших **Pull Request**'ов!

При сообщении о багах или некорректной работе библиотеки нужно обязательно указывать:
- Версия библиотеки
- Какой используется МК
- Версия SDK (для ESP)
- Версия Arduino IDE
- Корректно ли работают ли встроенные примеры, в которых используются функции и конструкции, приводящие к багу в вашем коде
- Какой код загружался, какая работа от него ожидалась и как он работает в реальности
- В идеале приложить минимальный код, в котором наблюдается баг. Не полотно из тысячи строк, а минимальный код