#include <Arduino.h>
// Load Wi-Fi library
#include <ESP8266WiFi.h>
#include "sekret.h" // tu trzymane są namiary na wrazliwe dane przyklad pliku sekret.h_sample
#include "definicje.h"
#include "epromLib.h"
#include "konfigPortal.h"
#include "pompka.h"

Cpompka pompka;
CkonfigPortal konfigPortal;
CParams params;

int buttonState;             // the current reading from the input pin
int lastButtonState = LOW;   // the previous reading from the input pin
unsigned long lastDebounceTime = 0;  // the last time the output pin was toggled
unsigned long debounceDelay = 50; 

int calcVbat();

void setup() {
  delay(2000);
  Serial.begin(115200);
  Serial.println("");
  Serial.println("Start");
  EEPROM.begin(16);
  // Set outputs to LOW
  pinMode(pinADC,INPUT);
  calcVbat();
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
  konfigPortal.begin();
  buttonState=HIGH; // musi byc high bo inicjalnie portal off
}

bool checkBtn()
{
  bool ret=false;
////////////// debounce ///////////////
  int reading = digitalRead(pinKonfigBtn);
  if (reading != lastButtonState) {
    lastDebounceTime = millis();
  }
  if ((millis() - lastDebounceTime) > debounceDelay) {
    if (reading != buttonState) {
      buttonState = reading;
      ret=true;
    }
  }
  lastButtonState = reading;
  /////////  debounce koniec //////////
  return ret;
}

int calcVbat()
{
  int adc=analogRead(pinADC);Serial.println();
  Serial.print("* adc "); Serial.print(adc);
  Serial.print(", mnoznik "); Serial.print(params.batMnoznik,6);

  float v=adc*params.batMnoznik;
  Serial.print(", ");Serial.print(v,6);
  Serial.println("V ");

  params.vBatStr=String(v);
  return adc;
}



unsigned long ms=0;
void loop(){

  if(checkBtn())
  {
    if(buttonState == LOW)
    {
    konfigPortal.setKonfigMode(true);
    }else 
    {
      konfigPortal.setKonfigMode(false);
    }
  }
  if(millis()-ms>1000)
  {
    ms=millis();
    params.licznik_sekund++;
    Serial.print(".");
    Serial.print(buttonState);
    if(params.licznik_sekund % 15 ==0)
    {
      calcVbat();
    }
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
    if(!konfigPortal.getKonfigMode())
    { // idz spac
      uint16_t sen= params.czasOff*60-params.licznik_sekund;
      Serial.print(" deepSleep: ");Serial.println(sen);
      ESP.deepSleep(sen*1e6);
    }
  }

  switch(konfigPortal.loop(params,pompka.getStan()))
  {
    case 1: pompka.wlaczPompke();  break;
    case 0: pompka.wylaczPompke(); break;
    default: break;
  }
  delay(1);
}