#ifndef EPROM_LIB_H
#define EPROM_LIB_H
#include <Arduino.h>
#include <EEPROM.h>

int epromAddrON = 0;
//int epromAddrOFF = sizeof(float);


void epromCommit()
{
    if (EEPROM.commit()) {
      Serial.print("EEPROM successfully committed ");
    } else {
      Serial.print("ERROR! EEPROM commit failed ");
    }
}

int aktualizujEEPROM(int adr,String s)
{
  String x;
  int dlugosc;
  EEPROM.get(adr,dlugosc);
  if(dlugosc>MAX_NAZWA) dlugosc = MAX_NAZWA;
  int a = adr + sizeof(int);
  char c;
  for(int i=0;i<dlugosc;i++)
  {
    EEPROM.get(a+i,c);
    x+=c;
  }
  //EEPROM.get(adr,x);
  if(x==s && dlugosc == s.length()){
    Serial.print("EEPROM bez zmian ");Serial.println(s);
     return adr+sizeof(int)+dlugosc*sizeof(char);
  }
  int d=s.length();
  EEPROM.put(adr,d);
  a= adr+sizeof(int);
  for(int i=0;i<dlugosc;i++)
  {
    EEPROM.put(a+i,s.charAt(i));
    
  }
  //EEPROM.put(a,s);
  epromCommit();
   return adr+sizeof(int)+dlugosc*sizeof(char);
}
int aktualizujEEPROM(int adr,int f)
{
  int x;
  EEPROM.get(adr,x);
  if(x==f){
    Serial.print("EEPROM bez zmian ");Serial.println(f);
     return adr+sizeof(int);
  }
  EEPROM.put(adr,f);
  epromCommit();
  return adr+sizeof(int);
}
int aktualizujEEPROM(int adr,float f)
{
  float x;
  EEPROM.get(adr,x);
  if(x==f){
    Serial.print("EEPROM bez zmian ");Serial.println(f);
     return adr+sizeof(float);
  }
  EEPROM.put(adr,f);
  epromCommit();
  return adr+sizeof(float);
}

//void zapiszEEPROM(float czasOn, float czasOff)
void zapiszEEPROM(CParams& p)
{
    int a= aktualizujEEPROM(epromAddrON, p.czasOn);
  a = aktualizujEEPROM(a,p.czasOff);
  a = aktualizujEEPROM(a,p.batMnoznik);
  a = aktualizujEEPROM(a,p.nazwa);
}
void odczytEEPROM(CParams& p)
{
  int a = epromAddrON;
  EEPROM.get(a,p.czasOn);
  if(p.czasOn<0.1||p.czasOn>60)p.czasOn=2;
  a = a + sizeof(float);

  EEPROM.get(a,p.czasOff);
  a = a + sizeof(float);
  if(p.czasOff<0.1||p.czasOff>60)p.czasOff=15;

  EEPROM.get(a,p.batMnoznik);
  if(p.batMnoznik<0 || p.batMnoznik>1) p.batMnoznik=0.5;
  a = a + sizeof(float);

  int dlugosc;
  EEPROM.get(a,dlugosc);
  if(dlugosc>MAX_NAZWA) dlugosc = MAX_NAZWA;
  a = a + sizeof(int);
  char c;
  p.nazwa="";
  for(int i=0;i<dlugosc;i++)
  {
    EEPROM.get(a+i,c);
    p.nazwa+=c;
  }
  //EEPROM.get(a,p.nazwa);
}

#endif