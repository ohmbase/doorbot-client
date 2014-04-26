#ifndef DOORBOT_CLIENT_H
#define DOORBOT_CLIENT_H

#include <Arduino.h>
#include <sha256.h>

#include "BaseClient.h"

#define DOORBOT_TIMEOUT              4000
#define DOORBOT_KEY_SIZE               32
#define DOORBOT_DB_STATE_SIZE          64
#define DOORBOT_BUFFER_SIZE           128
#define DOORBOT_MSG_HMAC_SIZE          64
#define DOORBOT_AUTH_TOKEN_LENGTH      32
#define DOORBOT_AUTH_RESPONSE_LENGTH    2

class DoorbotClient : public BaseClient { 
  private:
    char _buffer[DOORBOT_BUFFER_SIZE];
    int readField(char* buffer, int max_length, boolean use_hmac = true, boolean last_hmac_field = false);
    int readFinalField(char* buffer, int max_length);
    
  public:
    DoorbotClient(String const& id, String const& key);
   
    /*! Authenticates against the server for provided auth_token. Uses HMAC.
        `auth_token' is assumed to be of length `DOORBOT_AUTH_TOKEN_LENGTH'. */
    boolean authenticateRFID(const uint8_t *auth_token);
};

#endif
