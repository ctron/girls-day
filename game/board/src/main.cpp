/********************************************************************************
* Copyright (c) 2017 Red Hat Inc and others
*
* This program and the accompanying materials are made available under the
* terms of the Eclipse Public License 2.0 which is available at
* http://www.eclipse.org/legal/epl-2.0.
*
* SPDX-License-Identifier: EPL-2.0
********************************************************************************/

#include <Arduino.h>

#include <SPI.h>
#include <ESP8266WiFi.h>
#include <Adafruit_MQTT.h>
#include <Adafruit_MQTT_Client.h>

#define LED 5
#define LED2 4
#define BUTTON 12

#define WIFI_SSID "Red Hat Guest"
#define WIFI_PASSWORD "" /* INSERT WIFI PASSWORD HERE */

#define MQTT_TOPIC_PREFIX "/GirlsDay2017/redhat/munich"
#define MQTT_SERVER "iot.eclipse.org"
#define MQTT_PORT 1883

// #define DEVICE_ID "oliver"

WiFiClient client;

Adafruit_MQTT_Client mqtt(&client, MQTT_SERVER, MQTT_PORT);

Adafruit_MQTT_Publish *  clickedPub;
Adafruit_MQTT_Publish * clientStatePub;
Adafruit_MQTT_Subscribe * serverStateSub;

uint32_t state = 0;

void MQTT_connect();

void serverChanged(uint32_t);

String prefix() {
  String prefix = String(MQTT_TOPIC_PREFIX);
  prefix.concat("/");

  #ifdef DEVICE_ID
  prefix.concat(DEVICE_ID);
  #else
  prefix.concat(WiFi.macAddress());
  #endif

  prefix.replace(':', '-');
  return prefix;
}

String * topicClicked;
String * topicClient;
String * topicServer;

void setup() {

  Serial.begin(9600);
  Serial.println();

  pinMode(LED, OUTPUT);
  pinMode(LED2, OUTPUT);

  pinMode(BUTTON, INPUT);

  digitalWrite(LED, LOW);
  digitalWrite(LED2, LOW);

  Serial.println("Connecting to WiFi ...");

  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  topicClicked = new String(prefix());
  topicClicked->concat("/clicked");

  topicClient = new String(prefix());
  topicClient->concat("/client");

  topicServer = new String(prefix());
  topicServer->concat("/server");

  clickedPub = new Adafruit_MQTT_Publish(&mqtt, topicClicked->c_str());
  clientStatePub  = new Adafruit_MQTT_Publish(&mqtt, topicClient->c_str());
  serverStateSub = new Adafruit_MQTT_Subscribe(&mqtt, topicServer->c_str());

  Serial.println("Topics:");
  Serial.println(*topicClicked);
  Serial.println(*topicClient);
  Serial.println(*topicServer);

  Serial.print("MAC Address: ");
  Serial.println(WiFi.macAddress());

  while ( !WiFi.isConnected() ) {
    delay(100);
  }

  serverStateSub->setCallback(serverChanged);
  mqtt.subscribe(serverStateSub);

  Serial.println("WiFi connected!");
}

int clicks = 0;
bool lastClicked = false;

void loop() {

  MQTT_connect();

  mqtt.processPackets(10);

  digitalWrite(LED, ( state & 1 ) > 0 ? HIGH : LOW );
  digitalWrite(LED2, ( state & 2 ) > 0 ? HIGH : LOW );

  bool clicked = digitalRead(BUTTON);

  if ( !lastClicked && clicked ) {
    clicks++;
    clickedPub->publish(clicks);
    Serial.println(clicks);
  }
  lastClicked = clicked;

}

void serverChanged(uint32_t _state) {
  Serial.print ( "Server state: ");
  Serial.println ( _state );

  state = _state;
}

void MQTT_connect() {
  int8_t ret;

  // Stop if already connected.
  if (mqtt.connected()) {
    return;
  }

  Serial.print("Connecting to MQTT... ");

  uint8_t retries = 3;
  while ((ret = mqtt.connect()) != 0) { // connect will return 0 for connected
       Serial.println(mqtt.connectErrorString(ret));
       Serial.println("Retrying MQTT connection in 5 seconds...");
       mqtt.disconnect();
       delay(5000);  // wait 5 seconds
       retries--;
       if (retries == 0) {
         // basically die and wait for WDT to reset me
         while (1);
       }
  }

  clientStatePub->publish(1);

  mqtt.will(topicClient->c_str(), "0", 1, 1);

  Serial.println("MQTT Connected!");
}
