#include <Arduino.h>
#include <SoftwareSerial.h>
#include <ESP8266.h>

#define BAUD_RATE 9600
#define V_LENGHT 10 // For prod 255 - max value 255
#define SOFT_SERIAL_RX 9
#define SOFT_SERIAL_TX 8

static const char DOMAIN[]    PROGMEM = "us-central1-linnunruokapuntari.cloudfunctions.net";
static const char PORT[]      PROGMEM = "80";
static const char AP[]        PROGMEM = "Lintumaki";
static const char PASSWORD[]  PROGMEM = "J44kk0l4";
static const char DID[]       PROGMEM = "c7d41282";

static const uint64_t WAIT_MEASURE = 1000; // 1 second
static uint64_t TIMESTAMP_MEASURE = 0;

static uint8_t VALUES[V_LENGHT];
static uint8_t V_INDEX = 0;

SoftwareSerial SoftSerial(SOFT_SERIAL_RX, SOFT_SERIAL_TX);
static ESP8266 esp8266(&SoftSerial);

void setup() {
  Serial.begin(BAUD_RATE);
  SoftSerial.begin(BAUD_RATE);
  pinMode(LED_BUILTIN, OUTPUT);
}

void loop() {
  if ( SoftSerial.available() )   {  Serial.write( SoftSerial.read() );  }
	if ( Serial.available() )       {  SoftSerial.write( Serial.read() );  }

  if (millis() > TIMESTAMP_MEASURE + WAIT_MEASURE) {
    TIMESTAMP_MEASURE = millis();

    VALUES[V_INDEX++] = 100; // TODO Do measurement here, type uint8_t
    if (V_INDEX >= V_LENGHT) {
      V_INDEX = 0;

      if(esp8266.enable(3, AP, PASSWORD) && esp8266.start(3, DOMAIN, PORT, 154 + 27 + (V_LENGHT * 4) - 1)) {
        // Last parameter, number of chars to send must be correct. \r\n counting as one
        uint8_t cl = 27 + (V_LENGHT * 4) - 1;

        // Building POST Request
        // headers lenght with cl 29 is 154
        esp8266.print(PSTR("POST /measure HTTP/1.1\r\n"));
        esp8266.print(PSTR("Host: "));
        esp8266.print(DOMAIN);
        esp8266.print(PSTR("\r\n"));
        esp8266.print(PSTR("Content-Type: application/json\r\n"));
        esp8266.print(PSTR("Content-Length: "));
        esp8266.print(cl);
        esp8266.print(PSTR("\r\nConnection: close\r\n\r\n"));

        // Body 27 without values
        esp8266.print(PSTR("{\"id\":\""));
        esp8266.print(DID);
        esp8266.print(PSTR("\",\"data\":["));
        for (uint8_t i = 0; i < V_LENGHT; i++) {
          esp8266.print(VALUES[i]);
          if (i < V_LENGHT - 1) esp8266.print(PSTR(","));
        }
        esp8266.print(PSTR("]}"));

        if (esp8266.waitFor(ESP_SEND_OK, ESP_SEND_FAIL, LONG_TIMEOUT) == Response::ACT) {
          Serial.println("\r\n\r\nDONE"); // TODO Remove in production
        }
      }
    }
  }
}
