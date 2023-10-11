
#include "gui.h"
#include "SPIFFS.h"
#include "myFingerPrint.hpp"
#include "diag.hpp"
#include "myRFID.hpp"

void setup()
{
    Serial.begin(9600); /* prepare for possible serial debug */
    gui_begin();
}

void loop()
{
    lv_timer_handler(); /* let the GUI do its work */
    delay(5);
}

