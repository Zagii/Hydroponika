#include <Arduino.h>
// Load Wi-Fi library
#include <ESP8266WiFi.h>
#include "sekret.h" // tu trzymane są namiary na wrazliwe dane przyklad pliku sekret.h_sample
#include "definicje.h"
#include "epromLib.h"
#include "konfigPortal.h"
#include "pompka.h"

Cpompka pompka;
CkonfigPortal konfogPortal;
CParams params;


void setup() {
  Serial.begin(115200);
  EEPROM.begin(16);
  // Set outputs to LOW
  pinMode(pinADC,INPUT);
  pinMode(pinKonfigBtn,INPUT_PULLUP);
  pinMode(LED_BUILTIN,OUTPUT);
  pompka.begin(pinPompka);
  pompka.wlaczPompke();

  odczytEEPROM(params.czasOn,params.czasOff);
  Serial.print("czasOn: ");Serial.println(params.czasOn);
  Serial.print("czasOff: ");Serial.println(params.czasOff);

  // Connect to Wi-Fi network with SSID and password
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  int i=0;
  while (WiFi.status() != WL_CONNECTED && i<20) {
    delay(200);
    Serial.print(".");
    i++;
  }
  // Print local IP address and start web server
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  konfogPortal.begin();
}



int calcVbat()
{
  int adc=analogRead(pinADC);
  Serial.print("adc "); Serial.println(adc);
  Serial.print("mnoznik "); Serial.println(params.batMnoznik,6);

  float v=adc*params.batMnoznik;
  Serial.print(v,6);
  Serial.print("V "); Serial.println(v,6);

  params.vBatStr=String(v);
  return adc;
}



unsigned long ms=0;
void loop(){
 if(digitalRead(pinKonfigBtn)==LOW)
 {
   konfogPortal.setKonfigMode(true);
 }else konfogPortal.setKonfigMode(false);


  if(millis()-ms>1000)
  {
    ms=millis();
    params.licznik_sekund++;
    Serial.print(".");
   // calcVbat();
  }
  if(pompka.getStan()==HIGH)//pompka pracuje czyli stan on
  {
    if(params.licznik_sekund>=(params.czasOn*60)) 
    {
      pompka.wylaczPompke();
      params.licznik_sekund=0;
    }
  }else
  {
    if(params.licznik_sekund>=(params.czasOff*60)) 
    {
      pompka.wlaczPompke();
      params.licznik_sekund=0;    
    }
    if(!konfogPortal.getKonfigMode())
    { // idz spac
      uint64_t sen= params.czasOff*60-params.licznik_sekund;
      ESP.deepSleep(sen*1e6);
    }
  }

  if(konfogPortal.loop(params,pompka.getStan()))
  {
    pompka.wlaczPompke();
  }
  delay(1);
}