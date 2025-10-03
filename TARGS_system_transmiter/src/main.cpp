#include <Arduino.h>
#include <radio.h>
#include <params.h>

RH_RF95* radio = new RH_RF95(RFM95_CS, RFM95_INT);

void setup()
{ 
    Serial.begin(9600);
    while (!Serial) 
        ;
    delay(100); // Wait a bit for serial to initialize

    if (!initializeCommunication(radio)) {
        Serial.println("Radio initialization failed. Check connections.");
        while (1);
    }
    Serial.println("Setup complete.");
}

void loop()//loop to send serial inputs commands of string over radio so transmit every user serial line
{
    if (Serial.available()) {
        String input = Serial.readStringUntil('\n');
        input.trim(); // Remove any leading/trailing whitespace
        if (input.length() > 0) {
            sendStringData(radio, input);
            Serial.print("Sent over LoRa: ");
            Serial.println(input);
        }
    }

    String command = checkForCommands(radio);
    if (command.length() > 0) {
        Serial.print("Received command: ");
        Serial.println(command);
    }

    delay(100); // Adjust delay as needed
}