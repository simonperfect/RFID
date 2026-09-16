#include <SPI.h>
#include <MFRC522.h>

// CityU EE Board / Arduino Mega 的 RFID 引腳設定
#define SS_PIN 53
#define RST_PIN 5

MFRC522 rfid(SS_PIN, RST_PIN);

void setup() {
  Serial.begin(115200);
  SPI.begin();       // 初始化 SPI 匯流排
  rfid.PCD_Init();   // 初始化 RFID 模組
  Serial.println("請將 RFID 卡片靠近感應區...");
}

void loop() {
  // 檢查是否有新卡片靠近
  if (!rfid.PICC_IsNewCardPresent() || !rfid.PICC_ReadCardSerial()) {
    return;
  }

  // 讀取並印出卡片的 UID (Hex 格式)
  Serial.print("偵測到卡片 UID: ");
  for (byte i = 0; i < rfid.uid.size; i++) {
    if (rfid.uid.uidByte[i] < 0x10) Serial.print(" 0");
    else Serial.print(" ");
    Serial.print(rfid.uid.uidByte[i], HEX);
  }
  Serial.println();

  // 停止卡片讀取，避免重複感應
  rfid.PICC_HaltA();
  rfid.PCD_StopCrypto1();
}