
#include "SPIFFS.h"
#include "myFingerPrint.hpp"
#include "diag.hpp"


//my FingerPrint
FingerPrint myFingerPrint;
uint8_t errorCount = 0;
int task = -1;
void setup() {
  Serial.begin(9600);
  
  //init SPIFFS
  if (!SPIFFS.begin()) {
    Log("Failed to init SPIFFS");
    return;
  }

  myFingerPrint.begin(57600);

}
void loop() {
  int id;
  while (1)
  {
    task = readNumber();
    switch (task)
    {
    case 0:
      myFingerPrint.scanFinger();
      break;
    case 1: 
      id = readNumber();
      myFingerPrint.enroll(id);
      task = 0;
      break;
    case 2: 
      id = readNumber();
      myFingerPrint.unEnroll(id);
      task = 0;
      break;
    default:
      break;
    }
  }

}

void Log(String log){
  Serial.println(log);
}

uint8_t readNumber()
{
    uint8_t num = 0;
    if (Serial.available()){
      num = Serial.parseInt();
    }
    return num;
}
