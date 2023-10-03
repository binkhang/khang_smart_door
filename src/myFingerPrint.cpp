#include "myFingerPrint.hpp"
FingerPrint::FingerPrint() : finger(&Serial2)
{

}

void FingerPrint::begin(uint16_t baudRate) 
{
   finger.begin(baudRate);

   if (finger.verifyPassword()) {
    Serial.println("Found fingerprint sensor!");
  } else {
    Serial.println("Did not find fingerprint sensor :(");
    while (1) { delay(1); }
  }

  Serial.println(F("Reading sensor parameters"));
  finger.getParameters();
  Serial.print(F("Status: 0x")); Serial.println(finger.status_reg, HEX);
  Serial.print(F("Sys ID: 0x")); Serial.println(finger.system_id, HEX);
  Serial.print(F("Capacity: ")); Serial.println(finger.capacity);
  Serial.print(F("Security level: ")); Serial.println(finger.security_level);
  Serial.print(F("Device address: ")); Serial.println(finger.device_addr, HEX);
  Serial.print(F("Packet len: ")); Serial.println(finger.packet_len);
  Serial.print(F("Baud rate: ")); Serial.println(finger.baud_rate);
  //get number of templates
  finger.getTemplateCount();
  Serial.print(F("Sensor contains: "));Serial.print(finger.templateCount);Serial.println(" templates");
}

void FingerPrint::scanFinger(){
  //check if dont have any fingerprint data
  // finger.getTemplateCount();
  // if (finger.templateCount == 0){
  //   // Serial.println("There are no fingerprint");
  //   return;
  // }
  int status = -1;
  //first stage: get image
  status = finger.getImage();
  if (status == FINGERPRINT_NOFINGER) {return;}
  else if(status != FINGERPRINT_OK) { 
    Serial.print("Your FingerPrint module has an issue at first stage (get image) , issue code: 0x0"); Serial.println(status);
    return;
  }

  //second stage: convert image
  status = finger.image2Tz();
  if (status != FINGERPRINT_OK){
    Serial.print("Your FingerPrint module has an issue at second stage (convert image) , issue code: 0x0"); Serial.println(status);
    return;
  }
  
  // third stage: searching!
  status = finger.fingerSearch();
  if (status == FINGERPRINT_NOTFOUND){
    Serial.println("Did not find a match");
    return ;
  }
  else if(status == FINGERPRINT_OK)
  {
    Serial.print("Found ID #");
    Serial.print(finger.fingerID);
    Serial.print(" with confidence of ");
    Serial.println(finger.confidence);
    return;
  }
}
bool FingerPrint::enroll(uint8_t id){

  int status = -1;

  //first stage: get image
  unsigned int startTime = millis();
  Serial.println("Place your finger on sensor");
  while (status != FINGERPRINT_OK && (millis() - startTime) < ScanTimeoutMillis)
    {
        status = finger.getImage();
        if (status == FINGERPRINT_OK) 
        {
          Serial.println("Image taken");
          break;
        }
        else if (status == FINGERPRINT_NOFINGER) { 
        }
        else{
          Serial.print("Your FingerPrint module has an issue at first stage (get image) , issue code: 0x0");
          Serial.println(status);
          return 0;
        }
    }
  //check if timeout
  if (status == FINGERPRINT_NOFINGER) {
    Serial.println("Timeout to scan fingerprint");
    return false;
  }
  

  //first stage: convert image
  status = finger.image2Tz(1);
  if (status != FINGERPRINT_OK)
  {
    Serial.print("Your FingerPrint module has an issue at first stage (convert image) , issue code: 0x0");
        Serial.println(status);
        return false;
  }

  status = 0;
  delay(1000);
  while (status != FINGERPRINT_NOFINGER){
    Serial.println("Remove your finger");
    status = finger.getImage();
  }


  //second stage: get image
  startTime = millis();
  Serial.println("Place your same finger on sensor again");
  while (status != FINGERPRINT_OK && (millis() - startTime) < ScanTimeoutMillis)
    {
        status = finger.getImage();
        if (status == FINGERPRINT_OK) 
        {
          Serial.println("Image taken");
          break;
        }
        else if (status == FINGERPRINT_NOFINGER) {}
        else{
          Serial.print("Your FingerPrint module has an issue at second stage (get image) , issue code: 0x0");
          Serial.println(status);
          return false;
        }
    }
  //check if timeout
  if (status == FINGERPRINT_NOFINGER) {
    Serial.println("Timeout to scan fingerprint");
    return false;
  }
  
  //second stage: convert image
  status = finger.image2Tz(2);
  if (status != FINGERPRINT_OK)
  {
    Serial.print("Your FingerPrint module has an issue at second stage (convert image) , issue code: 0x0");
    Serial.println(status);
    return false;
  }

  //third stage:create model

  Serial.print("Creating model for #");
  Serial.println(id);
  status = finger.createModel();
  if(status == FINGERPRINT_ENROLLMISMATCH){
    Serial.println("Failed to add new finger because your two fingers do not match");
    return false;
  }
  else if (status != FINGERPRINT_OK)
  {
    Serial.print("Your FingerPrint module has an issue at third stage (create model) , issue code: 0x0");
    Serial.println(status);
    return false;
  }

  //third stage: save model
  Serial.print("Saving model for #");
  Serial.println(id);
  status = finger.storeModel(id);
  if (status != FINGERPRINT_OK)
  {
    Serial.print("Your FingerPrint module has an issue at third stage (save model) , issue code: 0x0");
    Serial.println(status);
    return false;
  }

  // fourth stage: id into EEPROM
  // check if num of id in eeprom not match with 
  return true;
}

bool FingerPrint::unEnroll(uint8_t id ){
  uint8_t status = -1;
  status = finger.deleteModel(id);
  Serial.print("Deleteting model #");Serial.println(id);
  if(id != FINGERPRINT_OK)
  {
    Serial.println("Failed to delete model");
    return false;
  }
  delay(1000);
  Serial.print("Model # "); Serial.print(id); Serial.println(" has been deleted");
  return true;
}


bool FingerPrint::debug(){
  // This method is used to check if number of template in eeprom not match with in sensor
}

void FingerPrint::diagFingerPrint(){
  // This method is used to check if there are any issue with fingerprint module (connection,...)

  // This method will run on diag main function
}

bool FingerPrint::restore(){
  //This method is used to delete all template (both eeprom and sensor)
  return finger.emptyDatabase() == FINGERPRINT_OK;  
}