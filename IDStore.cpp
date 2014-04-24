#include "String.h"
#include "IDStore.h"

IDStoreClass::IDStoreClass()
{
  _nextSlot = _findNextEmpty();
}

IDStoreClass::~IDStoreClass()
{
  
}

int IDStoreClass::_slotBaseAddress(const int slot)
{
  if (slot <= 0)
    return 0;
  else
    return ID_STORE_BASE_ADDRESS + slot * ID_STORE_RFID_LENGTH;
}

int IDStoreClass::_findNextEmpty(const int from)
{
  for (int slot = from; slot < ID_STORE_TOTAL_SLOTS; slot++)
  {
    if (isEmpty(slot))
      return slot;
  }
      
  return -1;
}

boolean IDStoreClass::isEmpty(const int slot)
{
  int c = EEPROM.read(_slotBaseAddress(slot));
  return c == 255;
}

int IDStoreClass::find(String const &token)
{
  if (token.length() != ID_STORE_RFID_LENGTH)
    return -1;
    
  boolean matched = true;
  
  for (int slot = 0; slot < ID_STORE_TOTAL_SLOTS; slot++)
  {
    int baseAddress = _slotBaseAddress(slot);
    matched = true;
    
    for (int i = 0; i < ID_STORE_RFID_LENGTH; i++)
    {
      if (EEPROM.read(baseAddress + i) != token.charAt(i)) // No match
      {
        matched = false;
        break;
      }
    }
    
    if (matched == true)
      return slot; // Matched
  }
  
  return -1; // No match
}

int IDStoreClass::remove(const int slot)
{
  int baseAddress = _slotBaseAddress(slot);
  
  EEPROM.write(baseAddress, 0xFF);
  
  // Use this slot if necessary
  if (slot < _nextSlot)
  {
    _nextSlot = slot;
  }
  
  return slot;
}

int IDStoreClass::save(String const &token)
{
  if (token.length() != ID_STORE_RFID_LENGTH)
    return -1;
  
  // If token already exists, return its slot
  int slot = find(token);
  
  if (slot >= 0)
    return slot;
  
  // Token does not exist, save in next available slot
  slot = _nextSlot;
  int baseAddress = _slotBaseAddress(slot);
  
  for (int i = 0; i < ID_STORE_RFID_LENGTH; i++)
  {
    EEPROM.write(baseAddress + i, token.charAt(i));
  }
  
  // Get next slot
  _nextSlot = _findNextEmpty(slot);
  
  return slot;
}

String IDStoreClass::get(const int slot)
{
  if (isEmpty(slot))
    return "";
  
  char token[ID_STORE_RFID_LENGTH+1];
  
  int baseAddress = _slotBaseAddress(slot);
  
  for (int i = 0; i < ID_STORE_RFID_LENGTH; i++)
  {
    token[i] = EEPROM.read(baseAddress + i);
  }
  
  token[ID_STORE_RFID_LENGTH] = '\0';
    
  return String(token);
}

#ifdef DEBUG
void IDStoreClass::clearAll()
{
  for (int i = 0; i < ID_STORE_TOTAL_SLOTS; i++)
  {
    remove(i);
  }
}
#endif

IDStoreClass IDStore; // Global instance
