#ifndef DOORBOT_KEY_H
#define DOORBOT_KEY_H

#include <Arduino.h>
#include <sha256.h>

#define DOORBOT_CLIENT_KEY_SIZE 32 // 32 bytes = 64 hex characters

typedef struct Key { byte x[DOORBOT_CLIENT_KEY_SIZE]; } Key; // TODO? typedef byte x[DOORBOT_CLIENT_KEY_SIZE] Key

void stringToKey(String const& s, Key &k); //!< helper to convert a string to a key
//String bytesToHex(const byte* bytes, int length); //!< helper to convert blob of data to hex (String)
void bytesToHex(char* hexString, const byte* buf, int length); //!< helper to convert blob of data to hex (char*)

#endif
