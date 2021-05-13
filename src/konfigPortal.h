#ifndef KONFIG_PORTAL_H
#define KONFIG_PORTAL_H
#include <Arduino.h>

// siec
const char* ssid     = NAZWA_SSID;
const char* password = HASLO_WIFI;
WiFiServer server(80);


class CkonfigPortal{

    char t[50];
    String header;

    // Current time
    unsigned long currentTime = millis();
    // Previous time
    unsigned long previousTime = 0; 
    // Define timeout time in milliseconds (example: 2000ms = 2s)
    const long timeoutTime = 2000;

    bool konfigMode=false;


public:
    CkonfigPortal(){}
    void begin()
    {
        Serial.println("KonfigPortal.begin");
        konfigMode=false;
        server.begin();
    }
    bool getKonfigMode(){return konfigMode;}
    void setKonfigMode(bool m)
    { 
        if(m==konfigMode)return;
        konfigMode=m;
        Serial.print("KonfigMode: ");
        if(m)
        {
           // server.begin();
            Serial.println(" ON");
        }else
        {
           // server.stop();
            Serial.println(" OFF");
        }
    }
    int loop(CParams &params,uint8_t stanPompka)
    {
    int ret=-1;
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
         //   Serial.write(c);                    // print it out the serial monitor
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
                    params.czasOn=tmpf;
                    zapiszEEPROM(params);
                    ret=1;
                    }
                } else if (header.indexOf("GET /czasOff") >= 0) {
                Serial.print("czas Off ");
                String s=header.substring(header.indexOf("GET /czasOff"));
                s=s.substring(s.indexOf("czasOff")+8,s.indexOf("HTTP/")-1);
                Serial.println(s);
                float tmpf=s.toFloat();
                if(tmpf>0.1)
                    {
                    params.czasOff=tmpf;
                    zapiszEEPROM(params);
                    ret=1;
                    }
                }else if (header.indexOf("GET /mnoznik") >= 0) {
                    Serial.print("mnoznik ");
                    String s=header.substring(header.indexOf("GET /mnoznik"));
                    s=s.substring(s.indexOf("mnoznik")+8,s.indexOf("HTTP/")-1);
                    Serial.println(s);
                    params.batMnoznik=s.toFloat();
                    zapiszEEPROM(params);
                }else if (header.indexOf("GET /on") >= 0) {
                    ret=1;
                }else if (header.indexOf("GET /off") >= 0) {
                    ret=0;
                }else if (header.indexOf("GET /setName") >= 0) {
                    Serial.print("setName ");
                    String s=header.substring(header.indexOf("GET /setName"));
                    s=s.substring(s.indexOf("setName")+8,s.indexOf("HTTP/")-1);
                    Serial.println(s);

                    if(s.length()>0)
                    {
                        params.nazwa=s;
                        zapiszEEPROM(params);
                        ret=1;
                    }
                }
                
                // Display the HTML web page
                client.println("<!DOCTYPE html><html>");
                client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
                client.println("<link rel=\"icon\" href=\"data:,\">");
                // CSS to style the on/off buttons 
                // Feel free to change the background-color and font-size attributes to fit your preferences
                client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
                client.println(".button { background-color: #195B6A; border: none; color: white; padding: 16px 40px;");
                client.println("text-decoration: none; font-size: 14px; margin: 2px; cursor: pointer;}");
                client.println(".button2 {background-color: #77878A;}</style></head>");
                
                // Web Page Heading
                client.println("<body><h1>Hydroponika Web Server</h1>");
                 client.println("<h2>"+params.nazwa+"</h2>");
      //      client.println("<p>adc: " + String(calcVbat()) + "V. </p>"); 
                client.println("<p>vBat: " + params.vBatStr + "V. </p>");
                client.println("<p>Temp: " + params.tempStr + "C</p>");
                client.println("<p>Mnoznik: " + String(params.batMnoznik,6) + "</p>");
                if(stanPompka==HIGH)
                {
                client.println("<p>Pompka ON jeszcze " + String(params.czasOn*60 - params.licznik_sekund) + "</p>");
                }else
                {
                client.println("<p>Pompka OFF jeszcze " + String(params.czasOff*60 - params.licznik_sekund) + "</p>");
                }
                client.println("<p>licznik: " + String(params.licznik_sekund) + ", <a href=\"/\">odswiez<a></p>");
                sprintf(t,"/czasOn/%3.1f",params.czasOn+0.5);
                client.println("<p>czasOn: " + String(params.czasOn) + "<a href=\""+String(t)+"\"><button class=\"button\">+30s</button></a>");
                sprintf(t,"/czasOn/%3.1f",params.czasOn-0.5);
                client.println("<a href=\""+String(t)+"\"><button class=\"button\">-30s</button></a></p>");
                
                sprintf(t,"/czasOff/%3.1f",params.czasOff+0.5);
                client.println("<p>czasOff: " + String(params.czasOff) + "<a href=\""+String(t)+"\"><button class=\"button\">+30s</button></a>");
                sprintf(t,"/czasOff/%3.1f",params.czasOff-0.5);
                client.println("<a href=\""+String(t)+"\"><button class=\"button\">-30s</button></a></p>");
                 client.println("<hr><p><a href=\"/on\"><button class=\"button\">ON</button></a>");
                 client.println("  <a href=\"/off\"><button class=\"button\">OFF</button></a></p>");

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
        client.stop();
        Serial.println("Client disconnected.");
        Serial.println("");
    }
 return ret;
 }
};
#endif
