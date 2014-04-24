// Doorbot - 2011 OhmBase.org

#define DOOR_PIN 9

#include <Arduino.h>
#include <String.h>
#include <EEPROM.h>
#include <SoftwareSerial.h>

#include "IDStore.h"
#include "ParallaxRFID.h"
#include "AdminConsole.h"

void setup()
{
  pinMode(DOOR_PIN,OUTPUT); // On-board LED
  Serial.begin(9600); // On-board serial
  
  ParallaxRFID.begin(); // RFID reader
  
#ifdef DEBUG
  //IDStore.clearAll();
#endif
}

//LOOP

void loop()
{ 
  // Read RFID
  String rfidKey = ParallaxRFID.read();
  
  // TODO interrupt-based console access?
  // TODO passworded console?
  // TODO valid key only for console? (who is the first then?)
  if (Serial.available())
  {
    AdminConsole.start();
  }
  else if (IDStore.find(rfidKey) >= 0)
  {
    // TODO Play success tone
    // TODO Unlock the door
    
    // Unlock the door
    digitalWrite(DOOR_PIN,HIGH);   
    
    // Wait an acceptible time for the person to enter
    delay(2000);
    
    // Lock the door
    digitalWrite(DOOR_PIN,LOW);
    
  }
  else
  {
    // Play fail tone
  }
}
