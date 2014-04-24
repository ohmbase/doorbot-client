#ifndef BASE_CLIENT_H
#define BASE_CLIENT_H

#include <Arduino.h>
#include <Ethernet.h>
#include <sha256.h>

#include "Key.h"

// Pretty lame, I know, but better than nothing?
#define BASECLIENT_KEY_ITERATIONS 128

class BaseClient : public EthernetClient { 
  private:
    String _id;
    String _keystring;
    Key _key;
    Key _session_key;

  protected:    
    int readBytes(byte* buffer, uint16_t maximum_length, unsigned long timeout_ms = 5000);
    int readBytesUntil(byte* buffer, uint16_t maximum_length, char stop_char, unsigned long timeout_ms = 5000) {

    // Key-related functions
    Key sessionKey() { return _session_key; };
    void advanceSessionKey(); //!< Moves session key forward. Uses HMAC so call after finished using HMAC.

    // Message-related functions
    // TODO initializeMessage with FlashHelper?
    void initializeMessage(String const& message_type, boolean requires_hmac = false); //!< Initializes a message. First thing that is called when sending a message.


    // TODO sendField with FlashHelper?
    void sendField(float value, boolean requires_hmac = false);
    void sendField(int value, boolean requires_hmac = false);
    void sendField(String const& value, boolean requires_hmac = false);
    void sendField(const uint8_t* buffer, size_t buffer_length, boolean requires_hmac = false); //!< Treats bytes as hex (as treated by print(...,HEX))
    // TODO add more versions of sendField for more value types (no templating in Arduino)
    
    void finalizeMessage(boolean requires_hmac = false); //!< Finalizes a message. Called when a message should be done.

  public:
    BaseClient(String const& id, String const& key);

    // Protocol-related functions
    boolean handshake(); //!< Performs the 'hello' handshake. Identifies client with server and generates session key.
    void shutdown(); //!< Disconnects and flushes receive buffer
};

#endif
