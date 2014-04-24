#include "BaseClient.h"

BaseClient::BaseClient(String const& id, String const& key)
  : _id(id), _keystring(key)
{
}

/*! Reads at most 'buffer_length' bytes into 'buffer'. Reading will stop
 * after 'timeout_ms' milliseconds of no data being available to read.
 * @returns number of bytes read into 'buffer'
 */
int BaseClient::readBytes(byte* buffer, uint16_t maximum_length, unsigned long timeout_ms) {
  uint16_t bytes_read = 0;
  unsigned long last_time_available = millis();
  
  while(bytes_read < buffer_length) {
    if (available()) {
      last_time_available = millis();
      buffer[bytes_read] = EthernetClient::read();
      bytes_read++;
    }
    else if (!connected()) {
      // Nothing is available and we're not connected
      break;
    }
    else if (millis() - last_time_available > timeout_ms) {
      // Nothing is available, we're connected, but it's been too long
      // TODO probably don't need this since ethernet client supports timeouts in v1.0
      break;
    }
  }
  
  return (int)bytes_read;
}

/*! Read bytes into `buffer'. Reading will stop when `maximum_length',
 * bytes have been read, `stop_char' is read, or after `timeout_ms'
 * milliseconds have passed with no data available to read.
 *
 * @returns number of bytes read into 'buffer'
 */
int BaseClient::readBytesUntil(byte* buffer, uint16_t maximum_length, char stop_char, unsigned long timeout_ms) {
  uint16_t bytes_read = 0;
  char c;
  unsigned long last_time_available = millis();
  
  while(bytes_read < buffer_length) {
    if (available()) {
      last_time_available = millis();
      c = EthernetClient::read();
      
      if (c = stop_char) {
        break;
      }
      else {
        buffer[bytes_read] = c;
        bytes_read++;
      }
    }
    else if (!connected()) {
      // Nothing is available and we're not connected
      break;
    }
    else if (millis() - last_time_available > timeout_ms) {
      // Nothing is available, we're connected, but it's been too long
      // TODO probably don't need this since ethernet client supports timeouts in v1.0
      break;
    }
  }
  
  return (int)bytes_read;
}

/** Advances the session key. This is done by hashing the
 * session key with the master key (sorry cryptographers).
 */
void BaseClient::advanceSessionKey() {
  // Hash the session key to advance it to the next key
  
  byte* hmac_result; 
  
  Sha256.initHmac(_key.x, DOORBOT_CLIENT_KEY_SIZE);
  
  // Write the key to the digest
  for (uint8_t i = 0; i < DOORBOT_CLIENT_KEY_SIZE; i++) {
    Sha256.write(_session_key.x[i]);
  }
  
  // Copy digest result back to the key
  hmac_result = Sha256.resultHmac();

  for (uint8_t i = 0; i < DOORBOT_CLIENT_KEY_SIZE; i++) {
    _session_key.x[i] = hmac_result[i];
  }
}


// Protocol ///////////////////////////////////////////////////////////////////

/** Performs the handshake. Here, the client identifies itself
 * and the server provides the session key material. The session
 * key is derived from this key material by hashing it with the
 * master (private/pre-shared) key a "large" number of times.
 * The aim here is to prevent a MITM from spoofing responses or
 * succeeding with replay attacks, not to encrypt/obfuscate data
 * (Arduino isn't powerful enough) or to prevent DoS or any other
 * MITM mischief.
 * 
 * @returns whether or not the handshake was successful
 */
boolean BaseClient::handshake() {
  byte* hmac_result;
  Key key_material;

  // Initialize key
  stringToKey(_keystring, _key);

  // Send 'hello'
  print("h ");
  print(_id);
  print('\n'); // or finalizeMessage(false)
  
  // Get the key material in response
  if (readBytes(key_material.x, DOORBOT_CLIENT_KEY_SIZE) != DOORBOT_CLIENT_KEY_SIZE) {
    return false;
  }
  
  // Perform several iterations of hashing on the key material
  for(uint8_t i = 0; i < BASECLIENT_KEY_ITERATIONS; i++) {
    // Do several rounds on the key material to generate session key (sorry cryptographers)
    Sha256.initHmac(_key.x, DOORBOT_CLIENT_KEY_SIZE);
    
    // Write material to HMAC (first iteration)
    for (uint8_t j = 0; j < DOORBOT_CLIENT_KEY_SIZE; j++) {
      Sha256.write(key_material.x[j]);
    }
    
    hmac_result = Sha256.resultHmac();
    
    // Copy result back into key_material for another iteration...
    for (uint8_t j = 0; j < DOORBOT_CLIENT_KEY_SIZE; j++) {
      key_material.x[j] = hmac_result[j];
    }
    // TODO memcpy?
  }
  
  // Copy result into the session key
  for (uint8_t j = 0; j < DOORBOT_CLIENT_KEY_SIZE; j++) {
    _session_key.x[j] = key_material.x[j];
  }
  
  return true;
}

//!< Initializes message of 'message_type'.
void BaseClient::initializeMessage(String const& message_type, boolean requires_hmac) {
  print(message_type);

  if (requires_hmac) {
    Sha256.initHmac(_session_key.x, DOORBOT_CLIENT_KEY_SIZE);
    Sha256.print(message_type);
  }
}

void BaseClient::sendField(float value, boolean requires_hmac) {
  write(' ');
  print(value);

  if (requires_hmac) {
    Sha256.write(' ');
    Sha256.print(value);
  }
}

void BaseClient::sendField(int value, boolean requires_hmac) {
  write(' ');
  print(value);

  if (requires_hmac) {
    Sha256.write(' ');
    Sha256.print(value);
  }
}

void BaseClient::sendField(String const& value, boolean requires_hmac) {
  write(' ');
  print(value);

  if (requires_hmac) {
    Sha256.write(' ');
    Sha256.print(value);
  }
}

void sendField(const uint8_t* buffer, size_t buffer_length, boolean requires_hmac) {

  write(' ');
  if (requires_hmac) {
    Sha256.write(' ');
  }
  
  // Send binary blob as hex
  for(uint8_t i = 0; i < buffer_length; i++) {
    print(buffer[i], HEX);
    if (requires_hmac) {
      Sha256.print(buffer[i], HEX);
    }
  }

}

/** Finalizes the message. If 'requires_hmac' is true, then the HMAC
 * result is sent. When the HMAC is sent, the session key is advanced
 * such that it is one value ahead of the server. Finally, regardless of
 * the value of 'requires_hmac', a newline (the end-of-message delimeter)
 * is sent.
 * 
 * Note: using HMAC assumes Sha256 has already been initialized and fed
 *       with the data. We simply call resultHmac() here.
 */
void BaseClient::finalizeMessage(boolean requires_hmac) {
  if (requires_hmac) {
    // Initialize space for result HMAC (hex string)
    char hmac[2*DOORBOT_CLIENT_KEY_SIZE+1];
    memset(hmac, 0, 2*DOORBOT_CLIENT_KEY_SIZE+1);
    
    // Copy HMAC result to string buffer as hex
    bytesToHex(hmac, Sha256.resultHmac(), DOORBOT_CLIENT_KEY_SIZE);

    // Send the HMAC portion of the message: ' HMAC'
    write(' ');
    print(hmac);

    // Advance the session key
    advanceSessionKey();
  }

  // Finish the message off with a delimiter
  write('\n');
}

void BaseClient::shutdown() {
  // Flush outgoing buffer
  flush();
  
  // Disconnect
  stop();
  
  // Flush receive buffer
  while(available()) {
    read();
  }
}
