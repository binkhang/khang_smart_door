#include "myEEPROM.hpp"


myEEPROM eeprom;
bool doorStatus = false;

void setup(){
    Serial.begin(9600);
    eeprom.begin();
}

void loop(){
    int value = 0;
    int addr = 0;
    int task = readNumber();
    switch (task)
    {
    case 1:
        Serial.println("Nhap value");
        while(value == 0){
            
            value = readNumber();
        }
        Serial.println("Nhap addr");
        while(addr == 0){
            addr = readNumber();
        }
        eeprom.write(addr,value);
        break;
    case 2:
        Serial.println("Nhap addr");
        while(addr == 0){
            addr = readNumber();
        }
        Serial.print("Value at "); Serial.print(addr); Serial.print(" is: ");
        Serial.println(eeprom.read(addr));
        break;
    case 3:
        eeprom.restore();
        break;
    case 4:
        eeprom.readAll();
        break;

    default:
        break;
    }

}

int readNumber()
{
    int num = 0;
    if (Serial.available())
        num = Serial.parseInt();
    return num;
}
