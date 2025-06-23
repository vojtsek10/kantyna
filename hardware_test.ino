/*
 * Hardware Test Sketch for ESP32 Cafeteria System
 * Use this to verify all hardware components are working correctly
 * before running the main kantyna.ino program
 */

#include <SPI.h>
#include <MFRC522.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>

// Pin definitions (same as main project)
#define SS_PIN 5
#define RST_PIN 2
#define KEYPAD_ROWS 4
#define KEYPAD_COLS 4

// Hardware objects
MFRC522 mfrc522(SS_PIN, RST_PIN);
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Keypad setup
byte rowPins[KEYPAD_ROWS] = {13, 12, 14, 27};
byte colPins[KEYPAD_COLS] = {26, 25, 33, 32};
char keys[KEYPAD_ROWS][KEYPAD_COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, KEYPAD_ROWS, KEYPAD_COLS);

void setup() {
  Serial.begin(115200);
  Serial.println("ESP32 Cafeteria Hardware Test");
  Serial.println("=============================");
  
  // Test LCD
  Serial.println("Testing LCD...");
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("LCD Test OK");
  lcd.setCursor(0, 1);
  lcd.print("Hardware Check");
  delay(2000);
  
  // Test RFID
  Serial.println("Testing RFID...");
  SPI.begin();
  mfrc522.PCD_Init();
  
  if (mfrc522.PCD_PerformSelfTest()) {
    Serial.println("RFID Self-test: PASSED");
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("RFID: OK");
  } else {
    Serial.println("RFID Self-test: FAILED");
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("RFID: FAILED");
  }
  
  // Reinitialize RFID after self-test
  mfrc522.PCD_Init();
  
  delay(2000);
  
  // Test complete
  Serial.println("\nHardware test complete!");
  Serial.println("Instructions:");
  Serial.println("1. Press keys on keypad - they should appear on Serial Monitor");
  Serial.println("2. Place RFID card near reader - UID should be displayed");
  Serial.println("3. Check LCD for clear, readable text");
  
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Test Complete");
  lcd.setCursor(0, 1);
  lcd.print("Press keys/RFID");
}

void loop() {
  // Test keypad
  char key = keypad.getKey();
  if (key) {
    Serial.print("Key pressed: ");
    Serial.println(key);
    lcd.setCursor(15, 0);
    lcd.print(key);
  }
  
  // Test RFID
  if (mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial()) {
    Serial.print("Card detected - UID: ");
    String uid = "";
    for (byte i = 0; i < mfrc522.uid.size; i++) {
      uid += String(mfrc522.uid.uidByte[i] < 0x10 ? "0" : "");
      uid += String(mfrc522.uid.uidByte[i], HEX);
    }
    uid.toUpperCase();
    Serial.println(uid);
    
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Card: ");
    lcd.print(uid.substring(0, 10));
    lcd.setCursor(0, 1);
    lcd.print("Press key to cont");
    
    mfrc522.PICC_HaltA();
    delay(2000);
    
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Test Complete");
    lcd.setCursor(0, 1);
    lcd.print("Press keys/RFID");
  }
  
  delay(100);
}