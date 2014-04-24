#ifndef ADMIN_CONSOLE_H
#define ADMIN_CONSOLE_H

#define ADMIN_CONSOLE_BAUD 9600

#include <String.h>
#include "IDStore.h"
#include "ParallaxRFID.h"

class AdminConsoleClass
{
  private:
    // Print
    void _welcome();
    void _prompt();
    String _readInput(unsigned long timeout_ms = 60000);
    void _newline();
    void _clearScreen();
    void _help();
  
    // Commands
    void _doAdd(); //!< Add ID into system
    void _doRemove(); //!< Remove ID from system
    void _doList(); //<! List IDs in system
    
  protected:
    boolean _authenticate(); //!< Perform authentication, returns true if the user has authenticated
    boolean _handleCommand(String const &command); //!< Dispatch a command
    
  public:
    AdminConsoleClass() {};
    ~AdminConsoleClass() {};
    boolean isReady();
    void start(); //!< Start a loop, reading commands until the exit command
};

extern AdminConsoleClass AdminConsole;

#endif
