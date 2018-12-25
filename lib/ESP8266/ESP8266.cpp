#include <ESP8266.h>

#ifdef DEBUG_ON
  ESP8266::ESP8266(SoftwareSerial* s) : SSerial(s) {}
#endif

bool ESP8266::enable(uint8_t retries, PGM_P AP, PGM_P PASSWORD, bool AT) {
  if (retries >= 1) {
    if (!AT) { // module is responsive
      print(PSTR("AT\r\n"));
      switch (waitFor(ESP_OK, ESP_ERROR)) {
        case Response::ACT:
          return enable(retries, AP, PASSWORD, true);
        default:
          return enable(--retries, AP, PASSWORD);
      }
    }

    switch (status(3)) {
      case 2: // 2: The ESP8266 Station is connected to an AP and its IP is obtained.
        return true;
      case 3: // 3: The ESP8266 Station has created a TCP or UDP transmission.
        print(PSTR("AT+CIPCLOSE\r\n"));
        if (waitFor(ESP_CLOSED, ESP_OK, CONNECT_TIMEOUT) == Response::ACT) {
          return enable(retries, AP, PASSWORD, true);
        }
        return enable(--retries, AP, PASSWORD, true);
      case 4: // 4: The TCP or UDP transmission of ESP8266 Station is disconnected.
        return true;
      break;
      case 5: // 5: The ESP8266 Station does NOT connect to an AP.
        print(PSTR("AT+CWJAP=\""));
        print(AP);
        print(PSTR("\",\""));
        print(PASSWORD);
        print(PSTR("\"\r\n"));
        if (waitFor(ESP_OK, ESP_ERROR) == Response::ACT) {
          return enable(retries, AP, PASSWORD, true);
        }
        return enable(--retries, AP, PASSWORD, true);

      default:
        return false;
    }
    return enable(--retries, AP, PASSWORD);
  }

  if (restart(1)) {
    enable(++retries, AP, PASSWORD);
  }

  return false;
}

bool ESP8266::restart(uint8_t retries) {
  if (retries >= 1) {
    print(PSTR("AT+RST\r\n"));
    if (waitFor(ESP_OK, ESP_ERROR) == Response::ACT) {
      return true;
    }
  }

  // TODO reset with pin
  return false;
}

int ESP8266::status(uint8_t retries) {
  if (retries >= 1) {
    uint8_t cBuf[1];
    print(PSTR("AT+CIPSTATUS\r\n"));
    if (waitFor(ESP_STATUS, ESP_EMPTY, DEFAULT_TIMEOUT, cBuf, 1, true) == Response::ACT) {
      return atoi(cBuf);
    }
  }
  return 0;
}

bool ESP8266::start(uint8_t retries, PGM_P domain, PGM_P port, uint8_t length) {
  if (retries >= 1) {
    print(PSTR("AT+CIPSTART=\"TCP\",\""));
    print(domain);
    print(PSTR("\","));
    print(port);
    print(PSTR("\r\n"));

    switch (waitFor(ESP_CON_OK, ESP_CON_ALREADY, CONNECT_TIMEOUT)) {
      case Response::ACT:
      case Response::NACT:
        print(PSTR("AT+CIPSEND="));
        print(length);
        print(PSTR("\r\n"));
        if (waitFor(ESP_START_SEND, ESP_ERROR, DEFAULT_TIMEOUT, NULL, 0, false) == Response::ACT) {
          return true;
        }
        return start(--retries, domain, port, length);

      default:
        return start(--retries, domain, port, length);
    }
  }
  return false;
}

Response ESP8266::waitFor(PGM_P act, PGM_P nact, uint32_t timeout, uint8_t *cBuf, size_t cSize, bool flush) {
  PGM_P p_act = act;
  uint8_t len_act = strlen_P(act);
  uint8_t i_act = 0;

  PGM_P p_nact = nact;
  uint8_t len_nact = strlen_P(nact);
  uint8_t i_nact = 0;

  int8_t c_byte; // Current byte
  Response value = Response::ERROR;

  uint32_t deadline = millis() + timeout;
  digitalWrite(LED_BUILTIN, HIGH);

  while (deadline >= millis()
    && value != Response::NACT && value != Response::ACT) {

    if (ESP8266_AVAILABLE()) {
      c_byte = ESP8266_READ();
      DEBUG_WRITE(c_byte);

      if (c_byte != -1) {

        // Finding ACT string from the reply
        if (len_act > 0) {
          if (c_byte == pgm_read_byte(p_act)) {

            // Move to next char and increment index
            p_act++;
            i_act++;

            if (i_act == len_act) {
              value = Response::ACT;
            }
          } else {
            // Reset index and pointer
            i_act = 0;
            p_act = act;
          }

          // Finding NACT string from the reply
          if (len_nact > 0) {
            if (c_byte == pgm_read_byte(p_nact)) {

              // Move to next char and increment index
              p_nact++;
              i_nact++;

              if (i_nact == len_nact) {
                value = Response::NACT;
              }
            } else {
              // Reset index and pointer
              p_nact = nact;
              i_nact = 0;
            }
          }
        }
      }
    }
  }
  digitalWrite(LED_BUILTIN, LOW);

  // If we have found ACT and capture has value, capture to response buffer (rBuf)
  if (value == Response::ACT && cSize) {
      DEBUG_PRINT(F("~"));
      uint8_t i_cBuf = 0;
      digitalWrite(LED_BUILTIN, HIGH);
      bool lineEnd = false;
      while (deadline >= millis() && i_cBuf < cSize && !lineEnd) {
        if (ESP8266_AVAILABLE()) {
          c_byte = ESP8266_READ();

          if (c_byte == '\r' || c_byte == '\n') {
            lineEnd = true;
          } else if (c_byte != -1) {
            DEBUG_WRITE(c_byte);
            *(cBuf + i_cBuf++) = c_byte;
          }
        }
      }
      digitalWrite(LED_BUILTIN, LOW);
      DEBUG_PRINT(F("~"));
  }

  if(flush) {
    delay(400); // Give a bit of time and empty serial
    while (deadline >= millis() && ESP8266_AVAILABLE()) {
      c_byte = ESP8266_READ();
      DEBUG_WRITE(c_byte);
    }
  }

  return value;
}

bool ESP8266::print(int num) {
  digitalWrite(LED_BUILTIN, HIGH);
  ESP8266_PRINT(num);
  DEBUG_PRINT(num);

  ESP8266_FLUSH();
  DEBUG_FLUSH();
  digitalWrite(LED_BUILTIN, LOW);
  return true;
}

bool ESP8266::print(char *str) {
  digitalWrite(LED_BUILTIN, HIGH);
  for (; (*str); str++) {
    ESP8266_WRITE(*str);
    DEBUG_WRITE(*str);
  }

  ESP8266_FLUSH();
  DEBUG_FLUSH();
  digitalWrite(LED_BUILTIN, LOW);
  return true;
}

bool ESP8266::print(PGM_P str) {
  digitalWrite(LED_BUILTIN, HIGH);
  for (uint8_t c; (c = pgm_read_byte(str)); str++) {
    ESP8266_WRITE(c);
    DEBUG_WRITE(c);
  }

  ESP8266_FLUSH();
  DEBUG_FLUSH();
  digitalWrite(LED_BUILTIN, LOW);
  return true;
}
