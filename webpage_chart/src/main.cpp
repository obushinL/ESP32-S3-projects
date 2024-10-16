/**
 * \file main.cpp
 * \author 
 * \brief 
 * \version 0.1
 * \date 
 * 
 * \copyright Copyright (c) 2024
 * 
 */

// Инструменты фреймворка Arduino
#include <Arduino.h>
// Библиотека для связи микроконтроллера с устройствами через интерфейс I2C
#include <Wire.h>
// Библиотека для создания Веб Сервера 
#include <WebServer.h>
// Библиотека для подключения к беспроводной сети
#include <WiFi.h>

// Подключаем .h файл с кодом Веб страницы
#include "site.h"

/* Логин сети устройства*/
const char *ssid = "Your_ssid";
/* Пароль сети устройства*/
const char *password = "Your_passsword";

/* IP локальной сети*/
IPAddress local_ip(192, 168, 1, 1); 
/* Gateway локальной сети*/
IPAddress gateway(192, 168, 1, 1);
/* Маска локальной сети*/
IPAddress subnet(255, 255, 255, 0);

// Создаем объект веб-сервера с HTTP портом (80 по умолчанию)
WebServer server(80);

// Последние значение таймера
unsigned long time_p = 0;
unsigned long timePast = 0;
// Значение температуры
int8_t temp = 0;
// Строка со значением температуры клиента
String temp_value;

const int size = 10;
double values[size]{0,0,0,0,0,0,0,0,0,0};

// Обновление данных о температуре
void update_temp_data()
{
    // Текущее значение по таймеру в мс
    unsigned long time_n = millis();

    if((time_n - time_p) > 3000 )
    {
        temp = (int8_t)(temperatureRead());
        time_p = time_n;
    }
}


// Функция отправка кода веб страницы
void handle_root()
{
    // Отправка кода веб страницы клиенту
    server.send(200, "text/html", HTML);
    // Время обновления веб-страницы
    delay(200);
}

    // Формируем строку в формате json 
        //  {"id": "device_1"
    //  "data_type": "String"
    //  "Data": "temperature_values"
    //  "Value1": "values[0]"
    //  "Value2": "values[1]"
    //  "Value3": "values[2]"
    //  "Value4": "values[3]"
    //  "Value5": "values[4]"
    //  "Value6": "values[5]"
    //  "Value7": "values[6]"
    //  "Value8": "values[7]"
    //  "Value9": "values[8]"
    //  "Value10": "values[9]"
    // }

// Обновление данных о температуре
void update_temp_values_data() {
    int i;
    // Текущее значение по таймеру в мс
    unsigned long timeNew = millis();

    if ((timeNew - timePast) > 30000) {
        for (i = 0; i < 9; i++) {
            values[i] = values[i + 1]; 
        }
        timePast = timeNew;
        values[9] = (double)(temperatureRead());    
    }
    // Формируем строку в json формате
    String temp_value = "{\"id\":\"device_1\", \"data_type\":\"String\", \"Data\": \"temperature_values\"";
        
    for (i = 0; i < 10; i++) {
        temp_value += ", \"Value" + String(i + 1) + "\":\"" + String(values[i]) + "\"";
    }
        
    temp_value += "}"; // Закрывающая фигурная скобка для JSON
    server.send(200, "application/json", temp_value); 
}

void setup()
{
    // Инициализация последовательного соединения и определение скорости передачи данных в бит/c (бод)
    Serial.begin(115200);
    
    // Подключение wifi в режиме Station
    WiFi.softAP(ssid, password);
    WiFi.softAPConfig(local_ip, gateway, subnet);

    // Вывод IP микроконтроллера в сети
    Serial.println("Create wifi station: " + String(ssid));

    // Полный код веб-страницы
    server.on("/", handle_root);

    server.on("/data_temp_values", update_temp_values_data);
    // Запуск сервера
    server.begin();

}


// Основной цикл программы
void loop()
{
    // Принимать действия пользователя
    server.handleClient();
    update_temp_data();
}

