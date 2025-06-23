int promenna = 0;
#include <MFRC522v2.h>
#include <MFRC522DriverSPI.h>
//#include <MFRC522DriverI2C.h>
#include <MFRC522DriverPinSimple.h>
#include <MFRC522Debug.h>

#include <WiFi.h>
#include <WebServer.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#include <Key.h>
#include <Keypad.h>
// RFID čtečka - ukázka čtení adres tagů

// připojení knihoven SPI a MFRC522
#include <SPI.h>
#include <MFRC522.h>

// definování pinů pro SDA a RST
#define SDA_PIN 14
#define SCK_PIN 27
#define MOSI_PIN 26
#define MISO_PIN 25
#define RST_PIN 33

// vytvoření instance RFID čtečky z knihovny
MFRC522 mfrc522(SDA_PIN, RST_PIN);


LiquidCrystal_I2C lcd((0x27), 16, 2);

const byte ROWS = 4;  // 4 řádky
const byte COLS = 4;  // 4 sloupce

char hexaKeys[ROWS][COLS] = {
  { '1', '2', '3', 'A' },
  { '4', '5', '6', 'B' },
  { '7', '8', '9', 'C' },
  { '*', '0', '#', 'D' }
};

byte colPins[ROWS] = { 35, 32, 16, 23 };  // piny pro radky
byte rowPins[COLS] = { 22, 19, 12, 13 };  // piny pro sloupce

Keypad customKeypad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);

void vypisHex(byte *buffer, byte bufferSize) {
  for (byte i = 0; i < bufferSize; i++) {
    Serial.print(buffer[i] < 0x10 ? " 0" : " ");
    Serial.print(buffer[i], HEX);
  }
}
void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  // inicializace komunikace přes SPI
  SPI.end();
  SPI.begin(SCK_PIN, MISO_PIN, MOSI_PIN, SDA_PIN);
  // inicializace komunikace s RFID čtečkou
  mfrc522.PCD_Init();

  lcd.init();
  lcd.backlight();
  Serial.print("Zapis_1");
  //attachInterrupt(Set_Button, isr, FALLING);
}

void loop() {
  //Serial.print("Zapis_2");
  // put your main code here, to run repeatedly:
  if (!mfrc522.PICC_IsNewCardPresent())
    return;
  // kontrola správného přečtení RFID tagu
  if (!mfrc522.PICC_ReadCardSerial())
    return;
  // výpis informace o verzi RFID tagu
  MFRC522::PICC_Type piccType = mfrc522.PICC_GetType(mfrc522.uid.sak);
  Serial.print("RFID tag typu: ");
  Serial.println(mfrc522.PICC_GetTypeName(piccType));
  // kontrola podporovaných typů RFID tagu
  if (piccType != MFRC522::PICC_TYPE_MIFARE_MINI && piccType != MFRC522::PICC_TYPE_MIFARE_1K && piccType != MFRC522::PICC_TYPE_MIFARE_4K) {
    Serial.println("Tento RFID tag neni podporovany (typ MIFARE Classic).");
    return;
  } 
  else {
    Serial.println("Detekovan neznamy RFID tag!");
  }
  // výpis adresy RFID tagu v hexa formátu
  Serial.print("Adresa RFID tagu: ");
  vypisHex(mfrc522.uid.uidByte, mfrc522.uid.size);
  Serial.println();


  /* ukázka přidání dalšího tagu
  else if(mfrc522.uid.uidByte[0] == 0x?? & mfrc522.uid.uidByte[1] == 0x?? & mfrc522.uid.uidByte[2] == 0x?? & mfrc522.uid.uidByte[3] == 0x??) {
    Serial.println("Detekovan novy tag ..!");
  } 
   */

  Serial.println();
  // ukončení komunikace a jejího zabezpečení
  mfrc522.PICC_HaltA();
  mfrc522.PCD_StopCrypto1();

  // podprogram pro výpis adresy RFID tagu v hexa formátu


  lcd.setCursor(0, 0);
  if (mfrc522.uid.uidByte[0] == 0x45 && mfrc522.uid.uidByte[1] == 0x06 && mfrc522.uid.uidByte[2] == 0xF4 && mfrc522.uid.uidByte[3] == 0x07) {
    Serial.println("Detekovana bila karta!");
    lcd.print("Detekovana bila karta!");
    promenna = 1;
  } 
  else if (mfrc522.uid.uidByte[0] == 0x83 && mfrc522.uid.uidByte[1] == 0xDF && mfrc522.uid.uidByte[2] == 0x60 && mfrc522.uid.uidByte[3] == 0x44) {
    Serial.println("Detekovana ISIC karta!");
    lcd.print("Detekovana ISIC karta!");
    promenna = 3;
  } 
  else if (mfrc522.uid.uidByte[0] == 0x86 && mfrc522.uid.uidByte[1] == 0x39 && mfrc522.uid.uidByte[2] == 0x5D && mfrc522.uid.uidByte[3] == 0x30) {
    Serial.println("Detekovan modry cip!");
    lcd.print("Detekovan modry cip!");
    promenna = 2;
  } 
  else {
    char button = customKeypad.getKey();
    if (button) {
      Serial.println(button);
      /*lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Vas zustatek je:");
      lcd.setCursor(0, 1);
      lcd.print(button);*/
    }
  }
}
//https://randomnerdtutorials.com/esp32-mfrc522-rfid-reader-arduino/
