#ifndef DIAG_HPP
#define DIAG_HPP

#include "myFingerPrint.hpp"


void DiagMainFunction(){
    // This function is used for system-diag and will be called every 100ms 

    FingerPrint myFingerPrint;
    // run diag for fingerprint
    myFingerPrint.diagFingerPrint();
}

#endif