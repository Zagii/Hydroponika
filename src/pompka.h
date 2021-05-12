#ifndef POMPKA_H
#define POMPKA_H
#include <Arduino.h>

class Cpompka
{
    uint8_t pinOut;
    uint8_t stanPompka;
    public:
    void begin(uint pin)
    {
        Serial.println("Pompka Init");
        pinOut=pin;
        pinMode(pinOut, OUTPUT);
        wylaczPompke();
    }
    uint8_t getStan(){return stanPompka;}
    void wylaczPompke()
    {
        Serial.println("- Pompka OFF");
        stanPompka = LOW;
        digitalWrite(LED_BUILTIN, HIGH);
        digitalWrite(pinOut, HIGH);
    }
    void wlaczPompke()
    {
        Serial.println("- Pompka ON");
        stanPompka = HIGH;
        digitalWrite(LED_BUILTIN, LOW);
        digitalWrite(pinOut, LOW);
    }
  
};

#endif