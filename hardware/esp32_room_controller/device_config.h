#pragma once

// ======================================================
// PHOENIX ROOM CONTROLLER V3
// DEVICE CONFIGURATION
// ======================================================

// Hardware pins
const int RELAY_PIN = 4;
const int LDR_PIN = 5;

// LDR calibration
// Measured during testing:
// Light OFF: approximately 0-10
// Controlled LED ON: approximately 1600-1700
// Bright flashlight: approximately 2500-3000

const int LIGHT_ON_THRESHOLD = 1000;
const int LIGHT_OFF_THRESHOLD = 500;

// Sensor averaging
const int SENSOR_SAMPLES = 10;
const int SENSOR_SAMPLE_DELAY_MS = 5;

// Physical relay/load settling time
const int RELAY_SETTLE_DELAY_MS = 250;