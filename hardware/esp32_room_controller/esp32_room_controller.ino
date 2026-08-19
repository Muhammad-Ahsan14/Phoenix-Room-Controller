#include <WiFi.h>
#include <WebServer.h>

#include "secrets.h"
#include "device_config.h"


// ======================================================
// PHOENIX ROOM CONTROLLER V3
// ======================================================

WebServer server(80);


// ======================================================
// RUNTIME STATE
// ======================================================

bool commandedLightState = false;
bool actualLightState = false;


// ======================================================
// WEB UI
// ======================================================

const char INDEX_HTML[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="en">

<head>
  <meta charset="UTF-8">

  <meta
    name="viewport"
    content="width=device-width, initial-scale=1.0"
  >

  <title>PHOENIX Room Controller V3</title>

  <style>

    * {
      box-sizing: border-box;
      margin: 0;
      padding: 0;
    }


    body {

      min-height: 100vh;

      display: flex;

      align-items: center;
      justify-content: center;

      padding: 20px;

      font-family:
        Arial,
        Helvetica,
        sans-serif;

      color: white;

      background:
        radial-gradient(
          circle at top,
          #172554,
          #020617 55%
        );
    }


    .container {

      width: 100%;
      max-width: 520px;

      padding: 30px 24px;

      text-align: center;

      background:
        rgba(
          15,
          23,
          42,
          0.95
        );

      border:
        1px solid
        rgba(
          148,
          163,
          184,
          0.2
        );

      border-radius: 24px;

      box-shadow:
        0 25px 60px
        rgba(
          0,
          0,
          0,
          0.45
        );
    }


    .logo {

      font-size: 34px;

      font-weight: 800;

      letter-spacing: 2px;
    }


    .subtitle {

      margin-top: 6px;
      margin-bottom: 26px;

      color: #94a3b8;

      font-size: 14px;
    }


    .status-grid {

      display: grid;

      grid-template-columns:
        repeat(
          2,
          1fr
        );

      gap: 12px;

      margin-bottom: 18px;
    }


    .card {

      padding: 20px 10px;

      background: #0f172a;

      border:
        1px solid
        #1e293b;

      border-radius: 16px;
    }


    .label {

      margin-bottom: 9px;

      color: #94a3b8;

      font-size: 12px;

      letter-spacing: 1px;
    }


    .value {

      font-size: 24px;

      font-weight: 800;
    }


    .on {
      color: #22c55e;
    }


    .off {
      color: #ef4444;
    }


    .verified {
      color: #22c55e;
    }


    .fault {
      color: #f59e0b;
    }


    #systemStatus {

      margin-top: 6px;

      font-size: 22px;

      font-weight: 800;
    }


    #verificationDetail {

      margin-top: 7px;
      margin-bottom: 22px;

      color: #94a3b8;

      font-size: 13px;

      letter-spacing: 0.6px;
    }


    .buttons {

      display: grid;

      gap: 12px;
    }


    button {

      width: 100%;

      padding: 16px;

      border: none;

      border-radius: 14px;

      color: white;

      font-size: 16px;

      font-weight: 700;

      cursor: pointer;

      transition: 0.2s;
    }


    button:active {

      transform:
        scale(0.97);
    }


    .on-button {

      background: #16a34a;
    }


    .off-button {

      background: #dc2626;
    }


    .status-button {

      background: #2563eb;
    }


    #sensor {

      margin-top: 20px;

      color: #cbd5e1;

      font-size: 14px;
    }


    #message {

      min-height: 18px;

      margin-top: 10px;

      color: #94a3b8;

      font-size: 13px;
    }


    .footer {

      margin-top: 25px;

      padding-top: 18px;

      border-top:
        1px solid
        #1e293b;

      color: #64748b;

      font-size: 12px;
    }


    @media (
      max-width: 480px
    ) {

      .container {

        padding:
          25px 17px;
      }


      .logo {

        font-size: 28px;
      }


      .status-grid {

        grid-template-columns:
          1fr;
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
      Room Controller V3 • Verified Automation
    </div>


    <div class="status-grid">


      <div class="card">

        <div class="label">
          COMMANDED STATE
        </div>

        <div
          id="commanded"
          class="value"
        >
          --
        </div>

      </div>


      <div class="card">

        <div class="label">
          ACTUAL STATE
        </div>

        <div
          id="actual"
          class="value"
        >
          --
        </div>

      </div>


    </div>


    <div id="systemStatus">
      CHECKING...
    </div>


    <div id="verificationDetail">
      --
    </div>


    <div class="buttons">


      <button
        class="on-button"
        onclick="sendCommand('/light/on')"
      >
        LIGHT ON
      </button>


      <button
        class="off-button"
        onclick="sendCommand('/light/off')"
      >
        LIGHT OFF
      </button>


      <button
        class="status-button"
        onclick="getStatus()"
      >
        GET STATUS
      </button>


    </div>


    <div id="sensor">
      LDR Value: --
    </div>


    <div id="message">
      Connecting to PHOENIX...
    </div>


    <div class="footer">
      PHOENIX AI • Verified Room Automation
    </div>


  </div>


  <script>

    const commanded =
      document.getElementById(
        "commanded"
      );


    const actual =
      document.getElementById(
        "actual"
      );


    const systemStatus =
      document.getElementById(
        "systemStatus"
      );


    const verificationDetail =
      document.getElementById(
        "verificationDetail"
      );


    const sensor =
      document.getElementById(
        "sensor"
      );


    const message =
      document.getElementById(
        "message"
      );


    function setStateStyle(
      element,
      state
    ) {

      element.textContent =
        state;


      element.className =
        "value " +
        (
          state === "ON"
            ? "on"
            : "off"
        );
    }


    function updateDisplay(
      data
    ) {

      setStateStyle(
        commanded,
        data.commanded
      );


      setStateStyle(
        actual,
        data.actual
      );


      systemStatus.textContent =
        data.system_status;


      systemStatus.className =
        (
          data.system_status ===
          "VERIFIED"
        )
          ? "verified"
          : "fault";


      verificationDetail.textContent =
        data.detail.replaceAll(
          "_",
          " "
        );


      sensor.textContent =
        "LDR Value: " +
        data.ldr_value;
    }


    async function requestStatus(
      endpoint
    ) {

      try {

        const response =
          await fetch(
            endpoint
          );


        const data =
          await response.json();


        if (
          !response.ok ||
          !data.success
        ) {

          throw new Error(
            data.error ||
            "Request failed"
          );
        }


        updateDisplay(
          data
        );


        return true;
      }


      catch (
        error
      ) {

        systemStatus.textContent =
          "CONNECTION ERROR";


        systemStatus.className =
          "fault";


        verificationDetail.textContent =
          "DEVICE UNREACHABLE";


        message.textContent =
          "Could not communicate with PHOENIX device.";


        return false;
      }
    }


    async function sendCommand(
      endpoint
    ) {

      message.textContent =
        "Sending command...";


      const success =
        await requestStatus(
          endpoint
        );


      if (
        success
      ) {

        message.textContent =
          "Command completed.";
      }
    }


    async function getStatus() {

      const success =
        await requestStatus(
          "/light/status"
        );


      if (
        success
      ) {

        message.textContent =
          "Live monitoring active.";
      }
    }


    window.onload =
      getStatus;


    setInterval(
      getStatus,
      500
    );

  </script>

</body>

</html>
)rawliteral";


// ======================================================
// LDR SENSOR
// ======================================================

int readLDR() {

  long total = 0;


  for (
    int i = 0;
    i < SENSOR_SAMPLES;
    i++
  ) {

    total +=
      analogRead(
        LDR_PIN
      );


    delay(
      SENSOR_SAMPLE_DELAY_MS
    );
  }


  return
    total /
    SENSOR_SAMPLES;
}


// ======================================================
// ACTUAL LIGHT STATE
// ======================================================

void updateActualLightState(
  int ldrValue
) {

  // Light clearly detected.
  if (
    ldrValue >=
    LIGHT_ON_THRESHOLD
  ) {

    actualLightState =
      true;
  }


  // Light clearly not detected.
  else if (
    ldrValue <=
    LIGHT_OFF_THRESHOLD
  ) {

    actualLightState =
      false;
  }


  // Between thresholds:
  // preserve previous actual state.
  // This creates hysteresis and prevents flickering.
}


// ======================================================
// VERIFICATION ENGINE
// ======================================================

String getSystemStatus() {

  if (
    commandedLightState ==
    actualLightState
  ) {

    return
      "VERIFIED";
  }


  return
    "FAULT";
}


String getVerificationDetail() {

  if (
    commandedLightState ==
    actualLightState
  ) {

    return
      "STATE_MATCH";
  }


  if (
    commandedLightState &&
    !actualLightState
  ) {

    return
      "LIGHT_NOT_DETECTED";
  }


  return
    "UNEXPECTED_LIGHT_DETECTED";
}


// ======================================================
// LIGHT / RELAY CONTROL
// ======================================================

void setLight(
  bool state
) {

  commandedLightState =
    state;


  // Current hardware architecture:
  //
  // ESP32 HIGH
  // -> BC547 ON
  // -> relay input pulled LOW
  // -> relay activated

  digitalWrite(
    RELAY_PIN,
    state
      ? HIGH
      : LOW
  );


  // Allow relay and load
  // to physically settle.

  delay(
    RELAY_SETTLE_DELAY_MS
  );


  int ldrValue =
    readLDR();


  updateActualLightState(
    ldrValue
  );
}


// ======================================================
// JSON STATUS
// ======================================================

String createStatusResponse() {

  int ldrValue =
    readLDR();


  updateActualLightState(
    ldrValue
  );


  String commanded =
    commandedLightState
      ? "ON"
      : "OFF";


  String actual =
    actualLightState
      ? "ON"
      : "OFF";


  String systemStatus =
    getSystemStatus();


  String detail =
    getVerificationDetail();


  String response =
    "{";


  response +=
    "\"success\":true,";


  response +=
    "\"commanded\":\"";

  response +=
    commanded;

  response +=
    "\",";


  response +=
    "\"actual\":\"";

  response +=
    actual;

  response +=
    "\",";


  response +=
    "\"system_status\":\"";

  response +=
    systemStatus;

  response +=
    "\",";


  response +=
    "\"detail\":\"";

  response +=
    detail;

  response +=
    "\",";


  response +=
    "\"ldr_value\":";

  response +=
    String(
      ldrValue
    );


  response +=
    "}";


  return
    response;
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
    "\"device\":\"PHOENIX Room Controller V3\","
    "\"status\":\"online\"}"
  );
}


void handleLightOn() {

  setLight(
    true
  );


  server.send(
    200,
    "application/json",
    createStatusResponse()
  );
}


void handleLightOff() {

  setLight(
    false
  );


  server.send(
    200,
    "application/json",
    createStatusResponse()
  );
}


void handleLightStatus() {

  server.send(
    200,
    "application/json",
    createStatusResponse()
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
// WI-FI CONNECTION
// ======================================================

void connectToWiFi() {

  Serial.print(
    "Connecting to Wi-Fi"
  );


  WiFi.mode(
    WIFI_STA
  );


  WiFi.begin(
    WIFI_SSID,
    WIFI_PASSWORD
  );


  while (
    WiFi.status() !=
    WL_CONNECTED
  ) {

    delay(
      500
    );


    Serial.print(
      "."
    );
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
// HTTP ROUTES
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

  Serial.begin(
    115200
  );


  // ----------------------------------------------------
  // Relay output
  // ----------------------------------------------------

  pinMode(
    RELAY_PIN,
    OUTPUT
  );


  // ----------------------------------------------------
  // ADC configuration
  // ----------------------------------------------------

  analogReadResolution(
    12
  );


  analogSetPinAttenuation(
    LDR_PIN,
    ADC_11db
  );


  // ----------------------------------------------------
  // Safe startup
  // ----------------------------------------------------

  digitalWrite(
    RELAY_PIN,
    LOW
  );


  commandedLightState =
    false;


  delay(
    500
  );


  // ----------------------------------------------------
  // Initial sensor state
  // ----------------------------------------------------

  int initialReading =
    readLDR();


  updateActualLightState(
    initialReading
  );


  // ----------------------------------------------------
  // Serial startup information
  // ----------------------------------------------------

  Serial.println();


  Serial.println(
    "================================"
  );


  Serial.println(
    " PHOENIX ROOM CONTROLLER V3"
  );


  Serial.println(
    "================================"
  );


  Serial.println();


  Serial.print(
    "Initial LDR Value: "
  );


  Serial.println(
    initialReading
  );


  // ----------------------------------------------------
  // Wi-Fi
  // ----------------------------------------------------

  connectToWiFi();


  // ----------------------------------------------------
  // HTTP server
  // ----------------------------------------------------

  setupRoutes();


  server.begin();


  Serial.println();


  Serial.println(
    "HTTP server started."
  );


  Serial.println(
    "Feedback engine ready."
  );


  Serial.println(
    "Verification engine ready."
  );


  Serial.println(
    "Web controller ready."
  );
}


// ======================================================
// MAIN LOOP
// ======================================================

void loop() {

  server.handleClient();
}