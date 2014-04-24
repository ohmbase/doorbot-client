#include "ParallaxRFID.h"

ParallaxRFIDClass::ParallaxRFIDClass()
 : _Serial(PX_RFID_PIN_RX, PX_RFID_PIN_TX)
{
}

String ParallaxRFIDClass::read(unsigned long timeout_ms)
{
  int bytesRead = -1;
  char tagByte = '\0';
  char tagData[PX_RFID_LENGTH+1];
  unsigned long timeout_time_ms = timeout_ms + millis();
  
  // Zero memory
  memset(tagData, 0, PX_RFID_LENGTH+1);
  
  // Flush serial buffer before enabling the RFID
  while(_Serial.available() > 0) {
    _Serial.read();
  }
  
  // Enable the RFID reader
  enableRFID();
  
  while(bytesRead < PX_RFID_LENGTH && (timeout_ms == 0 || millis() < timeout_time_ms))
  {
      // Read the byte from the RFID reader
      tagByte = _Serial.read();
      
      if (tagByte == PX_RFID_START) // we got the "start" byte of the data
      {
        bytesRead = 0; // signals that we start reading into tagData[]
      }
      else if (bytesRead >= 0) // (i.e. we're currently populating the tagData)
      {
        // Append the RFID data
        if ((tagByte >= 'A' && tagByte <= 'F') || (tagByte >= '0' && tagByte <= '9')) { // ... we're selective because of weird bytes >= Arduino 1.0
          tagData[bytesRead++] = tagByte;
        }
      }
      // TODO check PX_RFID_END?
  }
  
  // Disable RFID (just to be safe)
  disableRFID();
  
  // Return result
  if (bytesRead == PX_RFID_LENGTH)
    return String(tagData);
  else
    return "";
}

void ParallaxRFIDClass::enableRFID()
{
  digitalWrite(PX_RFID_PIN_ENABLE, LOW);
}

void ParallaxRFIDClass::disableRFID()
{
  digitalWrite(PX_RFID_PIN_ENABLE, HIGH);
}

void ParallaxRFIDClass::begin()
{
  pinMode(PX_RFID_PIN_RX, INPUT);
  pinMode(PX_RFID_PIN_TX, OUTPUT);
  pinMode(PX_RFID_PIN_ENABLE, OUTPUT);
  
  _Serial.begin(PX_RFID_BAUD);
  
  disableRFID();
}

ParallaxRFIDClass ParallaxRFID; // Global instance
