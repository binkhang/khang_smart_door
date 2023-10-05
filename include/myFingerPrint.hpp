#ifndef FINGERPRINT_HPP
#define FINGERPRINT_HPP

// #define debugMode
#include <Adafruit_Fingerprint.h>

#define ScanTimeoutMillis 5000u
extern uint8_t errorCount;
enum fingerLocalStatus{
    //enum used for enroll method 
    FINGER_OK,
    FINGER_ERROR,
    FINGER_TIMEOUT,
};

class FingerPrint {
    Adafruit_Fingerprint finger;
public:
    FingerPrint();
    void scanFinger();
    void begin(uint16_t baudRate);
    bool enroll(uint8_t id);
    bool unEnroll(u_int8_t id);
    void getLog();
    bool debugFinger();
    void diagFingerPrint();
    bool restore();
    void queryFinger();


};
#endif