/** hardware.cpp
* ===========================================================
* Name: Flight Controller Hardware Implementation
* Section: TVC499
* Project: Flight Controller
* Purpose: LED and pyro channel control
* ===========================================================
*/

#include "../include/hardware.h"
#include "../include/params.h"

bool countDownDone = false; // Flag for countdown completion

void playAlertTone(int frequency, int duration) {
    unsigned long period = 1000000 / frequency; // Period in microseconds
    unsigned long halfPeriod = period / 2; // Half period in microseconds
    unsigned long startTime = millis(); 
    while (millis() - startTime < duration) { 
        digitalWrite(BUZZER_HIGH, HIGH); // Set buzzer high
        digitalWrite(BUZZER_LOW, LOW); // set buzzer low
        delayMicroseconds(halfPeriod); // Wait for half period
        
        digitalWrite(BUZZER_HIGH, LOW);
        digitalWrite(BUZZER_LOW, HIGH);
        delayMicroseconds(halfPeriod);
    }
    
    // Turn off buzzer
    digitalWrite(BUZZER_HIGH, LOW);
    digitalWrite(BUZZER_LOW, LOW);
}

void initializeHardware(bool& separationTriggered, bool& launchTriggered) {
    
    pinMode(BUZZER_HIGH, OUTPUT);
    pinMode(BUZZER_LOW, OUTPUT);
    
    for (int i = 0; i < 4; i++) {
        playAlertTone(2000 + i * 500, 200); // Play tone with increasing frequency 2000, 2500, 3000, 3500 hz
        delay(400); 
    }

    pinMode(LAUNCH_PYRO_FIRE, OUTPUT);
    digitalWrite(LAUNCH_PYRO_FIRE, LOW); 

}

void checkPyroContinuity(double* continuity) {
    continuity[0] = 0.0;
    int pyro1Value = analogRead(LAUNCH_PYRO_CONT); // Read analog value from pyro 1 continuity pin
    int pyro2Value = analogRead(SEP_PYRO_CONT); // Read analog value from pyro 2 continuity pin

    if (pyro1Value > CONTINUITY_THRESHOLD * (3.3 / 1023) && pyro2Value > CONTINUITY_THRESHOLD * (3.3 / 1023)) {
        continuity[0] = 2.0; // Both pyros have continuity
    }
    else if (pyro1Value > CONTINUITY_THRESHOLD * (3.3 / 1023)) { //   3.3/1023 is the conversion factor for 10 bit ADC on teensy (0-1023 to 0-3.3V)
        continuity[0] = 1.0; // Only pyro1 has continuity
    }
    else if (pyro2Value > CONTINUITY_THRESHOLD * (3.3 / 1023)) {
        continuity[0] = 3.0; // Only pyro2 has continuity
    }
}


void triggerFire() {

    digitalWrite(LAUNCH_PYRO_FIRE, HIGH); // Fire pyro
    digitalWrite(SEP_PYRO_FIRE, HIGH);
    
}

void checkPT(double* pt) {
    pinMode(27, INPUT); // PT sensor pin
    float ptValue = analogRead(27) * (3.3 / 1023);
    float val = map(ptValue, 0.3, 3.2, 0, 1600); // Convert to millivolts (0-3.3V range)

    pt[0] = val; // Return pressure value in psi

}

void countDownMusic() {
    if (countDownDone) return; // If countdown is already done, exit function

    else {
        for (int i = 0; i < 4; i++) {
            playAlertTone(2000 + i * 500, 10 / 4); // Play tone with increasing frequency 2000, 2500, 3000, 3500 hz
        }
        countDownDone = true; // Set countdown done flag to true
    }
    
}
