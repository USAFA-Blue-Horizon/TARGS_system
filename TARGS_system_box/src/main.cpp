#include <Arduino.h>
#include <radio.h>
#include <params.h>
#include <hardware.h>
#include <PWMServo.h> // Include the PWMServo library for controlling servos

PWMServo Fuel_Servo;  // create servo object to control a servo
PWMServo Ox_Servo;  // create servo object to control a servo
PWMServo Ox_load; // create servo object to control a servo
PWMServo CO2_Servo;  // create servo object to control a servo

// Declare systemState as a global variable
SystemState systemState = UNARMED;

RH_RF95* radio = new RH_RF95(RFM95_CS, RFM95_INT);

double continuity = 0.0; //hold 1 continuity value for pyro 1

int count = 0; // loop counter for telemetry

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  Fuel_Servo.attach(8);  // attaches the servo on pin 9 to the servo object
  Ox_Servo.attach(9);  // attaches the servo on pin 10 to the servo object
  Ox_load.attach(7); // attaches the servo on pin 11 to the servo object
  CO2_Servo.attach(6);  // attaches the servo on pin 12 to the servo object

  Fuel_Servo.write(0); // set servo to 0 degrees
  Ox_Servo.write(0); // set servo to 0 degrees
  Ox_load.write(0); // set servo to 0 degrees
  CO2_Servo.write(0); // set servo to 0 degrees

  Serial.println("System initialized, waiting for commands...");

  // systemState is already initialized globally
  if (!initializeCommunication(radio)) {
    Serial.println("LoRa radio init failed!");
    while (1);
  }
  Serial.println("LoRa radio init OK!");
  

}

void loop() {
  //run state machine here, checking for commands from LoRa radio
  //String command = checkForCommands(radio);
  
  // Only process serial commands if data is available
    String command = checkForCommands(radio);
    //check if command has content before printing
    if (command.length() > 0) {
      Serial.print("Command received: ");
      Serial.println(command);
    }
    checkPyroContinuity(&continuity); 
    if(count % 15 == 0) { //send continuity update every 10 loops
    sendData(radio, 0.0, continuity, systemState, "Continuity update");
    }

    if (command.length() > 0) { // Only process non-empty commands
      Serial.print("Command received: ");
      Serial.println(command);
      if(systemState == UNARMED && command != "ARM") {
        sendData(radio, 0.0, continuity, systemState, "System not armed");
      }
      else if (command == "ARM") {
        systemState = ARMED;
        Serial.println("System armed");
        sendData(radio, 0.0, continuity, systemState, "System armed");
      }
      else if (command == "DISARM") {
        systemState = UNARMED;
        Serial.println("System disarmed");
        sendData(radio, 0.0, continuity, systemState, "System disarmed");
      }
      else if (systemState == ARMED && command == "LOAD") {
        systemState = LOADING;
        Serial.println("Loading servos...");
        Ox_load.write(90); // set servo to 90 degrees
        sendData(radio, 0.0, continuity, systemState, "Loading servos");
      }
      else if (systemState == LOADING && command == "LOADED") {
        systemState = LOADED;
        Serial.println("Servos loaded");
        Ox_load.write(0); // set servo to 0 degrees
        sendData(radio, 0.0, continuity, systemState, "Servos loaded");
      }
      else if (systemState == LOADED && command == "FIRE") {
        systemState = FIRE;
        Serial.println("Firing sequence initiated...");
        triggerFire();
        Fuel_Servo.write(0); // set servo to 0 degrees
        Ox_Servo.write(0); // set servo to 0 degrees
        delay(2000); // wait for 2 seconds
        Serial.println("Servos fired");
        sendData(radio, 0.0, 0.0, systemState, "Servos & pyro fired");
      }
      else if (systemState == FIRE && command == "PURGE") {
        Serial.println("CO2 Purge sequence initiated...");
        CO2_Servo.write(90); // set servo to 90 degrees
        delay(2000); // wait for 2 seconds
        CO2_Servo.write(0); // set servo to 0 degrees
        delay(2000); // wait for 2 seconds
        Serial.println("CO2 Purge complete");
        systemState = UNARMED; // reset system state to UNARMED after CO2 purge
      }

      else {
        Serial.println("Unknown command");
      }
    }
    count++;
      delay(100); // Small delay to prevent excessive polling

  }




