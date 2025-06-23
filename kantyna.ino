/*
 * Advanced Cafeteria Management System with Firebase Integration
 * Hardware: ESP32 + RFID-RC522 + 4x4 Keypad + LCD1602
 * Features: User balance management, transaction logging, WiFi connectivity
 */

#include <SPI.h>
#include <MFRC522.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <WiFiClientSecure.h>
#include "config.h"  // Include configuration file with WiFi and Firebase credentials

// RFID pins - fixed from non-functional version
#define RFID_SS_PIN  5
#define RFID_RST_PIN 21

// Keypad configuration
#define KEYPAD_ROWS 4
#define KEYPAD_COLS 4

char keys[KEYPAD_ROWS][KEYPAD_COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};

// Keypad pin definitions - corrected from user's pin mapping
byte rowPins[KEYPAD_ROWS] = {22, 19, 12, 13}; // ROW1, ROW2, ROW3, ROW4
byte colPins[KEYPAD_COLS] = {35, 32, 16, 23}; // COL1, COL2, COL3, COL4

// Initialize instances
MFRC522 rfid(RFID_SS_PIN, RFID_RST_PIN);
LiquidCrystal_I2C lcd(0x27, 16, 2); // I2C address may need adjustment
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, KEYPAD_ROWS, KEYPAD_COLS);

// System states
enum SystemState {
  WAITING_FOR_CARD,
  CARD_DETECTED,
  AMOUNT_ENTRY,
  PROCESSING,
  SHOW_BALANCE,
  ERROR_STATE
};

enum OperationType {
  OPERATION_ADD,
  OPERATION_DEDUCT
};

SystemState currentState = WAITING_FOR_CARD;
OperationType currentOperation = OPERATION_ADD;
String inputAmount = "";
String currentCardUID = "";
float currentBalance = 0.0;
bool wifiConnected = false;
unsigned long lastDisplayUpdate = 0;
const unsigned long displayUpdateDelay = 100;

void setup() {
  Serial.begin(115200);
  
  // Initialize LCD first for status display
  Wire.begin();
  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.print("Starting up...");
  
  // Initialize WiFi
  connectToWiFi();
  
  // Initialize SPI and RFID
  SPI.begin();
  rfid.PCD_Init();
  
  // Final setup message
  lcd.clear();
  if (wifiConnected) {
    lcd.print("System Ready");
    lcd.setCursor(0, 1);
    lcd.print("Scan your card");
  } else {
    lcd.print("WiFi Failed!");
    lcd.setCursor(0, 1);
    lcd.print("Local mode only");
  }
  
  Serial.println("System initialized");
  delay(2000);
  
  lcd.clear();
  lcd.print("Ready for card...");
}

void loop() {
  // Check WiFi connection periodically
  if (millis() % 30000 == 0) {
    checkWiFiConnection();
  }
  
  switch(currentState) {
    case WAITING_FOR_CARD:
      handleCardWaiting();
      break;
      
    case CARD_DETECTED:
      handleCardDetected();
      break;
      
    case AMOUNT_ENTRY:
      handleAmountEntry();
      break;
      
    case PROCESSING:
      handleProcessing();
      break;
      
    case SHOW_BALANCE:
      handleShowBalance();
      break;
      
    case ERROR_STATE:
      handleError();
      break;
  }
}

void handleCardWaiting() {
  if (rfid.PICC_IsNewCardPresent() && rfid.PICC_ReadCardSerial()) {
    currentCardUID = getCardUID();
    Serial.println("Card detected: " + currentCardUID);
    
    lcd.clear();
    lcd.print("Card detected!");
    lcd.setCursor(0, 1);
    lcd.print("Loading...");
    
    // Get user balance from Firebase
    if (wifiConnected) {
      currentBalance = getUserBalance(currentCardUID);
      if (currentBalance >= 0) {
        currentState = CARD_DETECTED;
      } else {
        lcd.clear();
        lcd.print("Card not found!");
        lcd.setCursor(0, 1);
        lcd.print("Register first");
        delay(2000);
        currentState = WAITING_FOR_CARD;
      }
    } else {
      // Offline mode - assume balance exists
      currentBalance = 0.0;
      currentState = CARD_DETECTED;
    }
    
    rfid.PICC_HaltA();
    rfid.PCD_StopCrypto1();
  }
}

void handleCardDetected() {
  lcd.clear();
  lcd.print("Balance: $" + String(currentBalance, 2));
  lcd.setCursor(0, 1);
  lcd.print("1:Add 2:Buy 3:Bal");
  
  char key = keypad.getKey();
  if (key) {
    switch (key) {
      case '1': // Add money
        currentOperation = OPERATION_ADD;
        lcd.clear();
        lcd.print("Add amount:");
        inputAmount = "";
        currentState = AMOUNT_ENTRY;
        updateAmountDisplay();
        break;
        
      case '2': // Buy/Deduct money
        currentOperation = OPERATION_DEDUCT;
        lcd.clear();
        lcd.print("Deduct amount:");
        inputAmount = "";
        currentState = AMOUNT_ENTRY;
        updateAmountDisplay();
        break;
        
      case '3': // Show balance
        currentState = SHOW_BALANCE;
        break;
        
      case '*': // Go back
        currentState = WAITING_FOR_CARD;
        lcd.clear();
        lcd.print("Scan your card...");
        break;
    }
  }
}

void handleAmountEntry() {
  char key = keypad.getKey();
  
  if (key) {
    if (key >= '0' && key <= '9' && inputAmount.length() < 6) {
      inputAmount += key;
      updateAmountDisplay();
    }
    else if (key == '#') { // Confirm amount
      if (inputAmount.length() > 0) {
        float amount = inputAmount.toFloat();
        if (amount > 0) {
          currentState = PROCESSING;
        } else {
          lcd.clear();
          lcd.print("Invalid amount!");
          delay(1500);
          currentState = CARD_DETECTED;
        }
      }
    }
    else if (key == '*') { // Clear amount or go back
      if (inputAmount.length() > 0) {
        inputAmount = "";
        updateAmountDisplay();
      } else {
        currentState = CARD_DETECTED;
      }
    }
    else if (key == 'D') { // Decimal point
      if (inputAmount.indexOf('.') == -1 && inputAmount.length() > 0) {
        inputAmount += ".";
        updateAmountDisplay();
      }
    }
  }
}

void handleProcessing() {
  lcd.clear();
  lcd.print("Processing...");
  lcd.setCursor(0, 1);
  lcd.print("Please wait...");
  
  float amount = inputAmount.toFloat();
  bool isAddOperation = (currentOperation == OPERATION_ADD);
  
  // Check if operation is valid
  bool transactionSuccess = false;
  
  if (wifiConnected) {
    // Update balance in Firebase
    transactionSuccess = updateUserBalance(currentCardUID, amount, isAddOperation);
  } else {
    // Offline mode - just update local balance
    if (isAddOperation) {
      currentBalance += amount;
      transactionSuccess = true;
    } else {
      if (currentBalance >= amount) {
        currentBalance -= amount;
        transactionSuccess = true;
      } else {
        transactionSuccess = false;
      }
    }
  }
  
  // Show result
  lcd.clear();
  if (transactionSuccess) {
    lcd.print(isAddOperation ? "Money Added!" : "Purchase Done!");
    lcd.setCursor(0, 1);
    lcd.print("New: $" + String(currentBalance, 2));
    
    // Log transaction
    if (wifiConnected) {
      logTransaction(currentCardUID, amount, isAddOperation);
    }
  } else {
    lcd.print("Transaction Failed!");
    lcd.setCursor(0, 1);
    if (!isAddOperation && currentBalance < amount) {
      lcd.print("Insufficient funds");
    } else {
      lcd.print("Network error");
    }
  }
  
  delay(3000);
  
  // Reset to card detected state
  currentState = CARD_DETECTED;
  inputAmount = "";
}

void handleShowBalance() {
  lcd.clear();
  lcd.print("Current Balance:");
  lcd.setCursor(0, 1);
  lcd.print("$" + String(currentBalance, 2));
  
  char key = keypad.getKey();
  if (key) {
    currentState = CARD_DETECTED;
  }
}

void handleError() {
  lcd.clear();
  lcd.print("System Error!");
  lcd.setCursor(0, 1);
  lcd.print("Please restart");
  
  delay(5000);
  currentState = WAITING_FOR_CARD;
}

void updateAmountDisplay() {
  lcd.setCursor(0, 1);
  lcd.print("$" + inputAmount + "      ");
}

String getCardUID() {
  String cardUID = "";
  for (byte i = 0; i < rfid.uid.size; i++) {
    if (rfid.uid.uidByte[i] < 0x10) {
      cardUID += "0";
    }
    cardUID += String(rfid.uid.uidByte[i], HEX);
  }
  cardUID.toUpperCase();
  return cardUID;
}

// WiFi and Firebase Helper Functions
void connectToWiFi() {
  lcd.setCursor(0, 1);
  lcd.print("Connecting WiFi...");
  
  WiFi.begin(ssid, password);
  int attempts = 0;
  
  while (WiFi.status() != WL_CONNECTED && attempts < 20) {
    delay(500);
    Serial.print(".");
    attempts++;
  }
  
  if (WiFi.status() == WL_CONNECTED) {
    wifiConnected = true;
    Serial.println("");
    Serial.println("WiFi connected!");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
  } else {
    wifiConnected = false;
    Serial.println("WiFi connection failed!");
  }
}

void checkWiFiConnection() {
  if (WiFi.status() != WL_CONNECTED) {
    wifiConnected = false;
    Serial.println("WiFi disconnected! Attempting reconnection...");
    connectToWiFi();
  }
}

float getUserBalance(String cardUID) {
  if (!wifiConnected) return -1;
  
  HTTPClient http;
  String url = "https://" + String(FIREBASE_HOST) + "/users/" + cardUID + "/balance.json?auth=" + String(FIREBASE_AUTH);
  
  http.begin(url);
  int httpResponseCode = http.GET();
  
  if (httpResponseCode == 200) {
    String response = http.getString();
    http.end();
    
    if (response == "null") {
      // User doesn't exist, create with 0 balance
      createUser(cardUID);
      return 0.0;
    } else {
      return response.toFloat();
    }
  } else {
    Serial.println("Error getting user balance: " + String(httpResponseCode));
    http.end();
    return -1;
  }
}

bool createUser(String cardUID) {
  if (!wifiConnected) return false;
  
  HTTPClient http;
  String url = "https://" + String(FIREBASE_HOST) + "/users/" + cardUID + ".json?auth=" + String(FIREBASE_AUTH);
  
  http.begin(url);
  http.addHeader("Content-Type", "application/json");
  
  DynamicJsonDocument doc(200);
  doc["balance"] = 0.0;
  doc["created"] = millis();
  doc["name"] = "User " + cardUID.substring(0, 4);
  
  String jsonString;
  serializeJson(doc, jsonString);
  
  int httpResponseCode = http.PUT(jsonString);
  http.end();
  
  return httpResponseCode == 200;
}

bool updateUserBalance(String cardUID, float amount, bool isAddOperation) {
  if (!wifiConnected) return false;
  
  // First get current balance
  float newBalance;
  if (isAddOperation) {
    newBalance = currentBalance + amount;
  } else {
    if (currentBalance >= amount) {
      newBalance = currentBalance - amount;
    } else {
      return false; // Insufficient funds
    }
  }
  
  // Update balance in Firebase
  HTTPClient http;
  String url = "https://" + String(FIREBASE_HOST) + "/users/" + cardUID + "/balance.json?auth=" + String(FIREBASE_AUTH);
  
  http.begin(url);
  http.addHeader("Content-Type", "application/json");
  
  int httpResponseCode = http.PUT(String(newBalance));
  http.end();
  
  if (httpResponseCode == 200) {
    currentBalance = newBalance;
    return true;
  } else {
    Serial.println("Error updating balance: " + String(httpResponseCode));
    return false;
  }
}

void logTransaction(String cardUID, float amount, bool isAddOperation) {
  if (!wifiConnected) return;
  
  HTTPClient http;
  String url = "https://" + String(FIREBASE_HOST) + "/transactions.json?auth=" + String(FIREBASE_AUTH);
  
  http.begin(url);
  http.addHeader("Content-Type", "application/json");
  
  DynamicJsonDocument doc(300);
  doc["cardUID"] = cardUID;
  doc["amount"] = amount;
  doc["type"] = isAddOperation ? "add" : "deduct";
  doc["timestamp"] = millis();
  doc["balance_after"] = currentBalance;
  
  String jsonString;
  serializeJson(doc, jsonString);
  
  int httpResponseCode = http.POST(jsonString);
  http.end();
  
  if (httpResponseCode != 200) {
    Serial.println("Error logging transaction: " + String(httpResponseCode));
  }
}