/********************************************************************************
* Copyright (c) 2019 Red Hat Inc and others
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

#define LED_RED D1
#define LED_YELLOW D2
#define LED_GREEN D5
#define BUTTON D6

#define WIFI_SSID "" /* CHANGE WIFI NAME HERE */
#define WIFI_PASSWORD "" /* INSERT WIFI PASSWORD HERE */

#define SIGNAL_LOCAL "SectorZZ9PluralZAlpha" /* CHANGE LOCAL LOCATION  */
#define SIGNAL_REMOTE "Islington" /* CHANGE REMOTE LOCATION */

#define MQTT_TOPIC_PREFIX "/GirlsDay/redhat/"
#define MQTT_SERVER "iot.eclipse.org"
#define MQTT_PORT 1883

WiFiClient client;

Adafruit_MQTT_Client mqtt(&client, MQTT_SERVER, MQTT_PORT);

Adafruit_MQTT_Publish * clickedPublish;
Adafruit_MQTT_Subscribe * signalLocal;
Adafruit_MQTT_Subscribe * signalRemote;

uint32_t state = 0;

void MQTT_connect();

void signalChangedLocal(uint32_t);
void signalChangedRemote(uint32_t);

String * topicClickLocal;
String * topicClickRemote;

void setup() {

  Serial.begin(9600);
  Serial.println();
  Serial.println("Local as: " SIGNAL_LOCAL);
  Serial.println("Remote as: " SIGNAL_REMOTE);

  pinMode(LED_RED, OUTPUT);
  pinMode(LED_YELLOW, OUTPUT);
  pinMode(LED_GREEN, OUTPUT);

  pinMode(BUTTON, INPUT);

  digitalWrite(LED_RED, LOW);
  digitalWrite(LED_YELLOW, LOW);
  digitalWrite(LED_GREEN, LOW);

  Serial.println("Connecting to WiFi ...");

  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  topicClickLocal = new String(MQTT_TOPIC_PREFIX);
  topicClickLocal->concat("click/" SIGNAL_LOCAL );

  topicClickRemote = new String(MQTT_TOPIC_PREFIX);
  topicClickRemote->concat("click/" SIGNAL_REMOTE );

  clickedPublish = new Adafruit_MQTT_Publish(&mqtt, topicClickLocal->c_str());
  signalLocal = new Adafruit_MQTT_Subscribe(&mqtt, topicClickLocal->c_str());
  signalRemote = new Adafruit_MQTT_Subscribe(&mqtt, topicClickRemote->c_str());

  Serial.println("Topics:");
  Serial.println(*topicClickLocal);
  Serial.println(*topicClickRemote);

  Serial.print("MAC Address: ");
  Serial.println(WiFi.macAddress());

  while ( !WiFi.isConnected() ) {
    delay(100);
  }

  Serial.println("WiFi connected!");

  signalLocal->setCallback(signalChangedLocal);
  mqtt.subscribe(signalLocal);
  signalRemote->setCallback(signalChangedRemote);
  mqtt.subscribe(signalRemote);
}

bool lastClicked = false;

void loop() {

  if(!mqtt.connected()) {
    digitalWrite(LED_GREEN, LOW);
    digitalWrite(LED_YELLOW, LOW);
    MQTT_connect();
  }

  mqtt.processPackets(10);

  bool clicked = digitalRead(BUTTON);
  digitalWrite(LED_RED, clicked);

  if ( clicked != lastClicked ) {
    clickedPublish->publish(clicked);
    lastClicked = clicked;
  }

}

void signalChangedLocal(uint32_t state) {
  Serial.print ( "Local state: ");
  Serial.println ( state );

  digitalWrite(LED_YELLOW, state>0 ? HIGH : LOW);
}

void signalChangedRemote(uint32_t state) {
  Serial.print ( "Remote state: ");
  Serial.println ( state );

  digitalWrite(LED_GREEN, state>0 ? HIGH : LOW);
}

void MQTT_connect() {
  int8_t ret;

  // Return if already connected.
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

  Serial.println("MQTT Connected!");
}
