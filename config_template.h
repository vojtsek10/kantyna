/*
 * Configuration Template for Cafeteria Management System
 * 
 * Copy this file to 'config.h' and fill in your actual credentials
 * DO NOT commit the config.h file to version control for security!
 */

#ifndef CONFIG_H
#define CONFIG_H

// WiFi Configuration
const char* ssid = "wifi";           // Replace with your WiFi network name
const char* password = "Meow1999";   // Replace with your WiFi password

// Firebase Configuration
const char* FIREBASE_HOST = "test1-c29c0-default-rtdb.europe-west1.firebasedatabase.app";  // Replace with your Firebase Realtime Database URL
const char* FIREBASE_AUTH = "vGopof8yDOMTeggToP3CEnz37JtB0O051AcTHd0W";                      // Replace with your Firebase Database Secret

/*
 * How to get Firebase credentials:
 * 
 * 1. Go to Firebase Console (https://console.firebase.google.com/)
 * 2. Create a new project or select existing one
 * 3. Go to "Realtime Database" in the left sidebar
 * 4. Create a database and choose your preferred location
 * 5. For FIREBASE_HOST: Copy the database URL (without https:// and without the trailing slash)
 * 6. For FIREBASE_AUTH: 
 *    - Go to Project Settings (gear icon)
 *    - Navigate to "Service Accounts" tab
 *    - Click "Database secrets" 
 *    - Copy the secret key
 * 
 * Database Structure:
 * The system will automatically create this structure:
 * {
 *   "users": {
 *     "CARD_UID": {
 *       "balance": 0.0,
 *       "name": "User XXXX",
 *       "created": timestamp
 *     }
 *   },
 *   "transactions": {
 *     "transaction_id": {
 *       "cardUID": "CARD_UID",
 *       "amount": 10.0,
 *       "type": "add" or "deduct",
 *       "timestamp": timestamp,
 *       "balance_after": 25.0
 *     }
 *   }
 * }
 */

#endif