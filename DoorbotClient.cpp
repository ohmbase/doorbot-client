#include "DoorbotClient.h"

DoorbotClient::DoorbotClient(String const& id, String const& key)
  : BaseClient(id, key)
{
  setTimeout(DOORBOT_TIMEOUT);
}

int DoorbotClient::readField(char* buffer, int max_length, boolean use_hmac, boolean last_hmac_field) {
  int bytes_read = readBytesUntil(buffer, max_length, ' ');
  buffer[bytes_read] = '\0';
  
  if (use_hmac) {
    Sha256.print(buffer);
    if (!last_hmac_field) {
      Sha256.print(' ');
    }
  }
  
  return bytes_read;
}

int DoorbotClient::readFinalField(char* buffer, int max_length) {
  // Last field is either an HMAC or we're not using HMAC.
  int bytes_read = readBytesUntil(buffer, max_length, '\n');
  buffer[bytes_read] = '\0';
  
  return bytes_read;
}

boolean DoorbotClient::authenticateRFID(const uint8_t* token_hash) {
  String hmac_msg; hmac_msg.reserve(DOORBOT_MSG_HMAC_SIZE+1);
  String yes("OKAY");
  boolean response_valid = true; 
  boolean explicit_yes = false;
  
  // Send resquest (auth? rfid <token_hash> <hmac>)
  initializeMessage("auth?", true);
  sendField("rfid", true);
  sendField(token_hex, DOORBOT_TOKEN_LENGTH, true); // sends as hex using built-in print(..., HEX) for each byte
  finalizeMessage(true);
  
  // Get response from server
  Sha256.initHmac(sessionKey().x, DOORBOT_KEY_SIZE); // initialize HMAC

  // .. read response, "OKAY" or "FAIL"
  readField(_buffer, DOORBOT_AUTH_RESPONSE_SIZE+1, true, true);
  explicit_yes = yes.equalsIgnoreCase(_buffer);

  // .. read HMAC
  readFinalField(_buffer, DOORBOT_MSG_HMAC_SIZE+1);
  hmac_msg = _buffer; // save a copy of the HMAC to test against
  
  // Calculate HMAC of the response we received
  bytesToHex(_buffer, Sha256.resultHmac(), DOORBOT_KEY_SIZE);
  
  // Advance session key
  advanceSessionKey();
  
  return explicit_yes && hmac_msg.equalsIgnoreCase(_buffer);
}
