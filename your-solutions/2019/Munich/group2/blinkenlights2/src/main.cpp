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

#define LED1 D1
#define LED2 D2
#define BUTTON D6
#define LED3 D5

void setup() {

  Serial.begin(9600);
  Serial.println();

  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(LED3, OUTPUT);

  pinMode(BUTTON, INPUT);

  digitalWrite(LED1, HIGH);
  digitalWrite(LED2, LOW);
  digitalWrite(LED3, LOW);

  Serial.println("Los geht's");
}
void gruenSchalten(){
  digitalWrite(LED2, HIGH);
  delay(2000);
  digitalWrite(LED1, LOW);
  digitalWrite(LED2, LOW);
  digitalWrite(LED3, HIGH);
  delay(5000);
  digitalWrite(LED3, LOW);
  digitalWrite(LED2, HIGH);
  delay(2000);
  digitalWrite(LED2, LOW);
  digitalWrite(LED1, HIGH);
}
void loop() {
  for(int i=0; i<25; i++){
    delay(200);
    bool clicked = digitalRead(BUTTON);
    if ( clicked ) {
      break;
    }
  }
  gruenSchalten();
}
