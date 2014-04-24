#ifndef ID_STORE_H
#define ID_STORE_H

#include <Arduino.h>
#include <String.h>
#include <EEPROM.h>

#define ID_STORE_RFID_LENGTH 10
#define ID_STORE_TOTAL_SLOTS 100
#define ID_STORE_BASE_ADDRESS 0

class IDStoreClass
{
  private:
    int _slotBaseAddress(const int slot); //!< Find the next empty location
    int _findNextEmpty(const int from = 0); //!< Find the next empty location
    int _nextSlot; //!< Next empty location
  public:
    IDStoreClass(); //!< Constructor
    ~IDStoreClass(); //!< Destructor
    int find(String const &token); //!< Given a token, it returns the slot number, -1 otherwise.
    int remove(const int slot); //!< Deletes the RFID in the given slot.
    int save(String const &token); //!< Saves a token to store
    String get(const int slot); //!< Returns the token at the given slot, empty string if empty slot.
    boolean isEmpty(const int slot); //!< Returns whether or not a slot is empty
#ifdef DEBUG
    void clearAll();
#endif
};

extern IDStoreClass IDStore; //!< Global instance

#endif
