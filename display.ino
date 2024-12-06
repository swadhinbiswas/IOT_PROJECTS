#include <SPI.h>                  // Include SPI library for RFID communication
#include <MFRC522.h>              // Include MFRC522 library for RFID
#include <Wire.h>                 // Include Wire library for I2C communication
#include <LiquidCrystal_I2C.h>    // Include LiquidCrystal_I2C library for LCD

// RFID pins
#define SS_PIN 5                 // Define Slave Select pin for RFID
#define RST_PIN 2                // Define Reset pin for RFID

MFRC522 rfid(SS_PIN, RST_PIN);    // Create MFRC522 instance with defined SS and RST pins

// I2C LCD address (typically 0x27 or 0x3F)
LiquidCrystal_I2C lcd(0x27, 16, 2); // Create LCD instance with I2C address 0x27, 16 columns, and 2 rows

// Predefined UID for authorized card
byte authorizedUID[] = {0x0B, 0x23, 0x9B, 0x15}; // Example UID (change this to your card's UID)

void setup() {
  Serial.begin(9600);             // Start serial communication at 9600 baud rate

  // Initialize RFID
  SPI.begin();                    // Initiate SPI bus
  rfid.PCD_Init();                // Initiate MFRC522 RFID module

  // Initialize LCD
  lcd.init();                     // Initialize the LCD
  lcd.backlight();                // Turn on the LCD backlight
  lcd.clear();                    // Clear the LCD screen
  lcd.setCursor(0, 0);            // Set cursor to the first column of the first row
  lcd.print("Your Card please");  // Display initial message on the first row of the LCD
}

void loop() {
  // Look for new cards
  if (!rfid.PICC_IsNewCardPresent() || !rfid.PICC_ReadCardSerial()) {
    return;                       // If no new card is present, exit the loop
  }

  // Display UID on Serial Monitor
  Serial.print("UID tag: ");
  String content = "";            // Create a string to hold the UID content
  for (byte i = 0; i < rfid.uid.size; i++) {
    Serial.print(rfid.uid.uidByte[i] < 0x10 ? " 0" : " ");
    Serial.print(rfid.uid.uidByte[i], HEX); // Print each byte of the UID in HEX format
    content.concat(String(rfid.uid.uidByte[i] < 0x10 ? " 0" : " "));
    content.concat(String(rfid.uid.uidByte[i], HEX)); // Append each byte to the content string
  }
  Serial.println();
  Serial.println();

  // Check if the scanned UID matches the authorized UID
  bool authorized = true;
  for (byte i = 0; i < rfid.uid.size; i++) {
    if (rfid.uid.uidByte[i] != authorizedUID[i]) {
      authorized = false;
      break;
    }
  }

  // Display appropriate message on the second row of the LCD
  lcd.setCursor(0, 1);            // Set cursor to the first column of the second row
  if (authorized) {
    lcd.print("Welcome Home     ");    // Display welcome message for authorized UID, with spaces to overwrite any previous text
  } else {
    lcd.print("Wrong identity..."); // Display error message for unauthorized UID
  }

  delay(2000);                    // Hold the display for 2 seconds
  lcd.setCursor(0, 1);            // Set cursor to the first column of the second row
  lcd.print("                ");  // Clear the second row by printing spaces

  // Halt PICC
  rfid.PICC_HaltA();              // Halt the PICC (card)
}
