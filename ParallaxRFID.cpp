#include "ParallaxRFID.h"

ParallaxRFIDClass::ParallaxRFIDClass()
 : _RFIDReader(PX_RFID_PIN_RX, PX_RFID_PIN_TX)
{
}

String ParallaxRFIDClass::read()
{
  int bytesRead = -1;
  char tagByte = '\0';
  String tagData = "";
  
  // Flush serial buffer before reading the RFID
  while(_RFIDReader.available() > 0) {
    _RFIDReader.read();
  }
  
  // Enable the RFID reader
  enableRFID();
  
  while(bytesRead <= PX_RFID_LENGTH)
  {
      // Read the byte from the RFID reader
      tagByte = _RFIDReader.read();
      
      if (bytesRead >= 0) // (i.e. we're currently populating the tagData)
      {
        // Should we stop reading?
        if(bytesRead >= PX_RFID_LENGTH || tagByte == PX_RFID_START || tagByte == PX_RFID_END)
        {
          disableRFID();
          break; // stop reading
        }
        
        // Append the RFID data
        // ... we're dropping bytes because of weird bytes >= Arduino 1.0
        if ((tagByte >= 'A' && tagByte <= 'F') || (tagByte >= '0' && tagByte <= '9')) { 
          tagData.concat(tagByte);
          bytesRead++;
        }
      }
      else if(tagByte == PX_RFID_START) // we got the "start" byte of the data
      {
        bytesRead = 0; // signals that we start reading into tagData[]
      }
  }
  
  // Disable RFID (just to be safe)
  disableRFID();
  
  // Return result
  if (bytesRead == PX_RFID_LENGTH)
    return tagData;
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
  pinMode(PX_RFID_PIN_TX,OUTPUT);
  pinMode(PX_RFID_PIN_ENABLE, OUTPUT);
  
  _RFIDReader.begin(PX_RFID_BAUD);
  
  disableRFID();
}

ParallaxRFIDClass ParallaxRFID; // Global instance
