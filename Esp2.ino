#include <WiFi.h>
#include <UniversalTelegramBot.h>
#include <ArduinoJson.h>

// Wi-Fi Credentials
const char* ssid = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";

// Telegram Bot Token and Group Chat ID
const char* botToken = "YOUR_BOT_TOKEN";
const char* chatID = "YOUR_GROUP_CHAT_ID";

// Initialize Wi-Fi and Bot
WiFiClientSecure client;
UniversalTelegramBot bot(botToken, client);

// Sensor Pins
const int trigPin = D1; // Ultrasonic sensor Trig
const int echoPin = D2; // Ultrasonic sensor Echo
const int pirPin = D5;  // PIR motion sensor
const int irPin = D6;   // IR sensor
const int gasPin = A0;  // Gas sensor (Analog pin)
const int buzzerPin = D8;

// Thresholds
const int gasThreshold = 400; // Adjust based on your sensor's readings

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

  if (distance < 50) { // Object detected within 50 cm
    bot.sendMessage(chatID, "🚨 Ultrasonic Alert: Object detected within 50 cm!", "");
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
  gasValue = analogRead(gasPin);
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
