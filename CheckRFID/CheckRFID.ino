#include <SPI.h>
#include <MFRC522.h>


#define SS_PIN 53
#define RST_PIN 5

MFRC522 rfid(SS_PIN, RST_PIN);

void setup() {
  Serial.begin(115200);
  SPI.begin();     
  rfid.PCD_Init(); 
  Serial.println("Please place RFID near the reader...");
}

void loop() {

  if (!rfid.PICC_IsNewCardPresent() || !rfid.PICC_ReadCardSerial()) {
    return;
  }


  Serial.print("detected UID: ");
  for (byte i = 0; i < rfid.uid.size; i++) {
    if (rfid.uid.uidByte[i] < 0x10) Serial.print(" 0");
    else Serial.print(" ");
    Serial.print(rfid.uid.uidByte[i], HEX);
  }
  Serial.println();


  rfid.PICC_HaltA();
  rfid.PCD_StopCrypto1();
}
