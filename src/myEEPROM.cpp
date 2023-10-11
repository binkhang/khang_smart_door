#include "myEEPROM.hpp"

myEEPROM::myEEPROM(){
    //do nothing
}

void myEEPROM::begin(){
    EEPROM.begin(EEPROM_SIZE);
    Serial.println("Init EEPROM with 512 bytes");
    Serial.print("EEPROM size: "); Serial.println(EEPROM.length());
}

myEEPROM::~myEEPROM(){
    // do nothing
}

void myEEPROM::restore(){
    for (int i = 0; i < EEPROM.length();i++){
        EEPROM.write(i,0);
        EEPROM.commit();
    }
    Serial.println("Restore EEPROM done");
}

void myEEPROM::write(uint16_t addr, uint8_t Obj){
    EEPROM.write(addr, Obj);
    EEPROM.commit();
    // Serial.println("write data successfull");
}

int myEEPROM::read(uint16_t addr){
    return EEPROM.read(addr);
}

void myEEPROM::readAll(){
    for (int i = 0; i < EEPROM.length(); i++){
        if (read(i) != 0){
            Serial.print("Value at addr "); Serial.print(i); Serial.print(" is "); Serial.println(read(i));
        }
    }
}