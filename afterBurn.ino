#include <WiFi.h>
#include <UniversalTelegramBot.h>
#include <ArduinoJson.h>
#include <WiFiClientSecure.h>

// Wi-Fi Credentials
const char* ssid = "x";
const char* password = "12345678";

// Telegram Bot Token and Group Chat ID
const char* chatID  = "-4636684324";
const char* botToken = "7670914886:AAGV8P7EZO2vWJkEJqksDFzJvP5f5YsKZmk";

// Initialize Wi-Fi and Bot
WiFiClientSecure client;
UniversalTelegramBot bot(botToken, client);

// Sensor Pins
const int trigPin = 18; // Ultrasonic sensor Trig
const int echoPin = 19; // Ultrasonic sensor Echo
const int pirPin = 2;  // PIR motion sensor
const int irPin = 13;   // IR sensor
const int gasPin = 22;  // Gas sensor (Analog pin)
const int buzzerPin = 23;
const int vibrate = 14;

// Thresholds
const int gasThreshold = 400;  // Adjust based on your sensor's readings
const int vibra=500;

// Variables
long duration;
int distance;
int gasValue;

void setup() {
  Serial.begin(115200);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(pirPin, INPUT);
  pinMode(irPin, INPUT);
  pinMode(buzzerPin, OUTPUT);

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected to Wi-Fi!");
  client.setInsecure();
  bot.sendMessage(chatID, "ESP8266 Security System is Online!", "");
}

void loop() {
  // Ultrasonic Sensor
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  distance = duration * 0.034 / 2;

  // Send distance value to Telegram
  String distanceMessage = "🚨 Ultrasonic Alert: Object detected at " + String(distance) + " cm!";
  bot.sendMessage(chatID, distanceMessage.c_str(), "");
  

  if (distance < 25) { 
    activateBuzzer();
    delay(2000); // Avoid spamming
  }

  // PIR Motion Sensor
  if (digitalRead(pirPin) == HIGH) {
    bot.sendMessage(chatID, "🚨 Motion Detected!", "");
    activateBuzzer();
    delay(2000);
  }

  // IR Sensor
  if (digitalRead(irPin) == HIGH) {
    bot.sendMessage(chatID, "🚨 IR Sensor Alert: Object detected!", "");
    activateBuzzer();
    delay(2000);
  }

  // Gas Sensor
  gasValue = digitalRead(gasPin);
  if (gasValue > gasThreshold) {
    bot.sendMessage(chatID, "🚨 Gas Leak Detected! Be cautious!", "");
    activateBuzzer();
    delay(2000);
  }

  delay(100); // Loop delay
}

void activateBuzzer() {
  digitalWrite(buzzerPin, HIGH);
  delay(500);
  digitalWrite(buzzerPin, LOW);
}
