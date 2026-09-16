#include <SPI.h>
#include <MFRC522.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <GxEPD2_BW.h>


// RFID (Hardware SPI)
#define RFID_SS   53
#define RFID_RST  5

// E-Paper 1.54" (Hardware SPI 共用 51/52，使用獨立 CS 腳位)
#define EP_CS     4
#define EP_DC     6
#define EP_RST    12
#define EP_BUSY   13

// OLED (I2C)
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

// 超音波 / RGB / 繼電器
const int trigPin  = 2;
const int echoPin  = 3;
const int redPin   = 11;
const int greenPin = 10;
const int bluePin  = 9;
const int relayPin = 7;


MFRC522 rfid(RFID_SS, RFID_RST);
Adafruit_SSD1306 oled(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// E-Paper 宣告 (標準 4 參數建構子)
GxEPD2_BW<GxEPD2_154_D67, GxEPD2_154_D67::HEIGHT> epaper(
  GxEPD2_154_D67(EP_CS, EP_DC, EP_RST, EP_BUSY)
);

// 授權卡號 UID
byte myCardUID[7] = {0x04, 0x19, 0x37, 0xB2, 0xBA, 0x1A, 0x90};


void setRGB(int r, int g, int b) {
  analogWrite(redPin, r);
  analogWrite(greenPin, g);
  analogWrite(bluePin, b);
}

void showOLED(String statusMsg, String detailMsg) {
  oled.clearDisplay();
  oled.setTextSize(1);
  oled.setTextColor(SSD1306_WHITE);
  oled.setCursor(0, 0);
  oled.println("=== CityU EE Lock ===");
  oled.setCursor(0, 20);
  oled.println(statusMsg);
  oled.setTextSize(2);
  oled.setCursor(0, 42);
  oled.println(detailMsg);
  oled.display();
}

void drawMinionBase() {
  int cx = 100, cy = 90;
  epaper.fillRoundRect(cx - 45, cy - 60, 90, 120, 40, GxEPD_BLACK);
  epaper.fillRoundRect(cx - 42, cy - 57, 84, 114, 37, GxEPD_WHITE);
  epaper.fillRect(cx - 44, cy - 35, 88, 12, GxEPD_BLACK);
  epaper.fillCircle(cx, cy - 29, 24, GxEPD_BLACK);
  epaper.fillCircle(cx, cy - 29, 20, GxEPD_WHITE);
  
  epaper.fillRect(cx - 35, cy + 25, 70, 32, GxEPD_BLACK);
  epaper.drawLine(cx - 40, cy + 15, cx - 25, cy + 25, GxEPD_BLACK);
  epaper.drawLine(cx + 40, cy + 15, cx + 25, cy + 25, GxEPD_BLACK);
  epaper.fillCircle(cx - 24, cy + 26, 2, GxEPD_WHITE);
  epaper.fillCircle(cx + 24, cy + 26, 2, GxEPD_WHITE);
}

void epaperNormal() {
  int cx = 100, cy = 90;
  epaper.setPartialWindow(cx - 20, cy - 49, 40, 40);
  epaper.firstPage();
  do {
    epaper.fillCircle(cx, cy - 29, 14, GxEPD_WHITE);
    epaper.fillCircle(cx, cy - 29, 6, GxEPD_BLACK);
    epaper.fillCircle(cx + 2, cy - 31, 2, GxEPD_WHITE);
  } while (epaper.nextPage());
}

void epaperHappy() {
  int cx = 100, cy = 90;
  epaper.setPartialWindow(cx - 25, cy - 49, 50, 50);
  epaper.firstPage();
  do {
    epaper.fillCircle(cx, cy - 29, 15, GxEPD_WHITE);
    epaper.drawCircle(cx, cy - 29, 10, GxEPD_BLACK);
    epaper.fillRect(cx - 12, cy - 29, 24, 12, GxEPD_WHITE);
  } while (epaper.nextPage());
}

void epaperAngry() {
  int cx = 100, cy = 90;
  epaper.setPartialWindow(cx - 25, cy - 49, 50, 50);
  epaper.firstPage();
  do {
    epaper.fillCircle(cx, cy - 29, 15, GxEPD_WHITE);
    epaper.drawLine(cx - 8, cy - 37, cx + 8, cy - 21, GxEPD_BLACK);
    epaper.drawLine(cx + 8, cy - 37, cx - 8, cy - 21, GxEPD_BLACK);
  } while (epaper.nextPage());
}


void setup() {
  Serial.begin(115200);

  // 初始化硬體 SPI (由 RFID 與 E-Paper 共用)
  SPI.begin();
  rfid.PCD_Init();

  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);
  pinMode(relayPin, OUTPUT);
  digitalWrite(relayPin, LOW);

  setRGB(0, 0, 255);

  if(!oled.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("OLED 連線失敗！");
  }

  // 初始化 E-Paper
  epaper.init(115200, true, 50, false);
  epaper.setRotation(1);
  epaper.setFullWindow();
  epaper.firstPage();
  do {
    epaper.fillScreen(GxEPD_WHITE);
    drawMinionBase();
  } while (epaper.nextPage());

  epaperNormal();
  showOLED("Status: Ready", "Standby");
}

void loop() {
  // 1. 超音波感測
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  
  long duration = pulseIn(echoPin, HIGH);
  int distance = duration * 0.034 / 2;

  if (distance > 0 && distance < 15) {
    setRGB(255, 0, 0);   
    showOLED("Person Detected!", "Scan Card");
  } else {
    setRGB(0, 0, 255);   
    showOLED("Door Status:", "Standby");
  }

  // 2. 檢查 RFID
  if (!rfid.PICC_IsNewCardPresent() || !rfid.PICC_ReadCardSerial()) {
    delay(100);
    return;
  }

  // 3. 比對卡號
  bool isMatched = true;
  for (byte i = 0; i < 7; i++) {
    if (rfid.uid.uidByte[i] != myCardUID[i]) {
      isMatched = false;
      break;
    }
  }

  if (isMatched) {
    showOLED("Access Granted", "WELCOME!");
    setRGB(0, 255, 0);            
    digitalWrite(relayPin, HIGH); 
    epaperHappy();              
    
    delay(3000);                  
    
    digitalWrite(relayPin, LOW);  
    epaperNormal();             
  } else {
    showOLED("Access Denied", "ERROR!");
    epaperAngry();              
    
    for (int i = 0; i < 3; i++) {
      setRGB(255, 0, 0); delay(150);
      setRGB(0, 0, 0);   delay(150);
    }
    epaperNormal();             
  }

  rfid.PICC_HaltA();
  rfid.PCD_StopCrypto1();
}
