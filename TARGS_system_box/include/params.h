#ifndef CONFIG_H
#define CONFIG_H

// Other Constants
#define TELEMETRY_INTERVAL 200  // Send serial data every 200ms

// Pyro Pins
#define LAUNCH_PYRO_FIRE 28
#define SEP_PYRO_FIRE 29
#define LAUNCH_PYRO_CONT A11  // continuity pyro 1
#define SEP_PYRO_CONT A12  // continuity pyro 2
#define PYRO_DURATION 2000  // 2 seconds
#define CONTINUITY_THRESHOLD 2 // Threshold for continuity detection (in volts)

//PT
#define PT_SENSOR A13

// LED Configuration
#define NUM_LEDS 2
#define LED_DATA_PIN 3

// Buzzer Pins
#define BUZZER_HIGH 4
#define BUZZER_LOW 5

// LoRa Pins
#define RFM95_CS 10
#define RFM95_RST 23
#define RFM95_INT 1
#define RF95_FREQ 915.0

// State Definitions make systemState enum
enum SystemState {
    UNARMED,
    ARMED,
    LOADING,
    LOADED,
    FIRE,
    PURGE,
    PURGED
};

#define CO2PURGE 5

#endif // CONFIG_H