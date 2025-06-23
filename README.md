# Cafeteria Management System with ESP32, RFID, and Firebase

A comprehensive cafeteria management system using ESP32, RFID RC522, 4x4 keypad, LCD display, and Firebase database for user authentication, balance management, and transaction logging.

## Hardware Components

- **ESP32 WROOM-32**: Main microcontroller with WiFi capability
- **MFRC522 RFID Reader**: For card authentication
- **LCM1602 LCD Display**: User interface (I2C)
- **4x4 Membrane Keypad**: User input
- **RFID Cards/Tags**: User identification

## Pin Connections

### RFID RC522 to ESP32
- VCC → 3.3V
- GND → GND
- MISO → GPIO 19
- MOSI → GPIO 23
- SCK → GPIO 18
- SDA → GPIO 5
- RST → GPIO 2

### LCD I2C to ESP32
- VCC → 5V or 3.3V (depending on LCD module)
- GND → GND
- SDA → GPIO 21
- SCL → GPIO 22

### 4x4 Keypad to ESP32
- Row pins: GPIO 13, 12, 14, 27
- Column pins: GPIO 26, 25, 33, 32

## Features

### Core Functionality
- **RFID Authentication**: Secure card-based user identification
- **Real-time Balance Management**: Add funds and make purchases
- **4x4 Keypad Interface**: Intuitive amount entry and navigation
- **LCD Display**: Clear user feedback and transaction details
- **Firebase Integration**: Cloud-based user data and transaction storage
- **WiFi Connectivity**: Online and offline operation modes

### User Operations
- **Add Money**: Top up account balance
- **Purchase Items**: Deduct money from balance
- **Balance Inquiry**: Check current account balance
- **Transaction Logging**: All operations recorded in Firebase

### System Features
- **State Machine Logic**: Robust system state management
- **Error Handling**: Comprehensive error detection and user feedback
- **Automatic User Creation**: New RFID cards automatically registered
- **Offline Mode**: Basic functionality when WiFi is unavailable
- **Security**: Configuration file separation for credentials

## Required Libraries

Install these libraries via Arduino IDE Library Manager:

1. **MFRC522** by GithubCommunity - RFID functionality
2. **LiquidCrystal I2C** by Frank de Brabander - LCD control  
3. **Keypad** by Mark Stanley - Keypad input handling
4. **ArduinoJson** by Benoit Blanchon - JSON parsing for Firebase
5. **WiFi** (ESP32 Core) - Built-in WiFi functionality
6. **HTTPClient** (ESP32 Core) - HTTP requests for Firebase API

## Setup Instructions

### 1. Hardware Setup
- Wire all components according to pin connections above
- Test individual components before integration
- Ensure proper power supply (recommend using USB power)

### 2. Firebase Setup
1. Create a Firebase project at [Firebase Console](https://console.firebase.google.com/)
2. Enable Realtime Database
3. Get your project's database URL (format: `project-id-default-rtdb.firebaseio.com`)
4. Generate a database secret key for authentication
5. Set database rules for read/write access

### 3. Software Setup
1. Install all required libraries in Arduino IDE
2. Copy `config_template.h` to `config.h`
3. Update `config.h` with your WiFi and Firebase credentials
4. Upload the code to ESP32

### 4. Database Structure
The system automatically creates this structure in Firebase:
```json
{
  "users": {
    "card_uid": {
      "balance": 0.0,
      "created": "timestamp"
    }
  },
  "transactions": {
    "transaction_id": {
      "card_uid": "user_card_id",
      "type": "add/deduct",
      "amount": 10.0,
      "balance_after": 50.0,
      "timestamp": "2024-01-01T12:00:00Z"
    }
  }
}
```

## Usage Instructions

1. **Power on the system** - LCD shows "Connecting WiFi..."
2. **Scan RFID card** - Place card near reader
3. **Select operation**:
   - Press `1` for Add Money
   - Press `2` for Buy/Deduct Money
   - Press `3` for Show Balance
   - Press `4` to Go Back
4. **Enter amount** (for add/deduct operations)
5. **Confirm transaction** with `#` or cancel with `*`

## System States

- **WAITING_FOR_CARD**: Ready to scan RFID card
- **CARD_DETECTED**: Card scanned, showing menu options
- **AMOUNT_ENTRY**: User entering amount via keypad
- **PROCESSING**: Processing transaction with Firebase
- **SHOW_BALANCE**: Displaying current balance
- **ERROR_STATE**: System error occurred

## Troubleshooting

- **WiFi connection issues**: Check credentials in `config.h`
- **RFID not reading**: Verify wiring and card proximity
- **LCD not displaying**: Check I2C connections and address
- **Keypad not responding**: Verify row/column pin connections
- **Firebase errors**: Check database URL and authentication key

## Security Features

- Credentials stored in separate configuration file
- HTTPS communication with Firebase
- Automatic user registration for new cards
- Transaction logging for audit trails
- Offline mode for basic functionality

## Future Enhancements

- **Admin Interface**: Web dashboard for user management
- **Receipt Printing**: Thermal printer integration
- **Mobile App**: Smartphone balance checking
- **NFC Support**: Smartphone NFC payments
- **Analytics**: Usage statistics and reporting
- **Multi-language**: Support for different languages

## Contributing

Feel free to contribute to this project by submitting pull requests or opening issues for bugs and feature requests.

## License

This project is licensed under the MIT License - see the LICENSE file for details."}}}}