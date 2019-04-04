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

#define ROT D1
#define GELB D2
#define BUTTON D6
#define GRUEN D5

void setup() {

  Serial.begin(9600);
  Serial.println();

  pinMode(ROT, OUTPUT);
  pinMode(GELB, OUTPUT);
  pinMode(GRUEN, OUTPUT);

  pinMode(BUTTON, INPUT);

  digitalWrite(ROT, LOW);
  digitalWrite(GELB, LOW);
  digitalWrite(GRUEN, LOW);

  Serial.println("Los geht's");
}
  bool clicked2 = false;
void loop() {

  bool clicked = digitalRead(BUTTON);
  if ( clicked || clicked2 ) {
    clicked2=false;
    digitalWrite(GELB, HIGH);
    digitalWrite(GRUEN, LOW);
    digitalWrite(ROT, LOW);
    delay(4000);
    digitalWrite(GELB, LOW);
    digitalWrite(ROT, HIGH);
    delay(10000);
    digitalWrite(GELB, HIGH);
    delay(2000);
    digitalWrite(GELB, LOW);
    digitalWrite(ROT, LOW);
    digitalWrite(GRUEN, HIGH);
    for (int i=0; i<100; i++) {
    delay(100);
    if (digitalRead(BUTTON)){
    clicked2=true;
    }
    }




  } else {
    digitalWrite(GRUEN, HIGH);
  }
  }
