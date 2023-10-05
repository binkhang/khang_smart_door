#ifndef MYRFID_HPP
#define MYRFID_HPP

#define SS_PIN_HSPI 5
#define RST_PIN_HSPI 22
#define cardTimeout 5000u
#define maxNumCard 10
#define lengthOfCardCode 10
#include <Arduino.h>
#include <MFRC522.h>
#include <SPI.h>

class RFID
{
public:
    RFID();
    void begin();
    void scanRFID();
    void printCardList();
    bool isCardPresent();
    bool isCardRegistered();
    bool addCard();
    bool removeCard();
    bool restore();
    
private:
    MFRC522 mfrc522;
    String cardList[maxNumCard];
    u_int8_t numCards;
};

#endif  