#include <Arduino.h>
// Load Wi-Fi library
#include <ESP8266WiFi.h>
#include <EEPROM.h>
#include "sekret.h" // tu trzymane są namiary na wrazliwe dane przyklad pliku sekret.h_sample

// siec
const char* ssid     = NAZWA_SSID;
const char* password = HASLO_WIFI;

// piny
const uint8_t pinPompka=D3;
const uint8_t pinADC=A0;
const uint8_t pinKonfigBtn=D4;
const uint8_t pinTemp=D5;

// Set web server port number to 80
WiFiServer server(80);
String header;

int epromAddrON = 0;
int epromAddrOFF = sizeof(float);


float czasOn=3;
float czasOff=10;
String tempStr="x.x";
String vBatStr="";

/*
wyliczenie mnoznika teoretyczne, w praktyce lepiej chyba dobrać wartość z miernikiem
const float R1=12.00;
const float R2=22.00;
const float R=(R1 + R2)/R2;*/
 float batMnoznik=0.005755;//R*3.3/1023;


// Current time
unsigned long currentTime = millis();
// Previous time
unsigned long previousTime = 0; 
// Define timeout time in milliseconds (example: 2000ms = 2s)
const long timeoutTime = 2000;

bool konfigMode=true;
unsigned long konfigModeMs=0;

void updateKonfigTime()
{
  konfigModeMs=millis();
}
void setkonfigMode(bool m)
{
  konfigMode=m;
  if(m)
  {
    konfigModeMs=millis();
     server.begin();
  }else
  {
    server.stop();
  }
}

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

void zapiszEEPROM()
{
  aktualizujEEPROM(epromAddrON, czasOn);
  aktualizujEEPROM(epromAddrOFF, czasOff);
}
void odczytEEPROM()
{
  //czasOn=EEPROM.read(epromAddrON);
  EEPROM.get(epromAddrON,czasOn);
  if(czasOn<0.1||czasOn>60)czasOn=2;
  //czasOff=EEPROM.read(epromAddrOFF);
  EEPROM.get(epromAddrOFF,czasOff);
  if(czasOff<0.1||czasOff>60)czasOff=15;
  
}
char t[30];
unsigned long licznik_sekund=0;
uint8_t stanPompka=LOW;

void wylaczPompke()
{
  stanPompka=LOW;
  Serial.print(licznik_sekund);
  licznik_sekund=0;
  Serial.println("- Pompka OFF");
  digitalWrite(LED_BUILTIN,HIGH);
   digitalWrite(pinPompka,HIGH);
}
void wlaczPompke()
{
  stanPompka=HIGH;
  Serial.print(licznik_sekund);
  licznik_sekund=0;
  Serial.println("- Pompka ON");
  digitalWrite(LED_BUILTIN,LOW);
   digitalWrite(pinPompka,LOW);
}
void initPompka()
{
  Serial.println("Pompka Init");
  wlaczPompke();
}

void setup() {
  Serial.begin(115200);
  EEPROM.begin(16);
  // Set outputs to LOW
  pinMode(pinPompka, OUTPUT);
  initPompka();
  pinMode(LED_BUILTIN,OUTPUT);
  pinMode(pinADC,INPUT);

  // Connect to Wi-Fi network with SSID and password
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  // Print local IP address and start web server
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  odczytEEPROM();
  Serial.print("czasOn: ");Serial.println(czasOn);
  Serial.print("czasOff: ");Serial.println(czasOff);
 
  setkonfigMode(true);
}



int calcVbat()
{
  int adc=analogRead(pinADC);
Serial.print("adc "); Serial.println(adc);
Serial.print("mnoznik "); Serial.println(batMnoznik,6);

float v=adc*batMnoznik;
Serial.print(v,6);
Serial.print("V "); Serial.println(v,6);

  vBatStr=String(v);
  return adc;
}

void checkWWW()
{
  
  WiFiClient client = server.available();   // Listen for incoming clients

  if (client) {                             // If a new client connects,
    Serial.println("New Client.");          // print a message out in the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    currentTime = millis();
    previousTime = currentTime;
    while (client.connected() && currentTime - previousTime <= timeoutTime) { // loop while the client's connected
      currentTime = millis();         
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out the serial monitor
        header += c;
        if (c == '\n') {                    // if the byte is a newline character
          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();
            
            if (header.indexOf("GET /czasOn") >= 0) {
              Serial.print("czas On ");
              String s=header.substring(header.indexOf("GET /czasOn"));
              s=s.substring(s.indexOf("czasOn")+7,s.indexOf("HTTP/")-1);
              Serial.println(s);
              float tmpf=s.toFloat();
              if(tmpf>0.1)
                {
                  czasOn=tmpf;
                  zapiszEEPROM();
                  initPompka();
                }
            } else if (header.indexOf("GET /czasOff") >= 0) {
              Serial.print("czas Off ");
              String s=header.substring(header.indexOf("GET /czasOff"));
              s=s.substring(s.indexOf("czasOff")+8,s.indexOf("HTTP/")-1);
              Serial.println(s);
              float tmpf=s.toFloat();
              if(tmpf>0.1)
                {
                  czasOff=tmpf;
                  zapiszEEPROM();
                  initPompka();
                }
            }else if (header.indexOf("GET /mnoznik") >= 0) {
              Serial.print("mnoznik ");
              String s=header.substring(header.indexOf("GET /mnoznik"));
              s=s.substring(s.indexOf("mnoznik")+8,s.indexOf("HTTP/")-1);
              Serial.println(s);
              batMnoznik=s.toFloat();
            }
            
            // Display the HTML web page
            client.println("<!DOCTYPE html><html>");
            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<link rel=\"icon\" href=\"data:,\">");
            // CSS to style the on/off buttons 
            // Feel free to change the background-color and font-size attributes to fit your preferences
            client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
            client.println(".button { background-color: #195B6A; border: none; color: white; padding: 16px 40px;");
            client.println("text-decoration: none; font-size: 20px; margin: 2px; cursor: pointer;}");
            client.println(".button2 {background-color: #77878A;}</style></head>");
            
            // Web Page Heading
            client.println("<body><h1>Hydroponika Web Server</h1>");
            
           client.println("<p>adc: " + String(calcVbat()) + "V. </p>"); 
            client.println("<p>vBat: " + vBatStr + "V. </p>");
            client.println("<p>Temp: " + tempStr + "C</p>");
            client.println("<p>Mnoznik: " + String(batMnoznik,6) + "</p>");
            if(stanPompka==HIGH)
            {
              client.println("<p>Pompka ON jeszcze " + String(czasOn*60 - licznik_sekund) + "</p>");
            }else
            {
              client.println("<p>Pompka OFF jeszcze " + String(czasOff*60 - licznik_sekund) + "</p>");
            }
            client.println("<p>licznik: " + String(licznik_sekund) + ", <a href=\"/\">odswiez<a></p>");
            sprintf(t,"/czasOn/%3.1f",czasOn+0.5);
            client.println("<p>czasOn: " + String(czasOn) + "<a href=\""+String(t)+"\"><button class=\"button\">+30s:"+String(czasOn+0.5)+"</button></a>");
            sprintf(t,"/czasOn/%3.1f",czasOn-0.5);
            client.println("<a href=\""+String(t)+"\"><button class=\"button\">-30s:"+String(czasOn-0.5)+"</button></a></p>");
            
            sprintf(t,"/czasOff/%3.1f",czasOff+0.5);
            client.println("<p>czasOff: " + String(czasOff) + "<a href=\""+String(t)+"\"><button class=\"button\">+30s:"+String(czasOff+0.5)+"</button></a>");
            sprintf(t,"/czasOff/%3.1f",czasOff-0.5);
            client.println("<a href=\""+String(t)+"\"><button class=\"button\">-30s:"+String(czasOff-0.5)+"</button></a></p>");

            client.println("</body></html>");
            
            // The HTTP response ends with another blank line
            client.println();
            // Break out of the while loop
            break;
          } else { // if you got a newline, then clear currentLine
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }
      }
    }
    // Clear the header variable
    header = "";
    // Close the connection
    updateKonfigTime();
    client.stop();
    Serial.println("Client disconnected.");
    Serial.println("");
  }
}

unsigned long ms=0;
void loop(){
 

  if(millis()-ms>1000)
  {
    ms=millis();
    licznik_sekund++;
    Serial.print(".");
   // calcVbat();
  }
  if(stanPompka==HIGH)//pompka pracuje czyli stan on
  {
    if(licznik_sekund>=(czasOn*60)) wylaczPompke();
  }else
  {
    if(licznik_sekund>=(czasOff*60)) wlaczPompke();
  }
  
  if(millis()-konfigModeMs>2*60*1000)
  {
    setkonfigMode(false);
  }

  //if(konfigMode)
  checkWWW();
/*Serial.println(millis());
delay(20);
Serial.println("spac");
  ESP.deepSleep(15e6);
Serial.println(millis());
Serial.println("pobudka");*/
}