/**
 * DOORBOT
 * (c)2011-2013 Ohm Base Hackerspace (ohmbase.org)
 */

#include <Arduino.h>
#include <SPI.h>
#include <String.h>
#include <EEPROM.h>
#include <SoftwareSerial.h>
#include <Ethernet.h>
#include <sha256.h>

#include "config.h"
#include "pin_config.h"
#include "fail_codes.h"

#include "ParallaxRFID.h"
#include "AuthCache.h"
#include "Key.h"

#include "BaseClient.h"
#include "DoorbotClient.h"

DoorbotClient client(CLIENT_ID, CLIENT_KEY);
byte mac[] = CLIENT_MAC;
boolean ethernet_initialized = false;
  
#ifndef USE_DHCP 
IPAddress gateway_ip = GATEWAY_IP;
IPAddress client_ip = CLIENT_IP;
byte subnet[] = SUBNET_MASK;
#endif // ifndef USE_DHCP

void setup() {
  // Initialize hardware serial
  Serial.begin(9600);
  
  // Initialize the RFID reader
  ParallaxRFID.begin();

  // Set up door pin
  pinMode(DOOR_PIN, OUTPUT);
  
  #ifdef CLEAR_CACHE_ON_BOOT
  AuthCache.clear();
  #end
}

inline void fail(int code) {
  //Serial.print('!');
  //Serial.println(code, HEX); 
}

void loop() {
  /// Initialize ethernet (if not intialized) /////////////////////////
  if (!ethernet_initialized) {
    #ifdef USE_DHCP
    // .. using DHCP
    ethernet_initialized = !(Ethernet.begin(mac) == 0);
    #else
    // .. using fixed settings
    Ethernet.begin(mac, client_ip, gateway_ip, gateway_ip, subnet);
    ethernet_initialized = true;
    #endif // ifdef USE_DHCP
  
    if (!ethernet_initialized) {
      fail(FAIL_ETHERNET_BEGIN);
    }
  }
  
  // Connect to server ////////////////////////////////////////////////
  
  // Force clean-up if there is data to be read (FIXME)
  if (ethernet_initialized && client.available()) {
    client.shutdown();
  }
  
  if (ethernet_initialized && !client.connected()) {
    // Connect to server (trying CONNECT_RETRY_COUNT times)
    int attempt = 0;
    while(attempt++ < CONNECT_RETRY_COUNT) {
      if (!client.connect(SERVER_IP, SERVER_PORT)) {
        // failed to connect...
        fail(FAIL_ETHERNET_CONNECT);
        client.shutdown();
        delay(CONNECT_RETRY_DELAY);
      else {
        // connected!
        break;
      }
    }
    
    // Perform handshake
    if (!client.handshake()) {
      fail(FAIL_ETHERNET_HANDSHAKE);
      client.shutdown();
    }
  }
  
  // Update cache /////////////////////////////////////////////////////
  if (ethernet_initialized && client.connected()) {
    client.refreshCache();
  }

  // Wait for RFID to be scanned (hoping TCP connection stays alive)
  String rfid_token = ParallaxRFID.read();
  if (rfid_token.length() == PX_RFID_LENGTH) {
    // Hash RFID value to get an "auth" token
    uint8_t auth_token[DOORBOT_AUTH_TOKEN_LENGTH] = {0};
    Sha256.init();
    Sha256.print(rfid_token);
    memcpy(auth_token, Sha256.result(), DOORBOT_AUTH_TOKEN_LENGTH);
    
    // Check authentication token against server
    if (client.authenticateRFID(auth_token)) {
      // Token request succeeded, cache token
      AuthCache.save(auth_token);
      
      // Unlock the door
      digitalWrite(DOOR_PIN, HIGH);
    
      // Wait an acceptible time for the door to be opened
      delay(DOOR_OPEN_DELAY);
    
      // Lock the door
      digitalWrite(DOOR_PIN, LOW);
    }
    else {
      // Token request failed, flush cache of token
      AuthCache.remove(token);
    }
  }
  
  // Clean up the client if there was a problem...
  if (ethernet_initialized && (client.available() || !client.connected())) {
    client.shutdown();
  }
  
  delay(LOOP_DELAY); // Limit loop time
}

