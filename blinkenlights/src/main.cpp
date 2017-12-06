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

#define LED1 5
#define LED2 4
#define BUTTON 12

void setup() {

  Serial.begin(9600);
  Serial.println();

  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);

  pinMode(BUTTON, INPUT);

  digitalWrite(LED1, LOW);
  digitalWrite(LED2, LOW);

  Serial.println("Los geht's");
}

void loop() {

  bool clicked = digitalRead(BUTTON);
  if ( clicked ) {
    digitalWrite(LED2, HIGH);
  } else {
    digitalWrite(LED2, LOW);
  }

  delay(500);
  digitalWrite(LED1, LOW);
  delay(500);
  digitalWrite(LED1, HIGH);
}
