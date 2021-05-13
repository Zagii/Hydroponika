#ifndef TERMOMETRY_H
#define TERMOMETRY_H
#include <OneWire.h>
#include <DallasTemperature.h>
#include "definicje.h"

OneWire oneWire(pinTemp);
DallasTemperature sensors(&oneWire);

class CTermometr{

    public:
        CTermometr(){};
        void begin()
        {
            sensors.begin();
        }
        float getTemp()
        {
            sensors.requestTemperatures(); 
            return sensors.getTempCByIndex(0);
        }

};


#endif