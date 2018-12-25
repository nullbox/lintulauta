#ifndef SIM900_h
#define SIM900_h

#include <Arduino.h>
#include <Response.h>

static const uint32_t DEFAULT_TIMEOUT = 5000;
static const uint32_t CONNECT_TIMEOUT = 10000;
static const uint32_t LONG_TIMEOUT = 20000;

#define DEBUG_ON

#ifdef DEBUG_ON
  #include <SoftwareSerial.h>
  #define DEBUG_BEGIN(x)	      Serial.begin(x)
  #define DEBUG_PRINTLN(x)	    Serial.println(x)
  #define DEBUG_PRINT(x)		    Serial.print(x)
  #define DEBUG_WRITE(x)		    Serial.write(x)
  #define DEBUG_PRINTP(x,y)     Serial.print(x, y)
  #define DEBUG_FLUSH()         Serial.flush()
  #define DEBUG_AVAILABLE()     Serial.available()
  #define DEBUG_READ()          Serial.read()

  #define ESP8266_BEGIN(x)      SSerial->begin(x)
  #define ESP8266_PRINTLN(x)	  SSerial->println(x)
  #define ESP8266_PRINT(x)      SSerial->print(x)
  #define ESP8266_WRITE(x)      SSerial->write(x)
  #define ESP8266_PRINTP(x,y)   SSerial->print(x, y)
  #define ESP8266_FLUSH()       SSerial->flush()
  #define ESP8266_AVAILABLE()   SSerial->available()
  #define ESP8266_READ()        SSerial->read()
#endif

class ESP8266 {
  private:
    #ifdef DEBUG_ON
      SoftwareSerial* SSerial;
    #endif

  public:
    #ifdef DEBUG_ON
      ESP8266(SoftwareSerial* SSerial);
    #endif

    Response waitFor(PGM_P act, PGM_P nact, uint32_t timeout = DEFAULT_TIMEOUT,
      uint8_t *cBuf = NULL, size_t cSize = 0, bool flush = true);

    bool print(PGM_P str);
    bool print(uint8_t *str);
    bool print(char *str);
    bool print(int num);

    int status(uint8_t retries);
    bool enable(uint8_t retries, PGM_P AP, PGM_P PASSWORD, bool AT = false);
    bool start(uint8_t retries, PGM_P domain, PGM_P port, uint8_t length);
    bool restart(uint8_t retries);

};

#endif
