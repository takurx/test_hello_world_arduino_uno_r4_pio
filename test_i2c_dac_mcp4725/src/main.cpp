#include <Arduino.h>
#include <Wire.h>

// MCP4725のI2Cアドレス (デフォルト: 0x60, A0ピンをVDDに接続した場合: 0x61)
#define MCP4725_ADDR 0x60

// MCP4725の12ビット最大値
#define MCP4725_MAX_VALUE 4095

// targetのDAC値 (VDD=5Vの場合、中間値の2048)
//#define TARGET_VALUE 2048 //

// targetの電圧値
#define TARGET_VOLTAGE 2.5

// VDDの理想的な値
#define VDD_IDEAL_VOLTAGE 5.00
// VDDの実際の値
#define VDD_MEASURED_VOLTAGE 4.64

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
  
  Serial.print("\nSetting output to ");
  Serial.print(TARGET_VOLTAGE);
  Serial.println(" V...");
  
  float target_voltage = TARGET_VOLTAGE;
  float ideal_voltage = VDD_IDEAL_VOLTAGE;
  float measured_voltage = VDD_MEASURED_VOLTAGE;
  float ideal_resolution_voltage = ideal_voltage / MCP4725_MAX_VALUE;
  float actual_resolution_voltage = measured_voltage / MCP4725_MAX_VALUE;
  int dac_value = (int)(target_voltage/measured_voltage * MCP4725_MAX_VALUE);

  if (setMCP4725Voltage(target_voltage, measured_voltage)) {
    Serial.print("Success! Output voltage: ");
    float actual_voltage = actual_resolution_voltage * dac_value ;
    Serial.print(actual_voltage);
    Serial.println(" V (based on measured VDD)");
    Serial.print("DAC Value: ");
    Serial.print(dac_value);
    Serial.print(" / ");
    Serial.println(MCP4725_MAX_VALUE);
  } else {
    Serial.println("Failed to set DAC value!");
  }
  
  Serial.println("\n--- Setup Complete ---");
  Serial.print("Expected Voltage: ");
  Serial.print(target_voltage);
  Serial.print(" V, Ideal Voltage  (based on VDD=: ");
  Serial.print(ideal_voltage);
  Serial.print("V): ");
  Serial.print(ideal_resolution_voltage * dac_value);
  Serial.print(" V, Actual Voltage (based on VDD=");
  Serial.print(measured_voltage);
  Serial.print("V): ");
  Serial.print(actual_resolution_voltage * dac_value);
  Serial.println("V");
}

void loop() {
  // 初期設定で1.25Vが出力されているため、特に処理なし
  // MCP4725はパワーダウンモードに入るまで出力を保持します
  
  delay(1000);
}
