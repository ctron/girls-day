/********************************************************************************
* Copyright (c) 2017, 2018 Red Hat Inc and others
*
* This program and the accompanying materials are made available under the
* terms of the Eclipse Public License 2.0 which is available at
* http://www.eclipse.org/legal/epl-2.0.
*
* SPDX-License-Identifier: EPL-2.0
********************************************************************************/

#include <Arduino.h>
#include <SPI.h>

#define LED_RED D1
#define LED_YELLOW D2
#define LED_GREEN D5
#define BUTTON D6

void setup() {

  Serial.begin(9600);
  Serial.println();

  pinMode(LED_RED, OUTPUT);
  pinMode(LED_YELLOW, OUTPUT);
  pinMode(LED_GREEN, OUTPUT);

  pinMode(BUTTON, INPUT);

  digitalWrite(LED_RED, HIGH);
  digitalWrite(LED_YELLOW, LOW);
  digitalWrite(LED_GREEN, LOW);

  Serial.println("Los geht's");
}

void switch_to_green () {
  digitalWrite(LED_YELLOW, HIGH);
  delay(2000);
  digitalWrite(LED_RED, LOW);
  digitalWrite(LED_YELLOW, LOW);
  digitalWrite(LED_GREEN, HIGH);
}

void switch_to_red () {
  digitalWrite(LED_YELLOW, HIGH);
  digitalWrite(LED_GREEN, LOW);
  delay(2000);
  digitalWrite(LED_RED, HIGH);
  digitalWrite(LED_YELLOW, LOW);
}

void loop() {

  bool clicked = digitalRead(BUTTON);
  if ( clicked ) {
    switch_to_green ();
    delay(5000);
    switch_to_red();
  }

}
