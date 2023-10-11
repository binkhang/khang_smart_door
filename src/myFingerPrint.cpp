#include "myFingerPrint.hpp"
extern myEEPROM eeprom;
FingerPrint::FingerPrint() : finger(&Serial2)
{

}

FingerPrint::~FingerPrint() 
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
    // Serial.println("There are no fingerprint");
  //   return;
  // }
  int status = -1;
  //first stage: get image
  status = finger.getImage();
  if (status == FINGERPRINT_NOFINGER) {return;}
  else if(status != FINGERPRINT_OK) { 
    Serial.print("Your FingerPrint module has an issue at first stage of scanFinger (get image) , issue code: 0x0"); Serial.println(status);
    return;
  }

  //second stage: convert image
  status = finger.image2Tz();
  if (status != FINGERPRINT_OK){
    Serial.print("Your FingerPrint module has an issue at second stage of scanFinger (convert image) , issue code: 0x0"); Serial.println(status);
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
bool FingerPrint::enroll(){

  int status = -1;
  int id = 0;
  // temp stage to get ID
  std::vector<int> FPInUse = getFPInUsed();
  for (int idx = 1; idx < TotalFP + 1; idx++){
    if (FPInUse[idx] == 0){
      id = idx;
      break;
    }
  }
  if (id == 0){
    Serial.println("FingerPrint list is fulled");
    return false;
  }


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
          Serial.print("Your FingerPrint module has an issue at first stage of enroll (get image) , issue code: 0x0");
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
    Serial.print("Your FingerPrint module has an issue at first stage of enroll (convert image) , issue code: 0x0");
        Serial.println(status);
        return false;
  }

  status = 0;
  delay(1000);
  while (status != FINGERPRINT_NOFINGER){
    Serial.println("Remove your finger");
    status = finger.getImage();
  }
  delay(1000);

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
          Serial.print("Your FingerPrint module has an issue at second stage of enroll (get image) , issue code: 0x0");
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
    Serial.print("Your FingerPrint module has an issue at second stage of enroll (convert image) , issue code: 0x0");
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
    Serial.print("Your FingerPrint module has an issue at third stage of enroll (create model) , issue code: 0x0");
    Serial.println(status);
    return false;
  }

  //third stage: save model
  Serial.print("Saving model for #");
  Serial.println(id);
  status = finger.storeModel(id);
  if (status != FINGERPRINT_OK)
  {
    Serial.print("Your FingerPrint module has an issue at third stage of enroll (save model) , issue code: 0x0");
    Serial.println(status);
    return false;
  }

  // fourth stage: save id into EEPROM
  
  eeprom.write(id, id);

  // check if num of id in eeprom not match with 

  return true;
}

bool FingerPrint::unEnroll(uint8_t id ){
  uint8_t status = -1;
  status = finger.deleteModel(id);
  Serial.print("Deleteting model #");Serial.println(id);
  if(status != FINGERPRINT_OK)
  {
    Serial.println("Failed to delete model");
    return false;
  }
  eeprom.write(id,0);
  delay(500);
  Serial.print("Model # "); Serial.print(id); Serial.println(" has been deleted");
  return true;
}


bool FingerPrint::debugFinger(){
  // This method is used to check if number of template in eeprom not match with in sensor
}

void FingerPrint::diagFingerPrint(){
  // This method is used to check if there are any issue with fingerprint module (connection,...)

  // This method will run on diag main function
}

bool FingerPrint::restore(){
  //This method is used to delete all template (both eeprom and sensor)
  for (int i = 1; i<TotalFP+1;i++){
    eeprom.write(i,0);
  }
  Serial.println("Successfully delete all template");
  return finger.emptyDatabase() == FINGERPRINT_OK;  
}

void FingerPrint::queryFinger(){
  // This medthod is used to query number of template
}

std::vector<int> FingerPrint::getFPInUsed(){
  // This method is used to get the current FP in use
  std::vector<int> FPIndex(TotalFP + 1);
  for (int i = 1; i < TotalFP; i++){
    if (eeprom.read(i) != 0){
      FPIndex[i]=eeprom.read(i);
    }
    else{
      FPIndex[i]=0;
    }
  }

  //Debug
  
  // for (int j = 0; j < FPIndex.size(); j++)
  // {
  //   Serial.print("Index "); Serial.print(j); Serial.print(" :"); Serial.println(FPIndex[j]);
  // }
  return FPIndex;
}