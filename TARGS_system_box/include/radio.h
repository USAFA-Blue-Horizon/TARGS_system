/** communication.h
* ===========================================================
* Name: Flight Controller Communication Interface
* Section: TVC499
* Project: Flight Controller
* Purpose: LoRa radio communication and serial interface
* ===========================================================
*/

#ifndef COMMUNICATION_H
#define COMMUNICATION_H

#include <Arduino.h>
#include <RH_RF95.h>
#include "params.h"

extern double lastTelemetryTime;


/**
 * @brief Initialize LoRa communication and Serial interface
 * @param rf95 Pointer to RF95 LoRa radio object
 * @param lastTelemetryTime Pointer to last telemetry time
 * @return True if initialization successful, false otherwise
 */
bool initializeCommunication(RH_RF95* rf95);

/**
* @brief Check for commands from LoRa radio
* @param rf95 Pointer to RF95 LoRa radio object
* @return Command string received from LoRa radio
 */
String checkForCommands(RH_RF95* rf95);


/**
 * @brief Send telemetry data over LoRa
 * @param rf95 Pointer to RF95 LoRa radio object
 * @param quatAngles Array of quaternion angles [yaw,pitch,roll]
 * @param altData Array of altitude data [altitude,pressure,temp]
 * @param yawServo Pointer to yaw servo object
 * @param pitchServo Pointer to pitch servo object
 */
void sendData(RH_RF95* rf95, double pt, double continuity1, int state, String extraInfo);


#endif // COMMUNICATION_H