// 入門編2　Lesson 04　サンプルスケッチ1
// DAC機能確認
// https://omoroya.com/
// Arduino 入門2 Lesson 04 【DAC機能確認】 | おもろ家
// https://omoroya.com/arduino-2-lesson04/

#include <Arduino.h>

void setup(){
  //analogWriteResolution(10);  //DAC分解能を10bit(0～1023)に設定
  analogWriteResolution(12);  //DAC分解能を12bit(0～4095)に設定
}

void loop(){
  int target_percentage = 100;    //出力目標値(%)
  int value = (4095 * target_percentage) / 100; //12bit分, 4095 = 2^12 -1
  // int value;
  
  analogWrite(A0, value);           //DACの4095の値をA0に出力
  delayMicroseconds(30);          //変換時間待機
}
