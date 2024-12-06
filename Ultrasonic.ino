

// const int trigPin = 2;
// const int echoPin = 4;
// const int buzzerPin = 5; // Replace 5 with the correct buzzer pin number
// const int ledPin = 2; // Replace 2 with the correct LED pin number

#include <WiFi.h>

// WiFi credentials
const char *ssid = "11111111";
const char *password = "11111111";

// Ultrasonic sensor pins
const int trigPin = 2;
const int echoPin = 4;

// Buzzer pin
const int buzzerPin = 5;

// Variables
long duration;
float distance;
WiFiServer server(80);

void setup()
{
  // Serial monitor
  Serial.begin(115200);

  // Ultrasonic sensor pins
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  // Buzzer pin
  pinMode(buzzerPin, OUTPUT);

  // Connect to WiFi
  Serial.print("Connecting to WiFi");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("\nConnected to WiFi.");
  Serial.print("ESP32 IP Address: ");
  Serial.println(WiFi.localIP());

  // Start server
  server.begin();
}

void loop()
{
  // Measure distance
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  distance = duration * 0.034 / 2;

  // Print to Serial Monitor
  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.println(" cm");

  // Sound buzzer if object is detected within 20 cm
  if (distance <= 20.0)
  {
    digitalWrite(buzzerPin, HIGH);
  }
  else
  {
    digitalWrite(buzzerPin, LOW);
  }

  // Handle web server
  WiFiClient client = server.available();
  if (client)
  {
    Serial.println("New Client Connected!");
    String request = client.readStringUntil('\r');
    Serial.println(request);

    // HTTP Response
    client.println("HTTP/1.1 200 OK");
    client.println("Content-Type: text/html");
    client.println("Connection: close");
    client.println();
    client.println("<!DOCTYPE HTML>");
    client.println("<html>");
    client.println("<h1>ESP32 Ultrasonic Sensor</h1>");
    client.print("<p>Distance: ");
    client.print(distance);
    client.println(" cm</p>");
    if (distance <= 20.0)
    {
      client.println("<p><b>Alert! Object Detected.</b></p>");
    }
    client.println("</html>");
    delay(1);
    Serial.println("Client Disconnected.");
  }

  delay(500); // Adjust for smoother readings
}