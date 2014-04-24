#ifndef AUTH_CACHE_H
#define AUTH_CACHE_H

#include <Arduino.h>
#include <String.h>
#include <EEPROM.h>

// Total number of slots used for
#define AUTH_CACHE_TOTAL_SLOTS 100

// Base address in EEPROM that cache begins
#define AUTH_CACHE_BASE_ADDRESS 0

// Cache entry field lengths
#define AUTH_CACHE_HEADER_LENGTH 1
#define AUTH_CACHE_TOKEN_LENGTH DOORBOT_AUTH_TOKEN_LENGTH
#define AUTH_CACHE_RESERVED_LENGTH 7
// Total length of cache entry
#define AUTH_CACHE_ENTRY_LENGTH (AUTH_CACHE_HEADER_LENGTH + AUTH_CACHE_TOKEN_LENGTH + AUTH_CACHE_RESERVED_LENGTH)

// Header values for slot types
#define AUTH_CACHE_HEADER_EMPTY 0xFF
#define AUTH_CACHE_HEADER_RFID  0x01

// Shorthand for base EEPROM address for cache entry given slot no.
#define AUTH_CACHE_SLOT_BASE_ADDR(SLOT)     (AUTH_CACHE_BASE_ADDRESS + (SLOT)*(AUTH_CACHE_ENTRY_LENGTH))
// Shorthands for EEPROM addresses for fields in cache entry given slot no.
#define AUTH_CACHE_SLOT_HEADER_ADDR(SLOT)   (AUTH_CACHE_SLOT_BASE_ADDR((SLOT))) 
#define AUTH_CACHE_SLOT_TOKEN_ADDR(SLOT)    (AUTH_CACHE_SLOT_HEADER_ADDR((SLOT)) + AUTH_CACHE_HEADER_LENGTH)
#define AUTH_CACHE_SLOT_RESERVED_ADDR(SLOT) (AUTH_CACHE_SLOT_TOKEN_ADDR((SLOT)) + AUTH_CACHE_TOKEN_LENGTH)

// Value returned for undefined or unknown slot
#define AUTH_CACHE_NO_SLOT -1

/*! AuthCache allows for the door to work without an internet
    connection. AuthCache stores authentication tokens.
    Each entry in the cache is 40 bytes and takes the form:
    
      /0        1/2                   32/33         39/
      +----------+----------------------+------------+ 
      | <header> |  <auth token hash>   | (reserved) |
      +----------+----------------------+------------+
    
    <header> is a single byte which gives information about
    what is in the cache entry. A header value of 0xFF
    (i.e. AUTH_CACHE_HEADER_EMPTY_SLOT) denotes a free/empty
    entry.
    
    <auth token hash> is a 32-byte identifier for the
    authentication information, e.g. the result of a
    SHA256 hash.
    
    (reserved) is a 7-byte space reserved for future
    features.
    
    This cache is checked when the remote server is not
    available. Cache entires are refreshed when either
    1) the remote server is used to validate the auth
    token and a valid response comes back, or 2) the
    client consults the server and expires the entry
    (FUTURE). If the server is not available, any entry
    in this cache is assumed valid. Currently, this
    cache is only useful for static tokens such as
    basic RFID. For moving tokens, such as HMAC-OTP,
    additional functionality would need to be added
    to validate offline (if possible).
*/
class AuthCacheClass
{
  private:
    int _findFirstEmptySlot(int from = 0); //!< Find the next empty location
    
  public:
    AuthCacheClass(); //!< Constructor, takes the NVRAM base address
    ~AuthCacheClass(); //!< Destructor
    int findByToken(const uint8_t *token); //!< Given a token, it returns the slot number or AUTH_CACHE_NO_SLOT if not found.
    int remove(const int slot); //!< Deletes the RFID in the given slot, returning the slot deleted or AUTH_CACHE_NO_SLOT if not found.
    int save(const uint8_t *token, int slot = AUTH_CACHE_NO_SLOT); //!< Saves a token to store, returning the slot or AUTH_CACHE_NO_SLOT if no available slots.
    boolean get(const int slot, uint8_t *token); //!< Returns whether or not the slot was empty and the token if found.
    boolean isEmpty(const int slot); //!< Returns whether or not a slot is empty
    void clear(); //!< Clears all slots
};

extern AuthCacheClass AuthCache; //!< Global instance

#endif
