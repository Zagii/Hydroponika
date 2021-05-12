#ifndef EPROM_LIB_H
#define EPROM_LIB_H
#include <Arduino.h>
#include <EEPROM.h>

int epromAddrON = 0;
int epromAddrOFF = sizeof(float);

void aktualizujEEPROM(int adr,float f)
{
  float x;
  EEPROM.get(adr,x);
  if(x==f){
    Serial.print("EEPROM bez zmian ");Serial.println(f);
     return;
  }
  EEPROM.put(adr,f);
  if (EEPROM.commit()) {
      Serial.print("EEPROM successfully committed ");Serial.println(f);
    } else {
      Serial.print("ERROR! EEPROM commit failed ");Serial.println(f);
    }
}

void zapiszEEPROM(float czasOn, float czasOff)
{
  aktualizujEEPROM(epromAddrON, czasOn);
  aktualizujEEPROM(epromAddrOFF, czasOff);
}
void odczytEEPROM(float &czasOn, float &czasOff)
{
  //czasOn=EEPROM.read(epromAddrON);
  EEPROM.get(epromAddrON,czasOn);
  if(czasOn<0.1||czasOn>60)czasOn=2;
  //czasOff=EEPROM.read(epromAddrOFF);
  EEPROM.get(epromAddrOFF,czasOff);
  if(czasOff<0.1||czasOff>60)czasOff=15;
  
}

#endif