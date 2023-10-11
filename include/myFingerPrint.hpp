#ifndef FINGERPRINT_HPP
#define FINGERPRINT_HPP

// #define debugMode
#include <Adafruit_Fingerprint.h>
#include "myEEPROM.hpp"
#include <vector>

#define ScanTimeoutMillis 5000u
#define StartAddrSaveFP 1  //Which each address will be used to save a FP ID. Range of ID value (1 to TotalFP)
#define TotalFP 20
extern uint8_t errorCount;
extern bool isDoorOpen;

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
    bool enroll();
    bool unEnroll(u_int8_t id);
    void getLog();
    bool debugFinger();
    void diagFingerPrint();
    bool restore();
    void queryFinger();
    std::vector<int> getFPInUsed();
    ~FingerPrint();
};
#endif