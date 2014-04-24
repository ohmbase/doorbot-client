#include "AuthCache.h"

AuthCacheClass::AuthCacheClass() {
}

AuthCacheClass::~AuthCacheClass() {
  
}

int AuthCacheClass::_findFirstEmptySlot(int slot) {
  slot = slot < 0 ? 0 : slot;

  while(slot < AUTH_CACHE_TOTAL_SLOTS) {
    if (isEmpty(slot)) {
      return slot;
    }
    slot++;
  }
  
  return AUTH_CACHE_NO_SLOT;
}

boolean AuthCacheClass::isEmpty(const int slot) {
  return AUTH_CACHE_HEADER_EMPTY_SLOT == EEPROM.read(AUTH_CACHE_SLOT_HEADER_ADDR(slot));
}

int AuthCacheClass::findByToken(uint8_t const* token) {
  boolean found = false;
  
  for (int slot = 0; !found && slot < AUTH_CACHE_TOTAL_SLOTS; slot++) {
    // Assume the token is found
    found = true;
    
    // Disprove "found = true" by comparing token until no longer matches
    for (uint8_t i = 0; i < AUTH_CACHE_TOKEN_LENGTH; i++) {
      if (EEPROM.read(AUTH_CACHE_SLOT_BASE_ADDR(slot) + i) != token.charAt(i)) {
        // Current slot does not match
        found = false;
        break;
      }
    }
    
    // Match found!
    if (found == true) {
      return slot;
    }
  }
  
  return AUTH_CACHE_NO_SLOT; // No match
}

int AuthCacheClass::remove(const int slot) {
  EEPROM.write(AUTH_CACHE_SLOT_HEADER_ADDR(slot), AUTH_CACHE_HEADER_EMPTY_SLOT);
  return slot;
}

int AuthCacheClass::save(const uint8_t *token) { 
  /// Find appropriate cache slot (new or exisiting) //////////////////
  int slot = findByToken(token);
  
  if (slot == AUTH_CACHE_NO_SLOT) {
    // Token does not exist, save in next available slot
    slot = _findFirstEmptySlot();
  }
  
  /// Update cache entry ////////////////////////////////////////////
  if (slot == AUTH_CACHE_NO_SLOT) {  
    // No available slot, cannot cache. Sorry!
  }
  else{
    
    // Update header
    EEPROM.write(AUTH_CACHE_SLOT_HEADER_ADDR(slot), AUTH_CACHE_HEADER_RFID_SLOT);
    
    // Update token
    for (int i = 0; i < AUTH_CACHE_TOKEN_LENGTH; i++) {
      EEPROM.write(AUTH_CACHE_SLOT_TOKEN_ADDR(slot) + i, token[i]);
    }
      
    // Update "reserved"
    // (just kidding, nothing to do)
  }
  
  return slot;
}

boolean AuthCacheClass::get(const int slot, uint8_t *token)
{
  if (isEmpty(slot)) {
    return false;
  }
  
  for (int i = 0; i < AUTH_CACHE_TOKEN_LENGTH; i++) {
    token[i] = EEPROM.read(AUTH_CACHE_SLOT_TOKEN_ADDR(slot) + i);
  }
  
  return true;
}

void AuthCacheClass::clear() {
  for (int i = 0; i < AUTH_CACHE_TOTAL_SLOTS; i++) {
    remove(i);
  }
}

AuthCacheClass AuthCache; // Global instance
