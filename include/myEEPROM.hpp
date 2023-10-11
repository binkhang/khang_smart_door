#ifndef MYEEPROM_H
#define MYEEPROM_H

#include <EEPROM.h>

#define EEPROM_SIZE 512


class myEEPROM{

public:
    myEEPROM();
    ~myEEPROM();
    void begin();
    void restore();
    void write(uint16_t addr, uint8_t Obj);
    int read(uint16_t addr);
    void readAll();

};


#endif