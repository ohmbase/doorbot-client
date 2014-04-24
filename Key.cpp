#include "Key.h"

void stringToKey(String const& s, Key &k) {
  Sha256.init();
  Sha256.print(s);
  byte* hash_result = Sha256.result();
  for (uint8_t i = 0; i < DOORBOT_CLIENT_KEY_SIZE; i++) {
    k.x[i] = hash_result[i];
  }
}

/*
String bytesToHex(const byte* buf, int length) {
  const char* alphabet = "0123456789abcdef";

  if (length <= 0) return "";
  String hexString = "";

  for (int i = 0; i < length; i++) {
      hexString += alphabet[buf[i]>>4];
      hexString += alphabet[buf[i]&0xf];
  }
  
  return hexString;
}*/

void bytesToHex(char* hex_string, const byte* buf, int length) {
  if (length <= 0)
    return;
    
  const char const* alphabet = "0123456789abcdef";
  hex_string[0] = '\0';
  
  for (int i = 0; i < length; i++) {
    hex_string[(i<<1)]   = alphabet[buf[i]>>4];
    hex_string[(i<<1)+1] = alphabet[buf[i]&0xf];
  }
  
  hex_string[length<<1] = '\0';
}
