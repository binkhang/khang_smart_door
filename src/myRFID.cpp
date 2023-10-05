#include "MyRFID.hpp"

RFID::RFID() : mfrc522(SS_PIN_HSPI, RST_PIN_HSPI)
{
    numCards = 0;
}

/*-------------Init RFID-------------*/
void RFID::begin()
{
    SPI.begin(14, 12, 13, SS_PIN_HSPI);
    mfrc522.PCD_Init();
    Serial.println("RFID Started");
    Serial.println(mfrc522.uid.size);
}

/*------------Check RFID-------------*/
void RFID::scanRFID()
{
    if (isCardPresent())
    {
        if (isCardRegistered())
        {
            Serial.println("Card is registered");
        }
        else
        {
            Serial.println("Card is not registered");
        }
    }
}

/*----------Detect card--------------*/
bool RFID::isCardPresent()
{
    if (mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial())
    {
        // Serial.println("Card detected");
        return true;
    }
    return false;
}

/*-----------Check card exist---------------*/
bool RFID::isCardRegistered()
{
    String cardUID = "";
    for (byte i = 0; i < mfrc522.uid.size; i++)
    {
        cardUID += String(mfrc522.uid.uidByte[i], HEX);
    }
    cardUID.toUpperCase();

    for (int i = 0; i < numCards; i++)
    {
        if (cardUID == cardList[i])
        {
            return true;
        }
    }
    return false;
}
/*-----------Add card-----------------*/
bool RFID::addCard()
{
    if (numCards < maxNumCard)
    {
        Serial.println("Present your card");
        unsigned long startTime = millis();

        while (millis() - startTime <= cardTimeout)
        {
            if (isCardPresent())
            {
                String cardUID = "";
                for (byte i = 0; i < mfrc522.uid.size; i++)
                {
                    cardUID += String(mfrc522.uid.uidByte[i], HEX);
                }
                cardUID.toUpperCase();
                cardList[numCards] = cardUID;
                numCards++;
                Serial.println("Successfully add your new card");
                return true; 
            }
        }

        Serial.println("Timeout: No card detected");
    }
    else
    {
        Serial.println("Failed to add card. Card list has fulled");
    }

    return false; 
}

/*-------------Remove card------------*/
bool RFID::removeCard()
{
    if (numCards > 0)
    {
        Serial.println("Present your card");
        unsigned long startTime = millis();
        while (millis() - startTime <= cardTimeout)
        {
            if (isCardPresent())
            {
                String cardUID = "";
                for (byte i = 0; i < mfrc522.uid.size; i++)
                {
                    cardUID += String(mfrc522.uid.uidByte[i], HEX);
                }
                cardUID.toUpperCase();

                for (int i = 0; i < numCards; i++)
                {
                    if (cardUID == cardList[i])
                    {
                        for (int j = i; j < numCards - 1; j++)
                        {
                            cardList[j] = cardList[j + 1];
                        }
                        numCards--;
                        Serial.println("Card removed from the list");
                        return true; 
                    }
                }
                Serial.println("Card not found in the list");
            }
        }
        Serial.println("Timeout: No card detected");
    }
    else
    {
        Serial.println("Card list is empty");
    }

    return false; 
}


/*------------Print list card----------*/
void RFID::printCardList()
{
    Serial.println("Registered Cards:");
    for (int i = 0; i < numCards; i++)
    {
        Serial.print(i);Serial.print(": ");Serial.println(cardList[i]);
    }
}


bool RFID::restore(){
    // This method used to delete all registed RFID card
    Serial.println("Successfully delete all you RFID card");
    
}