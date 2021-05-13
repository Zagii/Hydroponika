#ifndef MQTT_H
#define MQTT_H
#include <Arduino.h>

#include <ESP8266WiFi.h>
#include <PubSubClient.h>

#define MSG_BUFFER_SIZE	(50)
WiFiClient espClient;
PubSubClient client(espClient);

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  // Switch on the LED if an 1 was received as first character
  if ((char)payload[0] == '1') {
    digitalWrite(BUILTIN_LED, LOW);   // Turn the LED on (Note that LOW is the voltage level
    // but actually the LED is on; this is because
    // it is active low on the ESP-01)
  } else {
    digitalWrite(BUILTIN_LED, HIGH);  // Turn the LED off by making the voltage HIGH
  }

}
class Cmqtt
{   
    unsigned long lastMsg = 0;
    char msg[MSG_BUFFER_SIZE];
    int value = 0;
    const char* mqtt_server = "broker.mqtt-dashboard.com";
    const char* inTopic="hydro";
    const char* outTopic="hydro";

public:
    Cmqtt(){};
    void begin()
    {
        //client.setServer(mqtt_server, 1883);
        IPAddress ip(192,168,1,132);
        client.setServer(ip, 1883);
        client.setCallback(callback);
    }
    void reconnect() {
    // Loop until we're reconnected
        if (!client.connected()) {
            Serial.print("Attempting MQTT connection...");
            // Create a random client ID
            String clientId = "Hydro";
            clientId += String(ESP.getChipId());
            // Attempt to connect
            if (client.connect(clientId.c_str())) {
            Serial.println("connected");
            // Once connected, publish an announcement...
            client.publish(outTopic, "hello world");
            // ... and resubscribe
            client.subscribe(inTopic);
          
            } else {
            Serial.print("failed, rc=");
            Serial.println(client.state());
            delay(300);
            }
        }
    }

    void publish( const char* msg)
    {
        if (!client.connected()) {
            reconnect();
        }
        client.publish(outTopic, msg);
        
    }

    void loop() { 

        if (!client.connected()) {
            reconnect();
        }
        client.loop();

        unsigned long now = millis();
        if (now - lastMsg > 15000) {
            lastMsg = now;
            ++value;
         //   snprintf (msg, MSG_BUFFER_SIZE, "hello world #%ld", value);
          //  Serial.print("Publish message: ");
          //  Serial.println(msg);
          //  client.publish(outTopic, msg);
        }
    }
};
#endif