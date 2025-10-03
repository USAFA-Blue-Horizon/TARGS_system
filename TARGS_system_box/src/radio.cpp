#include "../include/radio.h"
#include "../include/params.h"



bool initializeCommunication(RH_RF95* rf95) {
    if (!rf95->init()) {
        Serial.println("RF95 LoRa init failed!");
        return false;
    }
    
    // Configure radio parameters https://www.rfwireless-world.com/calculators/LoRa-Data-Rate-Calculator.html

    rf95->setFrequency(RF95_FREQ);
    rf95->setTxPower(23, false);
    rf95->setSpreadingFactor(9);
    rf95->setSignalBandwidth(500000);
    rf95->setCodingRate4(5);
    
    Serial.println("LoRa radio initialized");
    return true;
}

String checkForCommands(RH_RF95* rf95) {
    String command = ""; 

    if (rf95->available()) {
        // Buffer for received message
        uint8_t buf[250]; // Buffer for received message 250 bytes
        uint8_t len = sizeof(buf);
        
        // Read the message
        if (rf95->recv(buf, &len)) { //recv returns true if a message is received, buf is the buffer, len is the length of the message
            buf[len] = '\0';  // Null-terminate buffer so we can treat it as a string
            String receivedCommand = String((char*)buf); 
            receivedCommand.trim(); // Remove any leading/trailing whitespace
            
            Serial.print("LoRa received: "); 
            Serial.println(receivedCommand);
            
            command = receivedCommand;
            }
        }
    return command; 
}


void sendData(RH_RF95* rf95, double pt, double continuity1, int state, String extraInfo) {
    // Create a buffer to hold the data
    uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
    // Format the data into a string
    int len = snprintf((char*)buf, sizeof(buf), "PT:%.2f,\nC1:%.2f,\nState:%d,\nStatus:%s", pt, continuity1, state, extraInfo.c_str());
    // Send the data
    if (len > 0 && len < sizeof(buf)) {
        rf95->send(buf, len);
        rf95->waitPacketSent();
    }
}

