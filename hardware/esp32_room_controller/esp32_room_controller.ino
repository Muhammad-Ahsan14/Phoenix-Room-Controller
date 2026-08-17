#include <WiFi.h>
#include <WebServer.h>
#include "secrets.h"

WebServer server(80);

const int LED_PIN = 4;

bool lightState = false;


void setLight(bool state) {
  lightState = state;

  if (lightState) {
    digitalWrite(LED_PIN, HIGH);
  } else {
    digitalWrite(LED_PIN, LOW);
  }
}


void handleRoot() {
  server.send(
    200,
    "application/json",
    "{\"device\":\"PHOENIX Room Controller V1\",\"status\":\"online\"}"
  );
}


void handleLightOn() {
  setLight(true);

  server.send(
    200,
    "application/json",
    "{\"success\":true,\"light\":\"ON\"}"
  );
}


void handleLightOff() {
  setLight(false);

  server.send(
    200,
    "application/json",
    "{\"success\":true,\"light\":\"OFF\"}"
  );
}


void handleLightStatus() {
  String state;

  if (lightState) {
    state = "ON";
  } else {
    state = "OFF";
  }

  String response =
    "{\"success\":true,\"light\":\"" + state + "\"}";

  server.send(
    200,
    "application/json",
    response
  );
}


void handleNotFound() {
  server.send(
    404,
    "application/json",
    "{\"success\":false,\"error\":\"Endpoint not found\"}"
  );
}


void connectToWiFi() {
  Serial.print("Connecting to Wi-Fi");

  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.println("Wi-Fi connected.");

  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
}


void setupRoutes() {
  server.on("/", HTTP_GET, handleRoot);

  server.on("/light/on", HTTP_GET, handleLightOn);

  server.on("/light/off", HTTP_GET, handleLightOff);

  server.on("/light/status", HTTP_GET, handleLightStatus);

  server.onNotFound(handleNotFound);
}


void setup() {
  Serial.begin(115200);

  pinMode(LED_PIN, OUTPUT);
  setLight(false);

  delay(1000);

  Serial.println();
  Serial.println("================================");
  Serial.println("  PHOENIX ROOM CONTROLLER V1");
  Serial.println("================================");
  Serial.println();

  connectToWiFi();

  setupRoutes();

  server.begin();

  Serial.println();
  Serial.println("HTTP server started.");
  Serial.println("PHOENIX device is ready.");
}


void loop() {
  server.handleClient();
}