#include "AdminConsole.h"

void AdminConsoleClass::_welcome()
{
  Serial.print("Doorlock Admin Console");
  _newline();
}

void AdminConsoleClass::_prompt()
{
  Serial.print("> ");
}

String AdminConsoleClass::_readInput(unsigned long timeout_ms)
{
  unsigned long startTime_ms = millis();
  
  String command = "";
  char c;
  
  // Read in the command
  while(1)
  {
    if ((millis() - startTime_ms) >= timeout_ms)
    {
      // Treat timeout as an "exit" command
      return "exit";
    }
    
    while (Serial.available() > 0) // TODO maximum length
    {
      c = Serial.read();
      
      if (c == '\r' || c == '\n') // Read until first newline
      {
        _newline();
        return command;
      }
      else
      {
        Serial.print(c);
        command.concat(c);
      }
    }
  }
}
    
void AdminConsoleClass::_newline()
{
  Serial.print("\r\n");
}

void AdminConsoleClass::_clearScreen()
{
  for (int x=0; x<24; x++)
  {
    _newline();
  }
}

void AdminConsoleClass::_help()
{
  Serial.print("Commands:");
  _newline();
  Serial.print("  ls, add, rm");
  _newline();
}

void AdminConsoleClass::_doAdd()
{
  Serial.print("Scan RFID...");
  _newline();
    
  String rfidKey = ParallaxRFID.read();
 
  if (rfidKey == "")
  {
    Serial.print("Failed to read RFID...");
    _newline();
  }
  else  //Read from the RFID successfully
  {
    int savedTo = IDStore.save(rfidKey);
    
    if (savedTo >= 0)
    {
      Serial.print(rfidKey);
      Serial.print(" saved to slot ");
      Serial.print(savedTo);
      _newline();
    }
  }
}

void AdminConsoleClass::_doRemove()
{
  Serial.print("Remove which?");
  _newline();
  
  //TODO get Jon to fix this nasty double conversion
  
  String slotString = _readInput();
  char slotCharArray[3];
  slotString.toCharArray(slotCharArray,3);
  int const slot = atoi(slotCharArray);
  Serial.print("Are You Sure? [y/n] ");
  if (_readInput() == "y")
  {
    if (!IDStore.isEmpty(slot))
    {
      IDStore.remove(slot);
    }
    else
    {
      Serial.print("Slot is empty.");
      return;
    }
  }
  else
  {
    Serial.print("Aborted");
    return;
  }
}

void AdminConsoleClass::_doList()
{
  int total = 0;
    
  for (int slot = 0; slot < ID_STORE_TOTAL_SLOTS; slot++)
  {
    if (!IDStore.isEmpty(slot))
    {
      Serial.print(slot);
      Serial.print(" ");
      Serial.print(IDStore.get(slot));
      _newline();
      total++;
    }
  }
  
  // Print total
  Serial.print(total);
  Serial.print(" entries found.");
  _newline();
}

boolean AdminConsoleClass::_handleCommand(String const &command)
{
  if (command == "ls")
  {
    _doList();
    return true;
  }
  else if (command == "rm")
  {
    _doRemove();
    return true;
  }
  else if (command == "add")
  {
    _doAdd();
    return true;
  }
  else if (command == "exit")
  {
    _clearScreen();
    return false;
  }
  else
  {
    _help();
    return true;
  }
}

void AdminConsoleClass::start()
{
  String command = _readInput();
  
  _clearScreen();
  
  _welcome(); // Show welcome
  _prompt(); // Show prompt
  
  // Read in commands until the command is to quit the prompt
  for (;;)
  {
    command = _readInput();

    Serial.print("command -> ");
    Serial.print(command);
    _newline();
    
    // Empty command
    if (command == "")
      continue;
    
    // Handle command
    if (!_handleCommand(command))
      return;
    
    // Show prompt for next command
    _prompt();
  }
}

AdminConsoleClass AdminConsole; // Global instance
