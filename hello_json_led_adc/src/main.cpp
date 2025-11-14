/*
  hello_json_led.ino - Example of JSON command processing on Arduino UNO R4 WiFi

  This example demonstrates how to receive JSON commands over Serial to control an LED,
  and periodically send status updates in JSON format.

  Commands:
    {"cmd": "power", "value": 1}  // Turn LED ON
    {"cmd": "power", "value": 0}  // Turn LED OFF

  Status Update (every second):
    {"led_state": true/false, "voltage1": float, "voltage2": float}

  Requires ArduinoJson library.

  created with ChatGPT
*/
#include <Arduino.h>
#include <ArduinoJson.h>

void parseJsonLine(String &line);

String buffer = "";

// 状態変数（例として初期値をセット）
bool  led_state = false;
float voltage   = 12.34;
float current   = 1.23;

unsigned long last_status_time = 0;  // 1秒タイマー用

int ledPin = 7; // LEDが接続されているピン番号

// int adc_resolution = 10; // Default ADC resolution
// int adc_resolution = 12; // Default ADC resolution
int adc_resolution = 14; // Default ADC resolution

void setup() {
  Serial.begin(115200);
  pinMode(ledPin, OUTPUT);

  // analogReadResolution(10); // Set ADC resolution to 10 bits (0-1023)
  // analogReadResolution(12); // Set ADC resolution to 12 bits (0-4095)
  // analogReadResolution(14); // Set ADC resolution to 14 bits (0-16383)
  analogReadResolution(adc_resolution); // Set ADC resolution
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
    int sensorValue_A0 = 0;
    int sensorValue_A1 = 0;
    int average_count = 16;
    float calibration_factor_a0 = 0.930242511; // 校正係数
    float calibration_term_a0 = 13.545; // 校正項
    float calibration_factor_a1 = 0.932370603; // 校正係数
    float calibration_term_a1 = 14.636; // 校正項

    for(int i=0; i<average_count; i++){
      sensorValue_A0 += analogRead(A0); // read the input on analog pin 0
      sensorValue_A1 += analogRead(A1); // read the input on analog pin 1
    }
    sensorValue_A0 /= average_count;
    sensorValue_A1 /= average_count;

    float calibrated_sensorValue_A0 = sensorValue_A0 * calibration_factor_a0 + calibration_term_a0;
    float calibrated_sensorValue_A1 = sensorValue_A1 * calibration_factor_a1 + calibration_term_a1;

    float voltage_A0 = calibrated_sensorValue_A0 * (5.0 / (pow(2, adc_resolution) - 1)); // Convert to voltage
    float voltage_A1 = calibrated_sensorValue_A1 * (5.0 / (pow(2, adc_resolution) - 1)); // Convert to voltage
    
    float calculate_voltage_A0 = voltage_A0 * ((47000.0 + 20000.0) / 20000.0); // Voltage divider calculation
    float calculate_voltage_A1 = voltage_A1 * ((47000.0 + 20000.0) / 20000.0); // Voltage divider calculation

    last_status_time = now;

    StaticJsonDocument<128> doc;
    doc["led_state"] = led_state;
    doc["voltage1"]   = calculate_voltage_A0;
    doc["voltage2"]   = calculate_voltage_A1;
    doc["raw_a0"]    = sensorValue_A0;
    doc["raw_a1"]    = sensorValue_A1;

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

