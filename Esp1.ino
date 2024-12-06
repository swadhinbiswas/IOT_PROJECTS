/*
 *  ESP2[Hendling The sensors]
 * Project: A shop Security System and (IoT Sensor Monitoring) with Firebase
 * Author: SwadhinBiswas
 * Date: 2024-12-05
 * Contact: swadhinbiswas.cse@gmail.com
 * GitHub: https://github.com/swadhinbiswas
 * 
 * Description: 
 *     This program reads sensor data from gas, motion, vibration, and ultrasonic sensors,
 *     and uploads the data to Firebase Realtime Database. Additionally, it implements
 *     security alerts if certain thresholds are breached.
 * 

 */


#include <WiFi.h>
#include <ESP8266HTTPClient.h>

// Wi-Fi credentials
const char* ssid = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";

// Telegram Bot details
const String BOT_TOKEN = "your_bot_token";
const String CHAT_ID = "your_chat_id";

// Sensor pins
#define TRIG_PIN D2
#define ECHO_PIN D1
#define PIR_PIN D3
#define IR_PIN D4
#define GAS_PIN A0

void setup() {
  Serial.begin(115200);
  
  // Setup sensors
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(PIR_PIN, INPUT);
  pinMode(IR_PIN, INPUT);

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to Wi-Fi...");
  }
  Serial.println("Connected to Wi-Fi");
}

void loop() {
  // Read sensor data
  float distance = getUltrasonicDistance();
  int motionDetected = digitalRead(PIR_PIN);
  int obstacleDetected = digitalRead(IR_PIN);
  int gasLevel = analogRead(GAS_PIN);

  // Log data to Serial Monitor
  Serial.printf("Distance: %.2f cm, Motion: %d, Obstacle: %d, Gas Level: %d\n",
                distance, motionDetected, obstacleDetected, gasLevel);

  // Generate a message for Telegram
  String message = "Sensor Update:\n";
  message += "📏 Distance: " + String(distance) + " cm\n";
  message += "👀 Motion: " + String(motionDetected ? "Detected" : "None") + "\n";
  message += "🚧 Obstacle: " + String(obstacleDetected ? "Detected" : "None") + "\n";
  message += "🌫️ Gas Level: " + String(gasLevel) + "\n";

  // Send data to Telegram
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    String url = "https://api.telegram.org/bot" + BOT_TOKEN + 
                 "/sendMessage?chat_id=" + CHAT_ID + 
                 "&text=" + message;

    http.begin(url);
    int httpResponseCode = http.GET();

    if (httpResponseCode > 0) {
      Serial.printf("Telegram Message Sent! Response: %d\n", httpResponseCode);
    } else {
      Serial.printf("Error Sending Message: %s\n", http.errorToString(httpResponseCode).c_str());
    }

    http.end();
  } else {
    Serial.println("Wi-Fi Disconnected!");
  }

  delay(5000); // Wait before sending the next update
}

float getUltrasonicDistance() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  float duration = pulseIn(ECHO_PIN, HIGH);
  return (duration * 0.034) / 2; // Convert to cm
}
