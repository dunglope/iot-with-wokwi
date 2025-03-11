#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <uri/UriBraces.h>
#include <HTTPClient.h>

#define WIFI_SSID "Wokwi-GUEST"
#define WIFI_PASSWORD ""
#define WIFI_CHANNEL 6

#define LED1 26
#define LED2 27
#define PIR_PIN 34

const char* thingSpeakHost = "api.thingspeak.com";
String thingSpeakApiKey = "T73H1V7OL78X8H3K"; // thingSpeak API key
unsigned long thingSpeakChannelID = 2873358; // channel ID
unsigned long lastThingSpeakUpdate = 0;
const long updateInterval = 15000;

WebServer server(80);

bool led1State = false;
bool led2State = false;
bool motionOverride1 = false;
bool motionOverride2 = false;

void sendHtml() {
  String response = R"(
    <!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <title>ESP32 Web Server</title>
    <link href="https://fonts.googleapis.com/css2?family=Poppins:wght@300;400;600&display=swap" rel="stylesheet">
    <style>
        * { margin: 0; padding: 0; box-sizing: border-box; }
        body { font-family: 'Poppins', sans-serif; text-align: center; background: linear-gradient(to right, #2c3e50, #4ca1af); color: #fff; padding: 20px; }
        h1 { font-size: 2.5em; margin-bottom: 20px; font-weight: 600; }
        .container { display: flex; justify-content: center; flex-wrap: wrap; gap: 20px; margin-top: 20px; }
        .card { background: rgba(255, 255, 255, 0.15); padding: 25px; border-radius: 15px; box-shadow: 0 8px 16px rgba(0, 0, 0, 0.2); text-align: center; width: 220px; transition: transform 0.3s ease; }
        .card:hover { transform: scale(1.05); }
        h2 { font-size: 1.8em; margin-bottom: 15px; }
        .btn { display: inline-block; width: 100%; padding: 12px; font-size: 1.4em; border: none; border-radius: 8px; cursor: pointer; transition: background 0.3s, transform 0.2s; font-weight: 600; color: white; }
        .ON { background: #27ae60; }
        .OFF { background: #e74c3c; }
        .btn:hover { transform: scale(1.05); opacity: 0.9; }
        @media (max-width: 600px) { .container { flex-direction: column; align-items: center; } .card { width: 90%; } }
    </style>
</head>
<body>
    <h1>ESP32 Web Server</h1>
    <div class="container">
        <div class="card">
            <h2>LED 1 (Motion)</h2>
            <a href="/toggle/1" class="btn LED1_TEXT">LED1_TEXT</a>
        </div>
        <div class="card">
            <h2>LED 2 (Motion)</h2>
            <a href="/toggle/2" class="btn LED2_TEXT">LED2_TEXT</a>
        </div>
    </div>
</body>
</html>
  )";
  response.replace("LED1_TEXT", led1State ? "ON" : "OFF");
  response.replace("LED2_TEXT", led2State ? "ON" : "OFF");
  server.send(200, "text/html", response);
}

void sendToThingSpeak() {
  if (millis() - lastThingSpeakUpdate < updateInterval) return; // Respect 15s interval

  WiFiClient client;
  HTTPClient http;
  String url = "http://";
  url += thingSpeakHost;
  url += "/update?api_key=";
  url += thingSpeakApiKey;
  url += "&field1=";
  url += led1State ? "1" : "0";
  url += "&field2=";
  url += led2State ? "1" : "0";
  url += "&field3=";
  url += (digitalRead(PIR_PIN) == HIGH) ? "1" : "0";

  http.begin(client, url);
  int httpCode = http.GET();
  if (httpCode > 0) {
    Serial.println("Data sent to ThingSpeak: HTTP " + String(httpCode));
  } else {
    Serial.println("Failed to send to ThingSpeak: " + http.errorToString(httpCode));
  }
  http.end();
  lastThingSpeakUpdate = millis();
}

void setup(void) {
  Serial.begin(115200);
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(PIR_PIN, INPUT);
  digitalWrite(LED1, LOW);
  digitalWrite(LED2, LOW);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD, WIFI_CHANNEL);
  Serial.print("Connecting to WiFi ");
  Serial.print(WIFI_SSID);
  while (WiFi.status() != WL_CONNECTED) {
    delay(100);
    Serial.print(".");
  }
  Serial.println(" Connected!");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.println("Type 'm' to reset both LEDs to motion mode");

  server.on("/", sendHtml);
  server.on(UriBraces("/toggle/{}"), []() {
    String led = server.pathArg(0);
    Serial.print("Toggle LED #");
    Serial.println(led);

    switch (led.toInt()) {
      case 1:
        led1State = !led1State;
        motionOverride1 = true;
        digitalWrite(LED1, led1State);
        Serial.println(led1State ? "LED1 ON (Manual)" : "LED1 OFF (Manual)");
        break;
      case 2:
        led2State = !led2State;
        motionOverride2 = true;
        digitalWrite(LED2, led2State);
        Serial.println(led2State ? "LED2 ON (Manual)" : "LED2 OFF (Manual)");
        break;
    }
    sendHtml();
  });

  server.begin();
  Serial.println("HTTP server started");
}

void loop(void) {
  server.handleClient();

  // Serial command to reset to motion mode
  if (Serial.available() > 0) {
    char input = Serial.read();
    if (input == 'm') {
      motionOverride1 = false;
      motionOverride2 = false;
      Serial.println("Reset both LEDs to Motion Mode");
    }
  }

  // Motion detection for both LEDs (if not overridden)
  bool motionDetected = digitalRead(PIR_PIN) == HIGH;
  static bool lastMotion = LOW;

  if (motionDetected != lastMotion) {
    if (motionDetected) {
      if (!motionOverride1) {
        led1State = true;
        digitalWrite(LED1, HIGH);
        Serial.println("Motion Detected - LED1 ON");
      }
      if (!motionOverride2) {
        led2State = true;
        digitalWrite(LED2, HIGH);
        Serial.println("Motion Detected - LED2 ON");
      }
    } else {
      if (!motionOverride1) {
        led1State = false;
        digitalWrite(LED1, LOW);
        Serial.println("No Motion - LED1 OFF");
      }
      if (!motionOverride2) {
        led2State = false;
        digitalWrite(LED2, LOW);
        Serial.println("No Motion - LED2 OFF");
      }
    }
    lastMotion = motionDetected;
  }

  sendToThingSpeak();

  delay(2);
}