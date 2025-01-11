// **********************************************************************************************
// *                        MQTT AUTO-DISCOVERY for HOME ASSISTANT                              *
// **********************************************************************************************

const char *SSR = "sensor"; // all inverters parameter's types will be : sensor

void MQTTconnect() {  
  if (!MQTTClient.connected()) {  // Test if not already connected
    Serial.println("Connecting to MQTT Server ...");
    MQTTClient.setHost(MQTTIP, MQTTPort);
    MQTTClient.onMessage(messageReceived);
    if (MQTTClient.connect(MQTTdeviceName.c_str(), MQTTUser.c_str(), MQTTPwd.c_str())) {
      Serial.println(MQTTdeviceName + " connected");
      Debug.println(MQTTdeviceName + " connected");
    } else {
      Serial.println("connection to MQTT failed.");
      Debug.println("connection to MQTT failed.");
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
//*          CONFIG OF DISCOVERY MESSAGE FOR HOME ASSISTANT               *
//*************************************************************************
void sendMQTTDiscoveryMsg(DanfossInverter *MQTTInverter) {

  String Class;
  String Round;
  bool checked = true;
  if (MQTTClient.connected()) {
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
        DeviceToDiscover(MQTTInverter, MQTTInverter->PrintNameParameter(i), MQTTInverter->PrintParNameParameter(i), MQTTInverter->PrintUnitParameter(i), Class, Round);
      } else {
        if (MQTTInverter->PrintParNameParameter(i) == "OpMode") {
          DeviceTextToDiscover(MQTTInverter, MQTTInverter->PrintNameParameter(i), MQTTInverter->PrintParNameParameter(i));
        }
      }
    }
    Serial.println("Auto-Discovery parameters published !");
    Debug.println("Auto-Discovery parameters published !");
  }
}

void DeviceToDiscover(DanfossInverter *MQTTInverter, String Name, String ParName, String Unit, String Class, String Round) {
  
  DynamicJsonDocument doc(768);  // this is the Payload json format
  JsonObject device;             // for device object     "device": {}
  JsonObject origin;             // for origin object     "origin": {}
  JsonObject components;         // for components object "components": {}
  JsonObject component;          // for component object  "component": {}
  char buffer[768];
  size_t n;
  bool published;
  String SN = MQTTInverter->PrintSN();
  String INVtype = MQTTInverter->PrintType();
  String HAdeviceName = INVtype + "_" + SN;
  String StateTopic = String(MQTTPrefix) + "/" + HAdeviceName + "_state";
  String DiscoveryTopic = String(MQTTPrefix) + "/device/" + HAdeviceName + "/config";
 
  device = doc.createNestedObject("dev");
  device["ids"] = "INV_" + HAdeviceName;  // unique identifier for Home Assistant (mandatory).
  device["name"] = MQTTInverter->PrintName();
  device["mf"] = "Danfoss";
  device["mdl"] = INVtype;
  device["mdl_id"] = MQTTInverter->PrintProductNumber();
  device["sn"] = MQTTInverter->PrintSN();
  //device["sw"] = Version;
  device["hw"] = MQTTInverter->PrintProductNumber();

  origin = doc.createNestedObject("o");
  origin["name"] = hostname;
  origin["sw"] = Version;
  origin["url"] = "http://" + WiFi.localIP().toString();
  
  components = doc.createNestedObject("cmps");

  component = components.createNestedObject(HAdeviceName +  "_" + SN + "_" + ParName);
  component["p"] = String(SSR);
  component["name"] = Name;     // Entity name in HA
  component["uniq_id"] = HAdeviceName + "_" + ParName;  // HA entity Unique name
  component["unit_of_meas"] = Unit;
  if (Unit=="Wh" || Unit=="kWh"){
      component["state_class"] = "total_increasing";
  } else {
    component["state_class"] = "measurement";
  }
  component["device_class"] = Class;
  component["val_tpl"] = "{{ value_json." + ParName + "|default(0)| round(" + Round + ") }}";
  doc["stat_t"] = StateTopic;

  n = serializeJson(doc, buffer);
  published = MQTTClient.publish(DiscoveryTopic.c_str(), buffer, n);
  doc.clear();
  buffer[0] = '\0';
}

void DeviceTextToDiscover(DanfossInverter *MQTTInverter, String Name, String ParName) {
  DynamicJsonDocument doc(512);  // this is the Payload json format
  JsonObject device;             // for device object     "device": {}
  JsonObject origin;             // for origin object     "origin": {}
  JsonObject components;         // for components object "components": {}
  JsonObject component;          // for component object  "component": {}
  char buffer[512];
  size_t n;
  bool published;
  String Titre;

  String SN = MQTTInverter->PrintSN();
  String INVtype = MQTTInverter->PrintType();
  String HAdeviceName = INVtype + "_" + SN;
  String StateTopic = String(MQTTPrefix) + "/" + HAdeviceName + "_state";
  String DiscoveryTopic = String(MQTTPrefix) + "/device/" + HAdeviceName + "/config";

  device = doc.createNestedObject("dev");
  device["ids"] = "INV_" + HAdeviceName;  // unique identifier for Home Assistant (mandatory).
  device["name"] = MQTTInverter->PrintName();
  device["mf"] = "Danfoss";
  device["mdl"] = INVtype;
  device["mdl_id"] = MQTTInverter->PrintProductNumber();
  device["sn"] = MQTTInverter->PrintSN();
  //device["sw"] = Version;
  device["hw"] = MQTTInverter->PrintProductNumber();

  origin = doc.createNestedObject("o");
  origin["name"] = hostname;
  origin["url"] = "http://" + WiFi.localIP().toString();
  origin["sw"] = Version;
  
  components = doc.createNestedObject("cmps");

  component = components.createNestedObject(String(HAdeviceName) + "_" + Name);
  component["p"] = String(SSR);
  component["name"] = Name;    // Name of entity
  component["uniq_id"] = HAdeviceName + "_" + ParName;  // Object unique name
  component["device_class"] = "enum";
  component["val_tpl"] = "{{ value_json." + ParName + " }}";

  doc["stat_t"] = StateTopic;

  n = serializeJson(doc, buffer);
  published = MQTTClient.publish(DiscoveryTopic.c_str(), buffer, n);
  doc.clear();
  buffer[0] = '\0';
}

void SendDataToHomeAssistant(DanfossInverter *MQTTInverter) {
  DynamicJsonDocument doc(1024);
  char buffer[1024];
  String SN = MQTTInverter->PrintSN();
  String INVtype = MQTTInverter->PrintType();
  String HAdeviceName = INVtype + "_" + SN;
  String StateTopic = String(MQTTPrefix) + "/" + HAdeviceName + "_state";

  for (int i = 0; i < MQTTInverter->PrintParametersCount(); i++) {
    if (MQTTInverter->PrintParNameParameter(i) == "OpMode") {
      doc[MQTTInverter->PrintParNameParameter(i)] = MQTTInverter->PrintOpMode();
    } else  {
      doc[MQTTInverter->PrintParNameParameter(i)] = MQTTInverter->PrintMeasParameter(i);
    }
  }
  size_t n = serializeJson(doc, buffer);
  bool published = MQTTClient.publish(StateTopic.c_str(), buffer, n);
  doc.clear();
  buffer[0] = '\0';

}