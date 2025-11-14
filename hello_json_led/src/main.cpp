/*
  hello_json_led.ino - Example of JSON command processing on Arduino UNO R4 WiFi

  This example demonstrates how to receive JSON commands over Serial to control an LED,
  and periodically send status updates in JSON format.

  Commands:
    {"cmd": "power", "value": 1}  // Turn LED ON
    {"cmd": "power", "value": 0}  // Turn LED OFF

  Status Update (every second):
    {"led_state": true/false}

  Requires ArduinoJson library.

  created with ChatGPT
*/
#include <Arduino.h>
#include <ArduinoJson.h>

void parseJsonLine(String &line);

String buffer = "";

// 状態変数（例として初期値をセット）
bool  led_state = false;

unsigned long last_status_time = 0;  // 1秒タイマー用

int ledPin = 7; // LEDが接続されているピン番号

void setup() {
  Serial.begin(115200);
  pinMode(ledPin, OUTPUT);
}

void loop() {
  while (Serial.available()) {
    char c = Serial.read();

    if (c == '\n') {
      // 行が完成したのでJSONとして処理
      parseJsonLine(buffer);
      buffer = ""; // 次の行に備える
    } else {
      buffer += c;
    }
  }

  // ----------------------------------------------------
  // ② 他の処理（ノンブロッキング）
  //    → 1秒おきに led_state, voltage, current を送信
  // ----------------------------------------------------
  unsigned long now = millis();
  if (now - last_status_time >= 1000) {  // 1秒経過
    last_status_time = now;

    StaticJsonDocument<128> doc;
    doc["led_state"] = led_state;

    serializeJson(doc, Serial);
    Serial.println(); // 行区切り
  }
}

void parseJsonLine(String &line) {
  StaticJsonDocument<256> doc;
  DeserializationError err = deserializeJson(doc, line);

  if (err) {
    Serial.println("JSON parse error");
    return;
  }

  // LED ON command
  // {"cmd": "power", "value": 1}
  // LED OFF command
  // {"cmd": "power", "value": 0}
  if (doc["cmd"] == "power") {
    bool value = doc["value"];
    led_state = value;
    digitalWrite(ledPin, value ? HIGH : LOW);
  }
}

