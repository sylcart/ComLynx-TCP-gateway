/*
    Name: DanfossTLX-REST        
    
    Used Libraries - output from verbose compile
    Multiple libraries were found for "WiFi.h" Used:       ...\packages\esp32\hardware\esp32\2.0.3\libraries\WiFi
    Using library WiFi at version 2.0.0 in folder:         ...\packages\esp32\hardware\esp32\2.0.3\libraries\WiFi 
    Using library WebServer at version 2.0.0 in folder:    ...\packages\esp32\hardware\esp32\2.0.3\libraries\WebServer 
    Using library ArduinoJson at version 6.19.4 in folder: ...\libraries\ArduinoJson 
    Using library FS at version 2.0.0 in folder:           ...\packages\esp32\hardware\esp32\2.0.3\libraries\FS 
    
*/ 

#include "DanfossTLX-RS485.h"
#include "Secrets.h" // #defines of SSID & WIFI_PSWD
#include <WiFi.h>         
#include <WiFiClient.h>
#include <WebServer.h>
#include <ArduinoJson.h>


#define BLYNK_PRINT Serial
#define RXD2 16
#define TXD2 17

//const char* ssid     = SSID;
//const char* password = WIFI_PSWD;

String Program="DanfossTLX-REST";
String Arduino="1.18.19";
String CodeDate = "2022-06-19";
String Board  = "LOLIN D32";

DanfossTLX MyTLX(RXD2, TXD2);
WebServer server(80);

StaticJsonDocument<250> jsonDocument;
char jsonbuffer[250];
void CreateJson(const char *tag, float value, const char *unit) {  
  jsonDocument.clear();
  jsonDocument["type"] = tag;
  jsonDocument["value"] = value;
  jsonDocument["unit"] = unit;
  serializeJson(jsonDocument, jsonbuffer);  
}
void CreateJsonTxt(const char *tag, const char *value) {  
  jsonDocument.clear();
  jsonDocument["type"] = tag;
  jsonDocument["value"] = value;
  serializeJson(jsonDocument, jsonbuffer);  
}
  
// Web page that holds all parameters/measurements in a table
void HandleRoot() {
  String msg = "<!DOCTYPE html><html><head><meta http-equiv=\"refresh\" content=60><title>Danfoss TLX</title></head><style>tr:nth-child(even) {background-color: #f3f3f3;}</style><body><h1>Danfoss TLX</h1><table><tr><th>Description</th><th>Value</th><th>Unit</th><th>Indeks</th></tr>";
  if(MyTLX.TLX.ProductNumber.c_str()[0] != NULL ){ msg += "<tr><td>Product Number</td><td>" + MyTLX.TLX.ProductNumber + "</td><td></td><td></td></tr>" ;}
  if(MyTLX.TLX.SerialNumber.c_str()[0]  != NULL ){ msg += "<tr><td>Serial Number</td><td>" + MyTLX.TLX.SerialNumber + "</td><td></td><td></td></tr>" ;}
  if(MyTLX.TLX.OpModeTxt.c_str()[0] != NULL){ msg += "<tr><td>Operation mode</td><td>" + MyTLX.TLX.OpModeTxt + "</td><td></td><td></td></tr>" ;}
  for (int i = 0; i < MyTLX.DATA_ENUMS; i++) {
    msg +=  "<tr><td>" + MyTLX.TLX.Name[i] +"</td><td>"+MyTLX.MeasString((DanfossTLX::Par_e)i)+"</td><td>"+MyTLX.TLX.Unit[i]+"</td><td>"+MyTLX.TLX.ParName[i] + "</td></tr>";
  }                 
  msg += "</body></html>";
  server.send(200, "text/html", msg);
}

// Send json object of requested parameter/value. See DanfossTLX::ParName for valid names
void ReturnTLXMeasValue() {
  String uri = server.uri();
  DanfossTLX::Par_e ParEnum = MyTLX.DATA_ENUMS; 
  bool found = false;
  if (uri.length() > 3 && uri.startsWith("/")){
    uri.remove(0,1);
    //Serial.println(uri);
    for (int i = 0; i < MyTLX.DATA_ENUMS; i++) {
      // Serial.println( uri + "<>" + MyTLX.TLX.ParName[i] );
      uri.toUpperCase();
      String Name = MyTLX.TLX.ParName[i];
      Name.toUpperCase();
      if ( uri == Name ){
        found = true;
        CreateJson(MyTLX.TLX.ParName[i].c_str(), MyTLX.TLX.Meas[i], MyTLX.TLX.Unit[i].c_str() );
        server.send(200, "application/json", jsonbuffer);
        break;
      }
    }
  }
  // Wrong parameter - return txt result for debug
  // http://#IP#/Unkown?Earth=MostlyHarmless&Answer=42
  // Pararameter not recognized 
  // URI: /Unkown
  // Method: GET
  // Arguments: 2
  //  Earth: MostlyHarmless
  //  Answer: 42
  
  if(!found){
   String msg = "Pararameter not recognized \n\n";
   msg += "URI: ";
   msg += server.uri();
   msg += "\nMethod: ";
   msg += (server.method() == HTTP_GET) ? "GET" : "POST";
   msg += "\nArguments: ";
   msg += server.args();
   msg += "\n";
   for (uint8_t i = 0; i < server.args(); i++) {
     msg += " " + server.argName(i) + ": " + server.arg(i) + "\n";
   }
   server.send(404, "text/plain", msg);
  }
}

void setup()
{
  Serial.begin(115200) ;  // Debug console
  Serial.println(CodeDate + Program);
  Serial.println(Arduino);
  Serial.println(Board);
  Serial.println(String("Compile: ") + String(__DATE__)  +" " + String(__TIME__));
  Serial.println(String(__FILE__));

  WiFi.mode(WIFI_STA); // Connect to AP
  WiFi.begin(SECRET_SSID, SECRET_WIFI_PSWD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(SECRET_SSID);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  server.on("/", HandleRoot);
  server.on("/OpModeTxt", []() { CreateJsonTxt("OpModeTxt", MyTLX.TLX.OpModeTxt.c_str());      server.send(200, "application/json", jsonbuffer); }); 
  server.on("/Product", []()   { CreateJsonTxt("Product",   MyTLX.TLX.ProductNumber.c_str() ); server.send(200, "application/json", jsonbuffer); }); 
  server.on("/SerialTxt", []() { CreateJsonTxt("Serial",    MyTLX.TLX.SerialNumber.c_str() );  server.send(200, "application/json", jsonbuffer); }); 
  server.onNotFound(ReturnTLXMeasValue);
  server.begin();
  Serial.println("HTTP server started\nSetup ended");
}

void loop()
{
  const unsigned long  WIFITIME = 30000;
  const unsigned long  TIMETLX = 15000;
  static unsigned long TimeTLX = millis();
  
  // Wraparround every ~50 days;
  if (millis() < TimeTLX){
    TimeTLX = millis();
  }
  if (millis() - TimeTLX >  TIMETLX){
    MyTLX.TLXGetParameters();
    //MyTLX.TLXPrintAll();
    TimeTLX = millis();
  }
  server.handleClient();
  // Test if connected
  if(WiFi.status() != WL_CONNECTED){
    Serial.println("Restart Wifi");
    WiFi.disconnect();
    WiFi.begin(SECRET_SSID, SECRET_WIFI_PSWD);
    unsigned long timenow=millis();
    while (WiFi.status() != WL_CONNECTED  && millis() - timenow < WIFITIME){
      delay(500);
      Serial.print(".");
    }
    Serial.println("Timeout");
  }
  
  
  delay(2);
}
