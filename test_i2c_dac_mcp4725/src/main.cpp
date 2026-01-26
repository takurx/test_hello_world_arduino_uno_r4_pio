#include <Arduino.h>
#include <Wire.h>

// MCP4725のI2Cアドレス (デフォルト: 0x60, A0ピンをVDDに接続した場合: 0x61)
#define MCP4725_ADDR 0x60

// MCP4725の12ビット最大値
#define MCP4725_MAX_VALUE 4095

// 2.5Vを出力するためのDAC値 (VDD=5Vの場合、中間値の2048)
#define OUTPUT_2_5V 2048

/**
 * MCP4725にDAC値を書き込む関数
 * @param value 0-4095の範囲のDAC値
 * @return true: 成功, false: 失敗
 */
bool setMCP4725Value(uint16_t value) {
  // 値の範囲チェック
  if (value > MCP4725_MAX_VALUE) {
    value = MCP4725_MAX_VALUE;
  }
  
  // Fast Write モード (DACレジスタのみに書き込み、EEPROMには書き込まない)
  Wire.beginTransmission(MCP4725_ADDR);
  
  // 上位4ビットと下位8ビットに分けて送信
  // Fast Writeモードコマンド: C1=0, C0=0 (上位2ビット)
  Wire.write((value >> 8) & 0x0F);  // 上位4ビット (D11-D8)
  Wire.write(value & 0xFF);          // 下位8ビット (D7-D0)
  
  uint8_t result = Wire.endTransmission();
  
  return (result == 0);  // 0 = 成功
}

/**
 * MCP4725に電圧値を書き込む関数
 * @param voltage 出力したい電圧 (V)
 * @param vdd 電源電圧 (V) デフォルト: 5.0V
 * @return true: 成功, false: 失敗
 */
bool setMCP4725Voltage(float voltage, float vdd = 5.0) {
  // 電圧をDAC値に変換
  uint16_t dacValue = (uint16_t)((voltage / vdd) * MCP4725_MAX_VALUE);
  return setMCP4725Value(dacValue);
}

void setup() {
  // シリアル通信の初期化
  Serial.begin(115200);
  while (!Serial) {
    ; // シリアルポートの接続を待つ
  }
  
  Serial.println("=== MCP4725 DAC Test ===");
  Serial.println("Arduino UNO R4 WiFi");
  
  // I2C通信の初期化
  Wire.begin();
  Serial.println("I2C initialized");
  
  // MCP4725が接続されているか確認
  Wire.beginTransmission(MCP4725_ADDR);
  uint8_t error = Wire.endTransmission();
  
  if (error == 0) {
    Serial.print("MCP4725 found at address 0x");
    Serial.println(MCP4725_ADDR, HEX);
  } else {
    Serial.print("Error: MCP4725 not found at address 0x");
    Serial.println(MCP4725_ADDR, HEX);
    Serial.println("Please check connections!");
    return;
  }
  
  // 2.5Vを出力
  Serial.println("\nSetting output to 2.5V...");
  
  if (setMCP4725Voltage(2.5, 5.0)) {
    Serial.println("Success! Output voltage: 2.5V");
    Serial.print("DAC Value: ");
    Serial.print(OUTPUT_2_5V);
    Serial.print(" / ");
    Serial.println(MCP4725_MAX_VALUE);
  } else {
    Serial.println("Failed to set DAC value!");
  }
  
  Serial.println("\n--- Setup Complete ---");
}

void loop() {
  // 初期設定で2.5Vが出力されているため、特に処理なし
  // MCP4725はパワーダウンモードに入るまで出力を保持します
  
  delay(1000);
}
