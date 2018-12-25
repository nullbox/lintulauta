#ifndef SIM900_RESPONSE_h
#define SIM900_RESPONSE_h

#include <avr/pgmspace.h>

// ESP 8266 replies
static const char ESP_OK[]	         PROGMEM = "OK";
static const char ESP_ERROR[]		     PROGMEM = "ERROR";
static const char ESP_READY[]        PROGMEM = "ready";
static const char ESP_CONNECTED[]    PROGMEM = "WIFI CONNECTED";
static const char ESP_GOT_IP[]       PROGMEM = "WIFI GOT IP";
static const char ESP_STATUS[]       PROGMEM = "STATUS:";
static const char ESP_CON_OK[]	     PROGMEM = "CONNECT";
static const char ESP_CON_ALREADY[]	 PROGMEM = "ALREADY	CONNECTED";
static const char ESP_SEND_OK[]		   PROGMEM = "SEND OK";
static const char ESP_SEND_FAIL[]		 PROGMEM = "SEND FAIL";
static const char ESP_START_SEND[]	 PROGMEM = ">";
static const char ESP_CLOSED[]       PROGMEM = "CLOSED";

// TODO BELOW
static const char ESP_SHUT[]         PROGMEM = "SHUT OK";
static const char ESP_DATA_END[]     PROGMEM = "\u001A";
static const char ESP_EMPTY[]        PROGMEM = "";

enum class Response: uint8_t {
  ACT,
  NACT,
  ERROR
};

#endif
