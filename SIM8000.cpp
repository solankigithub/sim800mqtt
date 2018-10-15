///Define class for MQTT
#include <stdint.h>
#include <string.h>
#include "Arduino.h"
#include "SIM8000.h"

bool SIM8000::checkModem()
{
  if (sendATcommand("AT\r", "OK", 500) == 0) return false;
  return true;
};

SIM8000::SIM8000() {
  pinMode(RESET_PIN, OUTPUT);
};

bool SIM8000::initialize(uint8_t mode) {
  uint8_t retryCount = 0;
  this->mode = mode;
  if (!initModem()) {
    Serial.println("Modem initialize failed..");
    return false;
  }

  while (retryCount < 5)
  {
    if (initNetwork()) break;
    Serial.println("Network initialize failed..");
    retryCount++;
  }

  if (!initGPRS()) {
    Serial.println("GPRS initialize failed..");
    return false;
  }
  return true;
}

bool SIM8000::initModem() {
  uint8_t tryCount = 0;
  resetModem();
  while (1)
  {
    if (checkModem()) break ;
    if (tryCount > 5) return false;
    tryCount++;
  }

  if (sendATcommand("ATE0\r", "OK", 1000) == 0) return false;
  if (sendATcommand("AT+CFUN?\r", "+CFUN: 1", 1000) == 0) return false;
  if (sendATcommand("AT+CREG?\r", "+CREG: 0,1", 1000) == 0) return false;
#ifdef debug
  Serial.println("Modem initialized successfully...");
#endif
  return true;
};

bool SIM8000::initNetwork() {
  if (this->mode == 0) { //MODE = 0
    if (sendATcommand("AT+CIPMODE=0\r", "OK", 1000) == 0) return false;
  }
  else {    // MODE = 1
    if (sendATcommand("AT+CIPMODE=1\r", "OK", 1000) == 0) return false;
  }
  if (sendATcommand("AT+CIPSRIP=0\r", "OK", 1000) == 0) return false;
  if (sendATcommand("AT+CGATT?\r", "+CGATT: 1", 5000) == 0) return false;
  if (sendATcommand("AT+CIPSTATUS\r", "INITIAL", 5000) == 0) return false;

  Serial.println("Network Initialized...");
  return true;
}

bool SIM8000::initGPRS() {
  if (sendATcommand("AT+CSTT=\"portalnmms\",\"\",\"\"\r", "OK", 30000) == 0) return false;
  if (sendATcommand("AT+CIPSTATUS\r", "START", 2000)  == 0 ) return false;
  if (sendATcommand("AT+CIICR", "OK", 30000)  == 0 ) return false;
  if (sendATcommand("AT+CIPSTATUS\r", "GPRSACT", 500)  == 0 ) return false;
  if (sendATcommand("AT+CIFSR\r", ".", 10000)  == 0 ) return false;
  if (sendATcommand("AT+CIPSTATUS\r", "IP STATUS", 2000)  == 0 ) return false;

  Serial.println("GPRS Initialized...");
  return true;

}

bool SIM8000::startTCP(char * servername, int port) {
  char str[100];
  memset(str, 0, sizeof(str));
  sprintf((char*)str, "AT+CIPSTART=\"TCP\",\"%s\",\"%d\"", servername, port);
  //str[strlen(str)] = '\0';
  if (sendATcommand(str, "OK\r\n\r\nCONNECT", 30000)  == 0 ) return false;
  Serial.print("TCP Connection opened in mode =");
  Serial.println(this->mode);
  return true;
}


bool SIM8000::setMode(uint8_t mode) {
  if (mode == 0)
    if (sendATcommand("AT+CIPMODE=0\r", "OK", 1000) == 0) return false;
    else if (mode == 1)
      if (sendATcommand("AT+CIPMODE=1\r", "OK", 1000) == 0) return false;

  this->mode = mode;
  return true;
}


short int SIM8000::getMode() {
  return mode;
}

bool SIM8000::checkNetwork() {
    
};

bool SIM8000::checkGPRS() {

};

bool SIM8000::getMACAddress() {

}

bool SIM8000::getIPAddress() {

}


void SIM8000::resetModem() {
#ifdef debug
  Serial.println("Resetting Modem.., please wait for 10 seconds..");
#endif
  digitalWrite(RESET_PIN, LOW);
  delay(100);
  digitalWrite(RESET_PIN, HIGH);
  delay(15000);
};

char* SIM8000::getTCPStatus()
{
  return sendATCommandResponse("AT+CIPSTATUS\r", 1000);
}

void SIM8000::setAPN(char* APN, char* username, char*password) {

};


int8_t SIM8000::sendATcommand(char* ATcommand, char* expected_answer, unsigned int timeout)
{

  uint8_t x = 0,  answer = 0;
  char response[500];
  unsigned long previous;
  char* str;
  memset(response, '\0', 100);    // Initialize the string
  delay(100);
  while ( Serial1.available() > 0) Serial1.read();   // Clean the input buffer
  Serial1.println(ATcommand);    // Send the AT command

#ifdef debug
  Serial.println(ATcommand);    // Send the AT command
#endif
  x = 0;
  previous = millis();
  // this loop waits for the answer
  do {
    if (Serial1.available() != 0) {
      // if there are data in the UART input buffer, reads it and checks for the asnwer
      response[x] = Serial1.read();
      //Serial1.print(response[x]);
      x++;
      // check if the desired answer  is in the response of the module
      if (strstr(response, expected_answer) != NULL)
      {
        answer = 1;
      }
    }
  }
  // Waits for the asnwer with time out
  while ((answer == 0) && ((millis() - previous) < timeout));
#ifdef debug
  Serial.println(response);    // Send the AT command
#endif
  return answer;
}

char* SIM8000::sendATCommandResponse(char* ATcommand, unsigned int timeout) {
  unsigned long previous = millis();
  char response[500];
  uint8_t index = 0;
  //while (Serial1.available())
  //  Serial1.read();

  Serial.println(ATcommand);
  while (1) {
    if ((millis() - previous) > timeout) break;
    if (Serial1.available())
    {
      while (Serial1.available()) {
        response[index] = (char)Serial1.read();
        index++;
      }
      response[index] = '\0';
    }
    if (strlen(response) > 0)
      return response;
    else
      return 0;
  }
  return 0;
}




