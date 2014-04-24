#ifndef PX_RFID_H 
#define PX_RFID_H

#include <Arduino.h>
#include <SoftwareSerial.h>
#include <String.h>

#define PX_RFID_BAUD   2400
#define PX_RFID_START  0x0A
#define PX_RFID_END    0x0D
#define PX_RFID_LENGTH 10

#define PX_RFID_PIN_ENABLE 2
#define PX_RFID_PIN_RX     3
#define PX_RFID_PIN_TX     8

/*!
 * This RFID reader provides a 10-digit hexidecimal token represented
 * in ASCII.  The reader communicates over a 2400-baud serial connection
 * (ParallaxRFIDClass#PX_RFID_BAUD). The start of the tag data is represented by
 * the byte 0x0A (ParallaxRFIDClass#PX_RFID_START) and the end by 0x0D
 * (ParallaxRFIDClass#PX_RFID_END).
 *
 * The RFID reader is enabled (ParallaxRFIDClass#enableRFID) by lowering the
 * digital pin connected to
 * \\ENABLE on the RFID board (ParallaxRFIDClass#setPinEnable). Is is disabled
 * by setting the pin to high (ParallaxRFIDClass#disableRFID).
 *
 * \brief A set of convenience functions for reading from a Parallax RFID reader.
 */
class ParallaxRFIDClass
{
  private:
    SoftwareSerial _RFIDReader;
  public:
    ParallaxRFIDClass();
    ~ParallaxRFIDClass() {}
    String read(); //!< Reads an RFID tag
    void enableRFID(); //!< Enables the RFID reader
    void disableRFID(); //!< Disables the RFID reader
    void begin(); //!< Sets the pin mode of the \\ENABLE pin (ParallaxRFIDClass#setPinEnable) as OUTPUT and disables the RFID reader
};

extern ParallaxRFIDClass ParallaxRFID;

#endif
