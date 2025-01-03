/*
    Name: ComLynx-TCP-gateway

    usefull to monitor Danfoss inverters by using the ComLynx protocol on RS485 protocol. metrics can be exported using MQTT protocol or via REST API

    Based on DanfossTLX by AMajland https://github.com/AMajland/Danfoss-TLX
    Based on DanfossTLX by Torben  https://github.com/TorbenNor/Danfoss-TLX
   
    Used Libraries - output from verbose compile
    Using library ArduinoOTA at version 3.1.0
    Using library Networking at version 3.1.0
    Using library Update at version 3.1.0
    Using library WiFi at version 3.1.0
    Using library ArduinoJson at version 7.3.0
    Using library PubSubClient at version 2.8
    Using library RemoteDebug2 at version 4.0.1
    Using library ESP Async WebServer at version 3.4.5
    Using library FS at version 3.1.0
    Using library AsyncTCP at version 1.1.4
    Using library ESPmDNS at version 3.1.0
    Using library Preferences at version 3.1.0
    Using library WebSockets at version 2.6.1
    Using library NetworkClientSecure at version 3.1.0
    
*/ 

#include "DanfossInverter.h" // Danfoss Inverter Class (TLX + ULX)
#include "ComLynx.h" // comlynx RS485 Interface Class
#include "InverterConfigElement.h"

#include <ArduinoOTA.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <ArduinoJson.h>
#include <MQTT.h>
#include <RemoteDebug.h>
#include <ESPAsyncWebServer.h>
#include <vector>
#include <ESPmDNS.h>


#define BLYNK_PRINT Serial
#define RXD2 16
#define TXD2 17

#define HOSTNAME "CLX-GW"

#define Program "ComLynx-TCP-gateway"
#define Version "0.3"
#define CodeDate "2025-01-02"
#define Board "LOLIN D32 - ESP32 3.1.0"
String hostname(HOSTNAME);

String SSID;
String SECRET_WIFI_PSWD;
const char *ap_default_ssid;        // Access Point Mode
const char *ap_default_psk = NULL;  // No default password on AP mode
const unsigned long WIFITIME  = 15000;

String ssid = "";
String password = "";
boolean dhcpOn = true;
IPAddress StaticIP;
IPAddress Gateway;
IPAddress Netmask;
IPAddress primaryDNS;
boolean MQTTOn;
boolean RestAPIOn;
unsigned long ClxInterval;
IPAddress MQTTIP;
unsigned int MQTTPort;
String MQTTUser;
String MQTTPwd;
String MQTTPrefix;
String MQTTdeviceName ;
unsigned int HTTPPort;

boolean* InverterScan;
float* ScanPercent;

//Internal Timers
unsigned long startMillis;

StaticJsonDocument<1000> jsonDocument;
char jsonbuffer[1000];

JsonDocument ParamsJson;
JsonDocument InvJson;

ComLynx MyComLynx(RXD2, TXD2);
DanfossInverter* myComLynxInverter;
DanfossInverter* myAPIInverter;
DanfossInverter* myInv;
std::vector<DanfossInverter*> mypInverterList;
std::vector<InverterConfigElement*> InverterMemList;

AsyncWebServer* server;

WiFiClient WifiMqttClient;
MQTTClient MQTTClient;
RemoteDebug Debug;

void EstablishWifiConnect(void){
  if (SSID.length() > 0) {
    if (!dhcpOn) {  //Static IP
      IPAddress secondaryDNS(8, 8, 4, 4);                    //optional
      if (!WiFi.config(StaticIP, Gateway, Netmask, primaryDNS, secondaryDNS)) {
        Serial.println("WIFI STA Failed to configure");
      }
    }
    Serial.println("Wifi Begin : " + SSID);
    WiFi.begin(SSID, SECRET_WIFI_PSWD);
    while (WiFi.status() != WL_CONNECTED && (millis() - startMillis < WIFITIME)) {  // Wait time out to connect in STA mode
      Serial.write('.');
      delay(300);
    }
  }
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println(".");
    Serial.print("Connected to ");
    Serial.println(SSID);
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
    Serial.println("Connected IP address: " + WiFi.localIP().toString() + " or <a href='http://" + hostname + ".local' >" + hostname + ".local</a>");
  } else {
    Serial.println("Can not connect to WiFi station. Go into AP mode.");
    // Go into software AP mode.
    WiFi.mode(WIFI_AP);
    delay(10);
    WiFi.softAP(ap_default_ssid, ap_default_psk);
    Serial.print("Access Point Mode. IP address: ");
    Serial.println(WiFi.softAPIP());
  }
}

void InitInverters(std::vector<DanfossInverter*> *_InverterList,std::vector<InverterConfigElement*> *_InverterMEMList) {
  InverterConfigElement *MemInv;
	for(int i = 0; i < _InverterMEMList->size(); i++){

		// Get inverter from memory list
		MemInv = _InverterMEMList->at(i);
    if (MemInv->InvType == "TLX") {
      _InverterList->push_back(new DanfossTLX(&MyComLynx, MemInv->Address, &MemInv->SerialNumber, &MemInv->ProductNumber));
      Serial.print("New TLX inverter initialized\n");
    } else {
      if (MemInv->InvType == "ULX") {
        _InverterList->push_back(new DanfossULX(&MyComLynx, MemInv->Address, &MemInv->SerialNumber, &MemInv->ProductNumber));
        Serial.print("New ULX inverter initialized\n");
      } else { Serial.print("Unknown interver type !"); }
    }
	}
}

void setup()
{
  Serial.begin(115200) ;  // Debug console
  delay(800);
  Serial.println("Booting");
  Serial.println(String(Program) + " " + String(Version));
  Serial.println(String(Board));
  Serial.println(String("Compile: ") + String(__DATE__) + " " + String(__TIME__));
  startMillis = millis();

  // myInverter = new DanfossTLX(&MyComLynx, "0224");

  // Configure hostname
  // **************
  uint32_t chipId = 0;
  for (int i = 0; i < 17; i = i + 8) {
    chipId |= ((ESP.getEfuseMac() >> (40 - i)) & 0xff) << i;
  }
  hostname += String(chipId);  //Add chip ID to hostname

  // get wifi parameters
  Serial.println("Retrieve wifi parameters");
  GetWifiParameters();

  // get configuration settings
  Serial.println("Retrieve config parameters");
  GetConfigParameters();

  // Configure WIFI
  // **************
  WiFi.hostname(hostname);
  Serial.println(hostname);
  ap_default_ssid = (const char *)hostname.c_str();
  // Check WiFi connection
  // ... check mode
  if (WiFi.getMode() != WIFI_STA) {
    WiFi.mode(WIFI_STA);
    delay(10);
  }

  // get wifi parameters
  Serial.println("connect Wifi");
  EstablishWifiConnect();
  Debug.begin("Debug started");

  GetInvertersFromEprom(&InverterMemList);
  InitInverters(&mypInverterList, &InverterMemList);

  InverterScan = new boolean(false);
  ScanPercent = new float(0);

  Serial.println("Starting webserver");
  // initalize http server
  server = new AsyncWebServer(HTTPPort);
  Init_Webserver();

  ArduinoOTA.setHostname((const char *)hostname.c_str());
  ArduinoOTA.begin();  //Mandatory

  MQTTClient.begin(WifiMqttClient);

  if (MQTTOn) {
    for(int i = 0; i < mypInverterList.size(); i++){
      myComLynxInverter = mypInverterList.at(i);
      MQTTDiscovery(myComLynxInverter);
    }
  }

  Serial.println("Boot successfull");
}

void loop()
{
  //const unsigned long  TIMETLX   = 800; // 36 par x 2 times a minute -> ~830ms each (Not to block server.handleClient();
  const unsigned long  TIMEPRINT = 60000;
  static unsigned long TimeTLX = millis();
  static unsigned long TimePrint = millis();

  //server.handleClient();
  ArduinoOTA.handle();
  Debug.handle();

  // Wraparround every ~50 days;
  // Should not be necessary after change to unsigned long
  if (millis() < TimeTLX){
    TimeTLX = millis();
  }
 
  if (millis() - TimeTLX >  ClxInterval){
    for(int i = 0; i < mypInverterList.size(); i++){

      // Get inverter from list
      myComLynxInverter = mypInverterList.at(i);

      // Serial.print ("NumberOfParameters : ");
      // Serial.println (mypInverterList.at(i)->PrintParametersCount());
      //mypInverterList.at(i)->FakeGetStatus();
      // Serial.print ("Type : ");
      // Serial.println (mypInverterList.at(i)->PrintType());
      // Serial.print ("SN : ");
      // Serial.println (mypInverterList.at(i)->PrintSN());
      // mypInverterList.at(i)->GetName();
      // Serial.print ("Name : ");
      // Serial.println (mypInverterList.at(i)->PrintName());
      // Serial.print ("Product Number : ");
      // Serial.println (mypInverterList.at(i)->PrintProductNumber());
      // Serial.print ("Operating Mode : ");
      // Serial.println (mypInverterList.at(i)->PrintOpMode());
      // Serial.print ("Parameter 1 name : ");
      // Serial.println (mypInverterList.at(i)->PrintNameParameter(1));
      // Serial.print ("Parameter 1 parname : ");
      // Serial.println (mypInverterList.at(i)->PrintParNameParameter(1));
      // Serial.print ("Parameter 1 unit : ");
      // Serial.println (mypInverterList.at(i)->PrintUnitParameter(1));
      // Serial.print ("Parameter 1 measure : ");
      // Serial.println (mypInverterList.at(i)->PrintMeasParameter(1));
      // Serial.print ("Parameter 1 formated measure : ");
      // Serial.println (mypInverterList.at(i)->PrintFormatedMeasParameter(1));

      //myComLynxInverter->GetRollingParameters();
      myComLynxInverter->GetAllParameters();
      //myComLynxInverter->GetOneParameter("TotalE");
      //myComLynxInverter->GetOneParameter("OpMode");
      if (MQTTOn) {
        SendMQTT(myComLynxInverter);
      }

    }
    //myDanfossULX.PrintAll();
    TimeTLX = millis();
  }
  if (millis() - TimePrint >  TIMEPRINT){
    //myDanfossULX.PrintAll();
    TimePrint = millis();
  }
  
  // Test if connected
  if(WiFi.getMode() == WIFI_STA && WiFi.status() != WL_CONNECTED){
    Serial.println("Restart Wifi");
    WiFi.disconnect();
    EstablishWifiConnect();
  }
  delay(2);
}

void ScanComlynxBus( void * parameter )
{
  mypInverterList.clear();
  InverterMemList.clear();
  MyComLynx.DiscoverInverters(&InverterMemList, InverterScan, ScanPercent);
  vTaskDelete( NULL );
}

