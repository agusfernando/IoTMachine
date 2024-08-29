
#include <WiFi.h>
#include <WebServer.h>
#include <EEPROM.h>
#include <ArduinoJson.h>
#include "LittleFS.h"
#include "Utilities.h"
// #include "OTA.h"
#include "crudfile.h"

WebServer server(80);

unsigned long lastMsg = 0;
#define MSG_BUFFER_SIZE (150)
char msgi[MSG_BUFFER_SIZE];


// DeserializationError error;


bool testWifi(void);
void launchWeb(void);
void setupAP(void);
void createWebServer(void);
// void readConfigFile(void);
// void readProductFile(void);
// void readSensorFile(void);


void setupWifi() {
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(10);


  setupCRUD();

  Serial.println(SSIDwifi);
  Serial.println(PassWifi);


  Serial.println("Device Connecting to Internet");
  WiFi.begin(SSIDwifi, PassWifi);
  delay(1000);



  if (testWifi()) {
    Serial.println("Device Succesfully Connect to Internet");
  
    // readProductFile();
    delay(100);
    return;
  }
  setupAP();
}

bool testWifi(void) {
  int c = 0;
  Serial.println("Waiting for Wifi to connect");
  while (WiFi.status() != WL_CONNECTED) {
    if (c == 30) {
      Serial.println("");
      Serial.println("Connecting timed out");
      return false;
    }
    delay(100);
    Serial.print("*");
    c++;
  }
  return true;
}

void setupAP(void) {

  Serial.println("");
  Serial.println("Config AP");
  WiFi.mode(WIFI_AP);
  WiFi.disconnect();
  delay(100);
  int n = WiFi.scanNetworks();
  Serial.println("scan done");
  if (n == 0)
    Serial.println("no networks found");
  else {
    Serial.print(n);
    Serial.println(" networks found");
    for (int i = 0; i < n; ++i) {
      // Print SSID and RSSI for each network found
      Serial.print(i + 1);
      Serial.print(": ");
      Serial.print(WiFi.SSID(i));
      Serial.print(" (");
      Serial.print(WiFi.RSSI(i));
      Serial.print(")");
      //      Serial.println((WiFi.encryptionType(i) == ENC_TYPE_NONE) ? " " : "*");
      delay(10);
    }
  }
  Serial.println("");
  st = "<select name='ssid'>";
  for (int i = 0; i < n; ++i) {
    // Print SSID and RSSI for each network found
    st += "<option>";
    st += WiFi.SSID(i);
    st += "</option>";
  }
  st += "</select>";
  delay(100);

  WiFi.softAP(AP, APpass);
  Serial.println("softap");
  launchWeb();

  int h = 0;
  while ((WiFi.status() != WL_CONNECTED)) {
    Serial.print(".");
    delay(100);
    server.handleClient();
    delay(500);
    h++;
    if (h % 100 == 0) {
      n = WiFi.scanNetworks();



      for (int i = 0; i < n; ++i) {
        // Print SSID and RSSI for each network found

        if (WiFi.SSID(i) == SSIDwifi) {
          delay(1000);
          Serial.println("WiFi Founded. Reconnecting");
          // ESP.restart();
          setupWifi();
        }
      }
    }
  }
  // if (h > 300) {
  //   delay(1000);
  //   Serial.println("time over");
  //   ESP.restart();
  // }
}


void launchWeb()

{
  Serial.println("");
  if (WiFi.status() == WL_CONNECTED)
    Serial.println("WiFi connected");
  Serial.print("Local IP: ");
  Serial.println(WiFi.localIP());
  Serial.print("SoftAP IP: ");
  Serial.println(WiFi.softAPIP());
  createWebServer();
  // Start the server
  server.begin();
  Serial.println("Server started");
}

void createWebServer() {
  {
    server.on("/", []() {
      IPAddress ip = WiFi.softAPIP();
      String ipStr = String(ip[0]) + '.' + String(ip[1]) + '.' + String(ip[2]) + '.' + String(ip[3]);
      ////////////////////////////////////////////////
      content = "<!DOCTYPE html>\n";
      content += "<html>\n";
      content += "<head>\n";
      content += "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">\n";
      content += "<style>";

      content += "body {flex-direction: column;background-color: red;}";
      content += "h2{width: 284px;height: 125px;left: 45px;top: 77px;font-family: Nunito;font-style: normal;font-weight: 600;font-size: 48px;line-height: 65px;color: rgba(255, 255, 255, 0.6);margin-bottom: 36px;}";
      content += "h1{width: 244px;height: 27px;left: 45px;top: 238px;font-family: Nunito;font-style: normal;font-weight: 800;font-size: 18px;line-height: 25px;color: rgba(255, 255, 255, 0.6);margin-bottom: 36px;}";
      //content +="input,select {font-style: normal;font-weight: 600;font-size: 17px;line-height: 25px;margin-bottom: 20px;border-radius: 10px;height: 35px;padding: 20px;display: block;width: 97%;border: none;background: #FFFFFF;border-radius: 30px;outline: none;}";
      content += "input, select{font-size: 1.2rem;margin-top: 5px;margin-bottom: 20px;border-radius: 10px;height: 35px;padding: 5px;display: block;\n";
      content += "width: 97%;border:none;border-bottom: 1px solid #1890ff;outline: none;}";

      content += "[placeholder]:focus::-webkit-input-placeholder {\n";
      content += "transition: text-indent 0.4s 0.4s ease;text-indent: -100%;opacity: 1;}\n";
      content += "button {font-style: normal;font-weight: 600;font-size: 17px;line-height: 25px;text-align: center;width: 124px;height: 58px;left: 236px;top: 749px;background: #000000;border-radius: 30px;border: none;float: right;color: white;}";

      content += "</style>\n";
      content += "</head>\n";
      content += "<body>\n";

      content += "  <h2>Setting\nCounter Machine</h2><br><br>";
      content += "<div class=\"card\">  \n";
      content += "  <div class=\"container\">\n";
      content += "  \t<form  method='get' action='setting'>\n";

      content += "  <p>WiFi</p><br>";
      content += st;
      content += "  <p>Password</p><br>";
      content += "      <input type=\"text\" name=\"pass\" placeholder=\" Password WiFi\" required>\n";
      content += "      <button type=\"submit\" >SAVE</button> \n";
      content += "\t</form>   \n";
      content += "  </div>\n";
      content += "</div>\n";
      content += "</body>\n";
      content += "</html> ";
      server.send(200, "text/html", content);
      //                content = "{\"success\": true, \"serial_number\": \"Hy0001\", \"message\": \"Please setup to your WiFi connection\"}";
      //                server.send(200, "application/json", content);
    });

    server.on("/setting", []() {
      String qsid = server.arg("ssid");
      String qpass = server.arg("pass");
      Serial.println();
      Serial.println(qsid);
      Serial.println(qpass);

      if (qsid.length() > 0 && qpass.length() > 0) {

        StaticJsonDocument<768> doc;

        doc["SSID"] = server.arg("ssid");
        doc["wfpas"] = server.arg("pass");
        File wconf = LittleFS.open("/config.json", "w+");  // write to file
        serializeJson(doc, wconf);
        serializeJsonPretty(doc, Serial);
        wconf.println();
        wconf.close();

        delay(10);
        content = "{\"success\": true, \"serial_number\": \"" + DevID + "\"}";  //\"Hy0001\"}";
        statusCode = 200;

      } else {
        content = "{\"Error\":\"404 not found\"}";
        statusCode = 404;
        Serial.println("Sending 404");
      }
      server.sendHeader("Access-Control-Allow-Origin", "*");
      server.send(statusCode, "application/json", content);


      delay(300);
      WiFi.mode(WIFI_STA);
      WiFi.softAPdisconnect(true);
      WiFi.disconnect();
      ESP.restart();
    });
  }
}

void resetAll() {

  StaticJsonDocument<768> doc;

  doc["SSID"] = "";
  doc["wfpas"] = "";


  // write to file
  File wconf = LittleFS.open("/config.json", "w+");
  serializeJson(doc, wconf);
  serializeJsonPretty(doc, Serial);
  wconf.println();
  wconf.close();
  delay(10);
  ESP.restart();
}