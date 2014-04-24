#ifndef FAIL_CODES_H
#define FAIL_CODES_H

#define FAIL_UNSPEC               0 // Unspecified failure
#define FAIL_ETHERNET_BEGIN       1 // Could not initialize ethernet (DHCP)
#define FAIL_ETHERNET_CONNECT     2 // Could not connect to server
#define FAIL_ETHERNET_HANDSHAKE   3 // Server handshake failure
#define FAIL_SLOT_NOT_FOUND      10 // Auth token not found in database

#endif
