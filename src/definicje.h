#ifndef DEFINICJE_H
#define DEFINICJE_H
#include <Arduino.h>


#define MAX_NAZWA 30

// piny
const uint8_t pinPompka=D3;
const uint8_t pinADC=A0;
const uint8_t pinKonfigBtn=D5;
const uint8_t pinTemp=D4;

class CParams
{
    public:
    float czasOn=3;
    float czasOff=10;
    String tempStr="x.x";
    String vBatStr="";
    String nazwa="brak";

    /*
    wyliczenie mnoznika teoretyczne, w praktyce lepiej chyba dobrać wartość z miernikiem
    const float R1=12.00;
    const float R2=22.00;
    const float R=(R1 + R2)/R2;*/
    float batMnoznik=0.005755;//R*3.3/1023;
    unsigned long licznik_sekund=0;
    void getStatusStr(char* str,uint8_t stan, uint8_t konf, float temp)
    {
        tempStr=String(temp,2);
        sprintf(str,"{\"id\":\"%s\",\"t\":%s,\"s\":%d,\"bat\":%s,"
        "\"on\":%3.1f,\"off\":%3.1f,\"konf\":%d,\"ip\":\"%s\"}",
        nazwa.c_str(), 
        tempStr.c_str(),stan,
        vBatStr.c_str(),
        czasOn,czasOff,konf,WiFi.localIP().toString().c_str());
    }
};

#endif