// #include <Arduino.h>
// #include "FS.h"
// #include <LittleFS.h>
// #include <ArduinoJson.h>
/* You only need to format LittleFS the first time you run a
   test or else use the LITTLEFS plugin to create a partition
   https://github.com/lorol/arduino-esp32littlefs-plugin
   
   If you test two partitions, you need to use a custom
   partition.csv file, see in the sketch folder */

//#define TWOPART

#define FORMAT_LITTLEFS_IF_FAILED true

  


DeserializationError error;
  

String getValue(String data, char separator, int index)
{
  int found = 0;
  int strIndex[] = {0, -1};
  int maxIndex = data.length() - 1;

  for (int i = 0; i <= maxIndex && found <= index; i++)
  {
    if (data.charAt(i) == separator || i == maxIndex)
    {
      found++;
      strIndex[0] = strIndex[1] + 1;
      strIndex[1] = (i == maxIndex) ? i + 1 : i;
    }
  }

  return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}

void listDir(fs::FS &fs, const char * dirname, uint8_t levels){
    Serial.printf("Listing directory: %s\r\n", dirname);

    File root = fs.open(dirname);
    if(!root){
        Serial.println("- failed to open directory");
        return;
    }
    if(!root.isDirectory()){
        Serial.println(" - not a directory");
        return;
    }

    File file = root.openNextFile();
    while(file){
        if(file.isDirectory()){
            Serial.print("  DIR : ");
            Serial.println(file.name());
            if(levels){
                listDir(fs, file.path(), levels -1);
            }
        } else {
            Serial.print("  FILE: ");
            Serial.print(file.name());
            Serial.print("\tSIZE: ");
            Serial.println(file.size());
        }
        file = root.openNextFile();
    }
}


bool readJsonFile(fs::FS &fs, const char * path){
    Serial.printf("\nReading Json file: %s\r\n", path);

    File file = fs.open(path,FILE_READ);
    if(!file || file.isDirectory()){
        Serial.println("- failed to open file for reading");
        return false;
    }

    StaticJsonDocument<1024> doc;
    error = deserializeJson(doc, file);

    // Test if parsing succeeds.
    if (error) {
      Serial.print(F("deserializeJson() failed: "));
      Serial.println(error.f_str());
      return false;
    }


    if(path=="/config.json")  { // debugln("============Config Read===============");
            const char* SSIDs = doc["SSID"]; // "CounterMachine"
            const char* wfpas = doc["wfpas"]; // "admin1234"
          SSIDwifi = SSIDs;
          PassWifi = wfpas;
            Serial.printf("SSID : %s\n",SSIDs);
            Serial.printf("PASS : %s\n",wfpas);
            // debugln("============Config Close===============");
          }
    
    else if(path=="/product.json")  { // debugln("============Product Read===============");
            const char* SerialNumber = doc["SerialNumber"]; // "CM000"
            const char* APoint = doc["APoint"]; // "CM000"
            const char* APpas = doc["APpas"]; // "machineCM000"
            const char* IP = doc["IP"]; // "192.168.4.1"
            const char* ver = doc["version"]; // "v.1"
            const char* productID = doc["productID"]; // "MC001-2024-9-1"


            Serial.printf("SerialNumber : %s\n",SerialNumber);
            Serial.printf("APoint : %s\n",APoint);
            Serial.printf("APpas : %s\n",APpas);
            Serial.printf("ver : %s\n",ver);
            Serial.printf("productID : %s\n",productID);
           

            DevID = SerialNumber;
            versionProd = ver;
            idProd = productID;
            AP = APoint;
            APpass = APpas;
            delay(100);
            // debugln("============Product Close===============");
          }
    
  

    file.close();
    return true;

}

void writeJsonFile(fs::FS &fs, const char * path, String data = ""){
  //data = "config:counter:admin1234"
  //data = "product:"
  //data = "sensor:400:800"

  Serial.printf("Writing Json file: %s\r\n", path);

   StaticJsonDocument<1024> doc;

   // config.json
   if(path=="/config.json") {
     String SSIDWifi, PassWifi;
     if(data != ""){
        SSIDWifi = getValue(data, ':', 1);    //CounterMachine
        PassWifi = getValue(data, ':', 2);//"admin1234"
     }
      doc["SSID"] = data == "" ? "CounterMachine" : SSIDWifi;
      doc["wfpas"] = data == "" ? "admin1234" : PassWifi;
   } 
   // product.json
   if(path=="/product.json") {
      String SN, APpass, broker, port, ver, proID;
     if(data != ""){
        SN = getValue(data, ':', 1);    //CM000
        APpass = getValue(data, ':', 2);//"machineC000"
        ver = getValue(data, ':', 5);   // "v.1"
        proID = getValue(data, ':', 6); // "MC001-2022-12-1"
     }
        doc["SerialNumber"] = data == "" ? "CM000" : SN;
        doc["APoint"] = data == "" ? "CM000" : SN;
        doc["APpas"] =  data == "" ? "machineCM000" : "machine"+SN;
        doc["IP"] ="192.168.4.1";
        doc["version"] = data == "" ? "v.1" : ver;
        doc["productID"] = data == "" ? "CM000-2022-12-1" : proID ;
   } 
   
    File file = fs.open(path, FILE_WRITE);
    serializeJson(doc, file);
    serializeJsonPretty(doc, Serial);

    file.println();
    file.close();
}

void deleteFile(fs::FS &fs, const char * path){
    Serial.printf("Deleting file: %s\r\n", path);
    if(fs.remove(path)){
        Serial.println("- file deleted");
    } else {
        Serial.println("- delete failed");
    }
    
}



void setupCRUD(){
    Serial.begin(115200);

    if(!LittleFS.begin(FORMAT_LITTLEFS_IF_FAILED)){
        Serial.println("LittleFS Mount Failed");
        return;
    }
  
  Serial.println( "LittleFS RUNNING" );  
	listDir(LittleFS, "/", 1);


    // writeJsonFile(LittleFS, "/config.json");
    // deleteFile(LITTLEFS, "/config.json");

  isexistfile =  readJsonFile(LittleFS, "/config.json");
  if(!isexistfile)  {
    Serial.println( "\nFile /config.json is NOT EXIST" ); 
    Serial.println( "Create file /config.json" ); 
    writeJsonFile(LittleFS, "/config.json");
  }

   isexistfile =  readJsonFile(LittleFS, "/product.json");
   if(!isexistfile)  {
    Serial.println( "\nFile /product.json is NOT EXIST" ); 
    Serial.println( "Create file /product.json" ); 
    writeJsonFile(LittleFS, "/product.json");
  }

  isexistfile =  readJsonFile(LittleFS, "/sensor.json");
   if(!isexistfile)  {
    Serial.println( "\nFile /sensor.json is NOT EXIST" ); 
    Serial.println( "Create file /sensor.json" ); 
    writeJsonFile(LittleFS, "/sensor.json");
  }

    Serial.println( "\nChecking Files complete" ); 
}


