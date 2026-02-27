#include <Arduino.h>
#include <radio.h>
#include <params.h>
#include <hardware.h>
#include <PWMServo.h> 

PWMServo Fuel_Servo;  
PWMServo Ox_Servo;  
PWMServo Ox_load; 
PWMServo CO2_Servo;  
PWMServo Cam_Servo;

SystemState systemState = UNARMED;

RH_RF95* radio = new RH_RF95(RFM95_CS, RFM95_INT);

double continuity = 0.0; 
double PT = 0.0; 

unsigned long load_timer = 0; 
unsigned long servo_timer = 0; 
unsigned long CO2_timer = 0; 
unsigned long cam_timer = 0;
bool camservo_fired = false;

int count = 0; 

// const int sprkPin = 24;


String status_message = "System initialized";

void setup() {

  pinMode(PT_SENSOR, INPUT); 
  pinMode(LAUNCH_PYRO_CONT, INPUT); 
  pinMode(SEP_PYRO_CONT, INPUT);

  pinMode(LAUNCH_PYRO_FIRE, OUTPUT);
  pinMode(SEP_PYRO_FIRE, OUTPUT);
  digitalWrite(LAUNCH_PYRO_FIRE, LOW); 
  digitalWrite(SEP_PYRO_FIRE, LOW);


  // pinMode(sprkPin, OUTPUT);


  Serial.begin(9600);
  Fuel_Servo.attach(8);  // J2
  Ox_Servo.attach(9);  // J1
  Ox_load.attach(7); //J3
  CO2_Servo.attach(6);   //J4
  Cam_Servo.attach(24);  //J5

  Fuel_Servo.write(8); //5 close 70 open
  Ox_Servo.write(5); //5 close 70 open
  Ox_load.write(5);  //5 close 70 open
  CO2_Servo.write(0); //0 close 65 open
  Cam_Servo.write(70); 


  Serial.println("System initialized, waiting for commands...");

  if (!initializeCommunication(radio)) {
    Serial.println("LoRa radio init failed!");
    while (1);
  }
  Serial.println("LoRa radio init OK!");
  

}

void loop() {
    String command = checkForCommands(radio);
    if (command.length() > 0) {
      Serial.print("Command received: ");
      Serial.println(command);
    }

    //status message update
    checkPyroContinuity(&continuity); 
    checkPT(&PT);
    if(count % 15 == 0) {
    sendData(radio, PT, continuity, systemState, status_message);
    }


    //process command loop/state machine
    if (command.length() > 0) { 
      if(systemState == UNARMED && command != "ARM") {
        status_message = "System not armed";
        sendData(radio, PT, continuity, systemState, status_message);
      }
      else if (command == "ARM") {
        systemState = ARMED;
        status_message = "System armed";
        sendData(radio, PT, continuity, systemState, status_message);
      }
      //no reliance on a previous state to disarm
      else if (command == "DISARM") {
        systemState = UNARMED;
        status_message = "System disarmed";
        sendData(radio, PT, continuity, systemState, status_message);
        Fuel_Servo.write(5); 
        Ox_Servo.write(5); 
        Ox_load.write(5); 
        CO2_Servo.write(0); 
        camservo_fired = false;
      }
      else if (systemState == ARMED && command == "LOAD") {
        systemState = LOADING;
        Ox_load.write(70); 
        status_message = "Loading servos";
        sendData(radio, PT, continuity, systemState, status_message);
        load_timer = millis();
      }
      else if (systemState == LOADING && (command == "LOADED" || (millis() - load_timer) > 5000)) {
        systemState = LOADED;
        Ox_load.write(5); 
        status_message = "Servos loaded";
        sendData(radio, PT, continuity, systemState, status_message);
      }
      else if (command == "FIRE" && systemState != FIRE) {
        systemState = FIRE;
        // Start the timer and fire everything
        servo_timer = millis();
        cam_timer = millis();   // initialize cam timer
        
        // analogWrite(sprkPin, 255);
        triggerFire(); 
        delay(300); 
        Fuel_Servo.write(65);
        Ox_Servo.write(65);
        Cam_Servo.write(60); 
        camservo_fired = true;

        status_message = "Servos & pyro fired";
        sendData(radio, PT, continuity, systemState, status_message);
      }
      else if (command == "PURGE") {
        CO2_Servo.write(65); 
        CO2_timer = millis();
        systemState = PURGE; 
        status_message = "Purging system";
        sendData(radio, PT, continuity, systemState, status_message);
      }
      else if (command == "PURGED") {
        systemState = PURGED;
        CO2_Servo.write(0); 
        status_message = "System purged";
        sendData(radio, PT, continuity, systemState, status_message);
      }

      else {
        status_message = "Last command Unknown";
        sendData(radio, PT, continuity, systemState, status_message);
      }
    }
    if (systemState == FIRE && (millis() - servo_timer > 1000)) {
        digitalWrite(LAUNCH_PYRO_FIRE, LOW); 
        digitalWrite(SEP_PYRO_FIRE, LOW);
    }
    if (systemState == FIRE && millis() - cam_timer >= 1500) {

        cam_timer = millis();  // reset timer

        if (camservo_fired) {
            Cam_Servo.write(70);
            camservo_fired = false;
        } else {
            Cam_Servo.write(60);
            camservo_fired = true;
        }
}

    if (systemState == FIRE && (millis() - servo_timer > 15000)) {
        Fuel_Servo.write(5); 
        
        if (status_message == "Servos & pyro fired") {
           status_message = "Firing complete";
           sendData(radio, PT, continuity, systemState, status_message);
        }
    }

    count++;
      delay(100); 

  }




