#include <WiFi.h>
#include <WebServer.h>
#include "secrets.h"

WebServer server(80);

const int RELAY_PIN = 4;

bool lightState = false;


// ======================================================
// WEB PAGE
// ======================================================

const char INDEX_HTML[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="en">

<head>
  <meta charset="UTF-8">

  <meta name="viewport"
        content="width=device-width, initial-scale=1.0">

  <title>PHOENIX Room Controller</title>

  <style>
    * {
      box-sizing: border-box;
      margin: 0;
      padding: 0;
    }

    body {
      font-family: Arial, Helvetica, sans-serif;
      background:
        radial-gradient(circle at top, #172554, #020617 55%);
      min-height: 100vh;
      display: flex;
      align-items: center;
      justify-content: center;
      padding: 20px;
      color: white;
    }

    .container {
      width: 100%;
      max-width: 480px;
      background: rgba(15, 23, 42, 0.92);
      border: 1px solid rgba(148, 163, 184, 0.2);
      border-radius: 24px;
      padding: 32px 24px;
      box-shadow: 0 25px 60px rgba(0, 0, 0, 0.45);
      text-align: center;
    }

    .logo {
      font-size: 34px;
      font-weight: 800;
      letter-spacing: 2px;
      margin-bottom: 6px;
    }

    .subtitle {
      color: #94a3b8;
      font-size: 14px;
      margin-bottom: 28px;
    }

    .device-card {
      background: #0f172a;
      border: 1px solid #1e293b;
      border-radius: 18px;
      padding: 22px;
      margin-bottom: 25px;
    }

    .device-name {
      color: #94a3b8;
      font-size: 13px;
      text-transform: uppercase;
      letter-spacing: 1.5px;
      margin-bottom: 10px;
    }

    #status {
      font-size: 30px;
      font-weight: 800;
    }

    .status-on {
      color: #22c55e;
    }

    .status-off {
      color: #ef4444;
    }

    .status-loading {
      color: #f59e0b;
    }

    .buttons {
      display: grid;
      gap: 14px;
    }

    button {
      width: 100%;
      border: none;
      border-radius: 14px;
      padding: 17px;
      font-size: 17px;
      font-weight: 700;
      cursor: pointer;
      transition: 0.2s;
      color: white;
    }

    button:active {
      transform: scale(0.97);
    }

    .on-button {
      background: #16a34a;
    }

    .on-button:hover {
      background: #15803d;
    }

    .off-button {
      background: #dc2626;
    }

    .off-button:hover {
      background: #b91c1c;
    }

    .status-button {
      background: #2563eb;
    }

    .status-button:hover {
      background: #1d4ed8;
    }

    #message {
      min-height: 20px;
      color: #94a3b8;
      font-size: 14px;
      margin-top: 20px;
    }

    .footer {
      margin-top: 28px;
      padding-top: 18px;
      border-top: 1px solid #1e293b;
      color: #64748b;
      font-size: 12px;
    }

    @media (max-width: 480px) {
      .container {
        padding: 27px 18px;
        border-radius: 20px;
      }

      .logo {
        font-size: 28px;
      }

      button {
        padding: 16px;
      }
    }
  </style>
</head>

<body>

  <div class="container">

    <div class="logo">
      PHOENIX
    </div>

    <div class="subtitle">
      Room Controller V2
    </div>

    <div class="device-card">

      <div class="device-name">
        Room Light
      </div>

      <div id="status" class="status-loading">
        CHECKING...
      </div>

    </div>

    <div class="buttons">

      <button class="on-button" onclick="lightOn()">
        LIGHT ON
      </button>

      <button class="off-button" onclick="lightOff()">
        LIGHT OFF
      </button>

      <button class="status-button" onclick="getStatus()">
        GET STATUS
      </button>

    </div>

    <div id="message">
      Connecting to PHOENIX device...
    </div>

    <div class="footer">
      PHOENIX AI • Room Automation System
    </div>

  </div>


  <script>

    const statusElement =
      document.getElementById("status");

    const messageElement =
      document.getElementById("message");


    function updateDisplay(state) {

      statusElement.textContent = state;

      statusElement.classList.remove(
        "status-on",
        "status-off",
        "status-loading"
      );

      if (state === "ON") {
        statusElement.classList.add("status-on");
      }
      else {
        statusElement.classList.add("status-off");
      }
    }


    async function sendRequest(endpoint) {

      messageElement.textContent =
        "Sending command...";

      try {

        const response =
          await fetch(endpoint);

        const data =
          await response.json();

        if (!response.ok || !data.success) {
          throw new Error(
            data.error || "Request failed"
          );
        }

        updateDisplay(data.light);

        messageElement.textContent =
          "Command completed successfully.";

      }

      catch (error) {

        statusElement.textContent =
          "ERROR";

        statusElement.className =
          "status-loading";

        messageElement.textContent =
          "Could not communicate with device.";
      }
    }


    function lightOn() {
      sendRequest("/light/on");
    }


    function lightOff() {
      sendRequest("/light/off");
    }


    function getStatus() {
      sendRequest("/light/status");
    }


    // Automatically retrieve current status
    // when page loads.
    window.onload = getStatus;

  </script>

</body>

</html>
)rawliteral";


// ======================================================
// HARDWARE CONTROL
// ======================================================

void setLight(bool state) {

  lightState = state;

  if (lightState) {
    digitalWrite(RELAY_PIN, HIGH);
  }
  else {
    digitalWrite(RELAY_PIN, LOW);
  }
}


// ======================================================
// HTTP HANDLERS
// ======================================================

void handleHome() {

  server.send_P(
    200,
    "text/html",
    INDEX_HTML
  );
}


void handleHealth() {

  server.send(
    200,
    "application/json",
    "{\"success\":true,"
    "\"device\":\"PHOENIX Room Controller V2\","
    "\"status\":\"online\"}"
  );
}


void handleLightOn() {

  setLight(true);

  server.send(
    200,
    "application/json",
    "{\"success\":true,"
    "\"light\":\"ON\","
    "\"relay\":\"ON\"}"
  );
}


void handleLightOff() {

  setLight(false);

  server.send(
    200,
    "application/json",
    "{\"success\":true,"
    "\"light\":\"OFF\","
    "\"relay\":\"OFF\"}"
  );
}


void handleLightStatus() {

  String state =
    lightState ? "ON" : "OFF";

  String response =
    "{\"success\":true,"
    "\"light\":\"" + state +
    "\",\"relay\":\"" + state + "\"}";

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
    "{\"success\":false,"
    "\"error\":\"Endpoint not found\"}"
  );
}


// ======================================================
// WI-FI
// ======================================================

void connectToWiFi() {

  Serial.print(
    "Connecting to Wi-Fi"
  );

  WiFi.mode(WIFI_STA);

  WiFi.begin(
    WIFI_SSID,
    WIFI_PASSWORD
  );

  while (
    WiFi.status() != WL_CONNECTED
  ) {

    delay(500);

    Serial.print(".");
  }

  Serial.println();

  Serial.println(
    "Wi-Fi connected."
  );

  Serial.print(
    "IP Address: "
  );

  Serial.println(
    WiFi.localIP()
  );
}


// ======================================================
// ROUTES
// ======================================================

void setupRoutes() {

  server.on(
    "/",
    HTTP_GET,
    handleHome
  );

  server.on(
    "/health",
    HTTP_GET,
    handleHealth
  );

  server.on(
    "/light/on",
    HTTP_GET,
    handleLightOn
  );

  server.on(
    "/light/off",
    HTTP_GET,
    handleLightOff
  );

  server.on(
    "/light/status",
    HTTP_GET,
    handleLightStatus
  );

  server.onNotFound(
    handleNotFound
  );
}


// ======================================================
// SETUP
// ======================================================

void setup() {

  Serial.begin(115200);

  pinMode(
    RELAY_PIN,
    OUTPUT
  );

  // Safe startup:
  // relay and light OFF.
  digitalWrite(
    RELAY_PIN,
    LOW
  );

  lightState = false;

  delay(1000);

  Serial.println();

  Serial.println(
    "================================"
  );

  Serial.println(
    "  PHOENIX ROOM CONTROLLER V2"
  );

  Serial.println(
    "================================"
  );

  Serial.println();

  connectToWiFi();

  setupRoutes();

  server.begin();

  Serial.println();

  Serial.println(
    "HTTP server started."
  );

  Serial.println(
    "Web controller ready."
  );

  Serial.println(
    "Relay controller ready."
  );
}


// ======================================================
// LOOP
// ======================================================

void loop() {

  server.handleClient();
}