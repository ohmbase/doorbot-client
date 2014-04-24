# doorbot 2.0 - Client code

## Overview

_doorbot_ is an access control and usage logging 

## Configuration

_doorbot_ client configuration is found in `config.h'. Yes, it's a C/C++ header file. Here you will find client parameters as well as some runtime options.

`SERVER_IP` - the IP address of the _doorbot_ server
`SERVER_PORT` - the port of the _doorbot_ server
`CLIENT_ID` - should match the client ID of the configuration on the server (identifies the client to the server)
`CLIENT_KEY` - a pre-shared secret that only the server and the client should know (i.e. do _not_ push this value to git)
`CLIENT_MAC` - the MAC address of the ethernet shield (should be written on or included with the device)
`CLIENT_IP` - the client's IP address
`SUBNET_MASK` - the client's subnet mask
`GATEWAY_IP` - the client's gateway IP

### Example `config.h`

    #ifndef CONFIG_H
    #define CONFIG_H

    /// Client configuration //////////////////////////////////////////////
    #define SERVER_IP    IPAddress(192,168,0,3)
    #define SERVER_PORT  60405
    
    #define CLIENT_ID   "client_id"
    #define CLIENT_KEY  "preshared_secret"
    #define CLIENT_MAC  {0xDE, 0xAD, 0xBE, 0xEF, 0xBA, 0x5E}

    #define CLIENT_IP   IPAddress(192,168,0,2)
    #define SUBNET_MASK {255, 255, 255, 0}
    #define GATEWAY_IP  IPAddress(192,168,0,1)

    // Runtime options ////////////////////////////////////////////////////
    #define DOOR_OPEN_DELAY     3000
    #define CONNECT_RETRY_DELAY 2000
    #define LOOP_DELAY          1000
    #define CONNECT_RETRY_COUNT    3
    //#define CLEAR_CACHE_ON_BOOT
    //#define USE_DHCP

    #endif

## Uploading configuration

Since the client configuration defines constants used at compile-time, the configuration needs to be compiled with the client software and uploaded to the Arduino.

1. Open `client.ino` in the Arduino software
2. Connect the Arduino via USB
3. (Ensure everything is hunky dory)
4. Click the "Upload" button.

## Offline usage

_doorbot_ has a simple caching system that remembers past authentication events. Currently, a cache entry is only added/modified/removed after a valid authentication response from the server. In the event of the server being unreachable, a token (e.g. an RFID tag) will allow access as long as the last attempt to use the token returned a valid and positive authentication response from the server. Alternatively, the cache can be cleared every time the device is (re)booted by adding `#define CLEAR_CACHE_ON_BOOT` to `config.h`.

## See also

* [doorbot-server](https://github.com/ohmbase/doorbot-server) - _doorbot_ server
* [doorbot-board](https://github.com/ohmbase/doorbot-board) - _doorbot_ hardware

## License

    Copyright (c) 2011-2012, Ohm Base Hackerspace
    All rights reserved.

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions
    are met:
        * Redistributions of source code must retain the above
          copyright notice, this list of conditions and the following
          disclaimer.
        * Redistributions in binary form must reproduce the above
          copyright notice, this list of conditions and the following
          disclaimer in the documentation and/or other materials
          provided with the distribution.
        * Neither the name of the Ohm Base Hackerspace nor the names of
          its contributors may be used to endorse or promote products
          derived from this software without specific prior written
          permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
    "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
    LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
    FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL OHM BASE
    HACKERSPACE BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
    SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
    LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF
    USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
    ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
    OR TORT(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
    OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
    SUCH DAMAGE.

## Authors & Acknowledgements
* Kris Scott ()
* Jon Kivinen ()