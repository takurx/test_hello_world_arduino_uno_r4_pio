#include <Arduino.h>

// int adc_resolution = 10; // Default ADC resolution
// int adc_resolution = 12; // Default ADC resolution
int adc_resolution = 14; // Default ADC resolution

void setup() {
  Serial.begin(115200);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB
  }
  Serial.println("Hello, ADC!");

  // analogReadResolution(10); // Set ADC resolution to 10 bits (0-1023)
  // analogReadResolution(12); // Set ADC resolution to 12 bits (0-4095)
  // analogReadResolution(14); // Set ADC resolution to 14 bits (0-16383)
  analogReadResolution(adc_resolution); // Set ADC resolution
}

void loop() {
  int sensorValue_A0 = analogRead(A0); // read the input on analog pin 0
  int sensorValue_A1 = analogRead(A1); // read the input on analog pin 1

  float voltage_A0 = sensorValue_A0 * (5.0 / (pow(2, adc_resolution) - 1)); // Convert to voltage
  float voltage_A1 = sensorValue_A1 * (5.0 / (pow(2, adc_resolution) - 1)); // Convert to voltage
  
  float calculate_voltage_A0 = voltage_A0 * ((47000.0 + 20000.0) / 20000.0); // Voltage divider calculation
  float calculate_voltage_A1 = voltage_A1 * ((47000.0 + 20000.0) / 20000.0); // Voltage divider calculation

  Serial.print("Analog reading from A0: ");
  Serial.println(sensorValue_A0);
  Serial.print("Voltage at A0: ");
  Serial.println(voltage_A0);
  Serial.print("Calculated Voltage at A0: ");
  Serial.println(calculate_voltage_A0);
  
  Serial.print("Analog reading from A1: ");
  Serial.println(sensorValue_A1);
  Serial.print("Voltage at A1: ");
  Serial.println(voltage_A1);
  Serial.print("Calculated Voltage at A1: ");
  Serial.println(calculate_voltage_A1);

  delay(1000); // wait for a second
}
