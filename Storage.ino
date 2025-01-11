/*
    Name: Storage
    Used to store config and inverters in eprom
    
*/ 

#include <Preferences.h>
#include "InverterConfigElement.h"

Preferences preferences;

void SaveConfigParameters(void) {
  preferences.begin("Settings", false);
  preferences.putBool("dhcp", dhcpOn); 
  preferences.putULong("StaticIP", (uint32_t) StaticIP);
  preferences.putULong("Gateway", (uint32_t) Gateway);
  preferences.putULong("Netmask", (uint32_t) Netmask);
  preferences.putULong("primaryDNS", (uint32_t) primaryDNS);
  preferences.putBool("mqtt", MQTTOn);
  preferences.putBool("RestAPI", RestAPIOn);
  preferences.putULong("ClxInterval", ClxInterval);
  preferences.putULong("MQTTIP", (uint32_t) MQTTIP);
  preferences.putUInt("MQTTPort", MQTTPort);
  preferences.putString("MQTTUser", MQTTUser); 
  preferences.putString("MQTTPwd", MQTTPwd); 
  preferences.putString("MQTTPrefix", MQTTPrefix);
  preferences.putBool("MQTTDiscOn", MQTTDiscOn);
  preferences.putString("MQTTdeviceName", MQTTdeviceName);
  preferences.putUInt("HTTPPort", HTTPPort);
  String msg = "All config settings saved to eprom";
  Serial.println(msg);
  Debug.println(msg);
  preferences.end();
}

void ResetConfigParameters(void) {
  preferences.begin("Settings", false);
  preferences.clear();
  String msg = "settings eprom deleted";
  Serial.println(msg);
  Debug.println(msg);
  preferences.end();
}

void GetConfigParameters(void) {
  preferences.begin("Settings", false);
  dhcpOn = preferences.getBool("dhcp", true); 
  StaticIP = preferences.getULong("StaticIP", 4110526656); // 192.168.1.245
  Gateway = preferences.getULong("Gateway", 4261521600); // 192.168.1.254
  Netmask = preferences.getULong("Netmask", 16777215); // 255.255.255.0
  primaryDNS = preferences.getULong("primaryDNS", 4261521600); // 192.168.1.254
  MQTTOn = preferences.getBool("mqtt", false);
  RestAPIOn = preferences.getBool("RestAPI", false);
  ClxInterval = preferences.getULong("ClxInterval", 30000);
  MQTTIP = preferences.getULong("MQTTIP", 302098624);  // 192.168.1.18
  MQTTPort = preferences.getUInt("MQTTPort", 1883);
  MQTTUser = preferences.getString("MQTTUser", "User"); 
  MQTTPwd = preferences.getString("MQTTPwd", "password"); 
  MQTTPrefix = preferences.getString("MQTTPrefix", "homeassistant");
  MQTTDiscOn = preferences.getBool("MQTTDiscOn", false);
  MQTTdeviceName = preferences.getString("MQTTdeviceName", hostname);
  HTTPPort = preferences.getUInt("HTTPPort", 80);
  String msg = "All config settings retrieved from eprom";
  Serial.println(msg);
  Debug.println(msg);

  preferences.end();
}

void GetWifiParameters(void) {
  preferences.begin("Wificredentials", false);
  SSID = preferences.getString("ssid", ""); 
  SECRET_WIFI_PSWD = preferences.getString("password", "");
  String msg = "Network Credentials retrieved in eprom";
  Serial.println(msg);
  Debug.println(msg);
  preferences.end();
}

void SaveWifiParameters(void) {
  preferences.begin("Wificredentials", false);
  preferences.putString("ssid", SSID); 
  preferences.putString("password", SECRET_WIFI_PSWD);

  String msg = "Network Credentials saved in eprom";
  Serial.println(msg);
  Debug.println(msg);

  preferences.end();
}

void GetInvertersFromEprom(std::vector<InverterConfigElement*> *_InverterCEList) {
  int _InvId;
  String _InvType;
  String _InvProductNumber;
  String _InvSerialNumber;
  String _InvAddress;
  byte   _OptionByte = 0;
  const char* prefName;
  _InverterCEList->clear();
  preferences.begin("Inverters", false);
  int InvNb = preferences.getUInt("InvNb", 0);

  for (int i = 0; i < InvNb; i++) {
    _InvId = i + 1;
    prefName = "InvTp" + _InvId;
    _InvType = preferences.getString(prefName, "");
    prefName = "InvPN" + _InvId;
    _InvProductNumber = preferences.getString(prefName, "");
    prefName = "InvSN" + _InvId;
    _InvSerialNumber = preferences.getString(prefName, "");
    prefName = "InvAd" + _InvId;
    _InvAddress = preferences.getString(prefName, "");
    prefName = "IOptB" + _InvId;
    preferences.getBytes(prefName, &_OptionByte, sizeof(_OptionByte));
    if ((_InvType == "TLX" || _InvType == "ULX") && _InvProductNumber != "" && _InvSerialNumber != "" && _InvAddress != "") {
      String msg = "Inverter " + String(_InvId) + " restored from eeprom";
      Serial.println(msg);
      Debug.println(msg);
      _InverterCEList->push_back(new InverterConfigElement(_InvId, _InvType, _InvProductNumber, _InvSerialNumber, _InvAddress, _OptionByte));
    } else {
      String msg = "Invalid inverters parameters preference will be cleared";
      Serial.println(msg);
      Debug.println(msg);
      preferences.clear();
    }
  }
  preferences.end();
}

void SaveInvertersToEprom(std::vector<InverterConfigElement*> *_InverterCEList) {
  InverterConfigElement  *MemInv;
  int _InvId;
  int InvNb = _InverterCEList->size();
  const char* prefName;

  preferences.begin("Inverters", false);
  preferences.clear();

  if (InvNb > 0) { 
    preferences.putUInt("InvNb",InvNb);
    for(int i = 0; i < InvNb; i++){
      _InvId = i + 1;
      MemInv = _InverterCEList->at(i);
      prefName = "InvTp" +_InvId;
      preferences.putString(prefName, MemInv->InvType);
      prefName = "InvPN" + _InvId;
      preferences.putString(prefName, MemInv->ProductNumber);
      prefName = "InvSN" + _InvId;
      preferences.putString(prefName, MemInv->SerialNumber);
      prefName = "InvAd" + _InvId;
      preferences.putString(prefName, MemInv->Address);
      if (MemInv->InvType == "TLX" && MemInv->OptionByte != 0) {
        prefName = "IOptB" + _InvId;
        preferences.putBytes(prefName, &MemInv->OptionByte, sizeof(MemInv->OptionByte) );
      }
      String msg = "Inverter " + String(_InvId) + " saved to eeprom";
      Serial.println(msg);
      Debug.println(msg);
    }
  }
  preferences.end();
}