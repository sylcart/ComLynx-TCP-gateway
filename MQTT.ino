// **********************************************************************************************
// *                        MQTT AUTO-DISCOVERY for HOME ASSISTANT or DOMOTICZ                            *
// **********************************************************************************************

// mandatory components type for HA Auto-Discovery.
const char *SSR = "sensor";
const char *SLCT = "select";
const char *NB = "number";
const char *BINS = "binary_sensor";
const char *SWTC = "switch";
const char *TXT = "text";


void MQTTconnect() {  
  if (!MQTTClient.connected()) {  // Test if not already connected
    Serial.println("Connecting to MQTT Server ...");
    MQTTClient.setHost(MQTTIP, MQTTPort);
    MQTTClient.onMessage(messageReceived);
    if (MQTTClient.connect(MQTTdeviceName.c_str(), MQTTUser.c_str(), MQTTPwd.c_str())) {
      Serial.println(MQTTdeviceName + " connected");
      Debug.println(MQTTdeviceName + " connected");
    } else {
      Serial.print("connection to MQTT failed.");
      Debug.print("connection to MQTT failed.");
      return;
    }
  }
  MQTTClient.loop();
}


void SendMQTT(DanfossInverter *MQTTInverter) {  
  MQTTconnect();
  SendDataToHomeAssistant(MQTTInverter);  // Send Payload to State topic
  MQTTClient.loop();
}

//Callback function
void messageReceived(String &topic, String &payload) {
  Serial.println("incoming: " + topic + " - " + payload);
  Serial.println("-------Unused function, broker send a mqtt message-----");
  Debug.println("-------Unused function, broker send a mqtt message-----");
}

void MQTTDiscovery(DanfossInverter *MQTTInverter) {  
  MQTTconnect();
  sendMQTTDiscoveryMsg(MQTTInverter);
  MQTTClient.loop();
}

//*************************************************************************
//*          CONFIG OF DISCOVERY MESSAGE FOR HOME ASSISTANT  / DOMOTICZ             *
//*************************************************************************


void sendMQTTDiscoveryMsg(DanfossInverter *MQTTInverter) {
  String SN = MQTTInverter->PrintSN();
  String Class;
  String Round;
  bool checked = true;
  for (int i = 0; i < MQTTInverter->PrintParametersCount(); i++) {
    if (MQTTInverter->PrintUnitParameter(i) == "A"){
      Class = "current";
      Round = "2";
    }else{
      if (MQTTInverter->PrintUnitParameter(i) == "W"){
        Class = "power";
        Round = "0";
      }else{
        if (MQTTInverter->PrintUnitParameter(i) == "V"){
          Class = "voltage";
          Round = "2";
        }else{
          if (MQTTInverter->PrintUnitParameter(i) == "kWh"){
            Class = "energy";
            Round = "2";
          }else{
            if (MQTTInverter->PrintUnitParameter(i) == "Hz"){
              Class = "frequency";
              Round = "2";
            }else{
              if (MQTTInverter->PrintUnitParameter(i) == "mA"){
                Class = "current";
                Round = "2";
              }else{
                if (MQTTInverter->PrintUnitParameter(i) == "°C"){
                  Class = "temperature";
                  Round = "2";
                }else{
                  if (MQTTInverter->PrintUnitParameter(i) == "W/m²"){
                    Class = "power";
                    Round = "0";
                  }else{
                    if (MQTTInverter->PrintUnitParameter(i) == "Wh/m²"){
                      Class = "energy";
                      Round = "0";
                    }else{
                      if (MQTTInverter->PrintParNameParameter(i) != "OpMode") {
                        Serial.println("Unknown UNit please check parameter : " + MQTTInverter->PrintNameParameter(i));
                        Debug.println("Unknown UNit please check parameter : " + MQTTInverter->PrintNameParameter(i));
                      }
                      checked = false;
                    }
                  }
                }
              }
            }
          }
        }
      }
    }
    if (checked) {
      DeviceToDiscover(SN + " " + MQTTInverter->PrintNameParameter(i), SN + "_" + MQTTInverter->PrintParNameParameter(i), MQTTInverter->PrintUnitParameter(i), Class, Round);
      } else {
        if (MQTTInverter->PrintParNameParameter(i) == "OpMode") {
          DeviceTextToDiscover(SN + "_" + MQTTInverter->PrintParNameParameter(i), SN + "_" + MQTTInverter->PrintNameParameter(i));
        }
      }
  }
  Serial.println("Auto-Discovery parameters published !");
  Debug.println("Auto-Discovery parameters published !");

}  // END OF sendMQTTDiscoveryMsg_global

void DeviceToDiscover(String Name, String UniqueName, String Unit, String Class, String Round) {

  String StateTopic = String(MQTTPrefix) + "/" + MQTTdeviceName + "_state";
  DynamicJsonDocument doc(768);  // this is the Payload json format
  JsonObject device;             // for device object  "device": {}
  JsonArray option;              // options (array) of this device
  char buffer[768];
  size_t n;
  bool published;

  String ESP_ID = WiFi.macAddress();  //The chip ID is essentially its MAC address.
  String DiscoveryTopic;
  //DiscoveryTopic = ConfigTopic(Name, SSR, "config");
  DiscoveryTopic = String(MQTTPrefix) + "/" + String(SSR) + "/" + String(MQTTdeviceName) + "_" + String(UniqueName) + "/" + String("config");

  doc["name"] = "Inv " + Name;     // Entity name in HA
  doc["uniq_id"] = String(MQTTdeviceName) + "_" + UniqueName;  // HA entity Unique name
  doc["stat_t"] = StateTopic;
  doc["unit_of_meas"] = Unit;
  if (Unit=="Wh" || Unit=="kWh"){
      doc["state_class"] = "total_increasing";
  } else {
    doc["state_class"] = "measurement";
  }
  
  doc["device_class"] = Class;
  doc["val_tpl"] = "{{ value_json." + UniqueName + "|default(0)| round(" + Round + ") }}";
  device = doc.createNestedObject("device");
  device["ids"][0] = ESP_ID;  // unique identifier for Home Assistant (mandatory).
  device["cu"] = "http://" + WiFi.localIP().toString();
  device["hw"] = String(ESP.getChipModel()) + " rev." + String(ESP.getChipRevision());
  device["mf"] = "Danfoss";
  device["mdl"] = "ESP32 - " + ESP_ID;
  device["name"] = hostname;
  device["sw"] = Version;

  n = serializeJson(doc, buffer);
  published = MQTTClient.publish(DiscoveryTopic.c_str(), buffer, n);
  doc.clear();
  buffer[0] = '\0';
}

void DeviceTextToDiscover(String Name, String Titre) {
  String StateTopic = String(MQTTPrefix) + "/" + MQTTdeviceName + "_state";
  DynamicJsonDocument doc(512);  // this is the Payload json format
  JsonObject device;             // for device object  "device": {}
  JsonArray option;              // options (array) of this device
  char buffer[512];
  size_t n;
  bool published;
  String ESP_ID = WiFi.macAddress();  //The chip ID is essentially its MAC address.
  String DiscoveryTopic;              // topic name for this sensor
  DiscoveryTopic = String(MQTTPrefix) + "/" + String(SSR) + "/" + String(MQTTdeviceName) + "_" + String(Name) + "/" + String("config");
  doc["name"] = "Inv " + Titre;    // Name of entity
  doc["uniq_id"] = String(MQTTdeviceName) + "_" + Name;  // Object unique name
  doc["stat_t"] = StateTopic;
  doc["device_class"] = "enum";
  doc["val_tpl"] = "{{ value_json." + Name + " }}";
  device = doc.createNestedObject("device");
  device["ids"][0] = ESP_ID;
  n = serializeJson(doc, buffer);
  published = MQTTClient.publish(DiscoveryTopic.c_str(), buffer, n);
  doc.clear();
  buffer[0] = '\0';
}

void SendDataToHomeAssistant(DanfossInverter *MQTTInverter) {
  //common state topic for all entities of this ESP32 Device
  String StateTopic = String(MQTTPrefix) + "/" + MQTTdeviceName + "_state";
  DynamicJsonDocument doc(1024);
  char buffer[1024];
  String SN = MQTTInverter->PrintSN();
  for (int i = 0; i < MQTTInverter->PrintParametersCount(); i++) {
    if (MQTTInverter->PrintParNameParameter(i) == "OpMode") {
      doc[SN + "_" + MQTTInverter->PrintParNameParameter(i)] = MQTTInverter->PrintOpMode();
    } else  {
      doc[SN + "_" + MQTTInverter->PrintParNameParameter(i)] = MQTTInverter->PrintMeasParameter(i);
    }
  }
  size_t n = serializeJson(doc, buffer);
  bool published = MQTTClient.publish(StateTopic.c_str(), buffer, n);
  doc.clear();
  buffer[0] = '\0';

}  // END SendDataToHomeAssistant