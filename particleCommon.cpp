// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.

#include "application.h"
#include "particleCommon.h"

//https://www.arduino.cc/en/Reference/For
void blinkLed(int pin, int blinkTime, int blinks) {
  while(blinks){
    digitalWrite(pin, HIGH);
    delay(blinkTime/2);
    digitalWrite(pin, LOW);
    delay(blinkTime/2);
    blinks = blinks - 1;
  }
}

/*
********************************************************************************
Cloud functions
********************************************************************************/
int getInfo(String command) {
  if(command == "wifi") {
    return 1;
  } else{
    return -1;
  }
}

//    Particle.subscribe("spark/", getPhotonInfo);
//    Particle.publish("spark/device/name");
//        antenna = WiFi.selectAntenna(ANT_INTERNAL);
//        antenna = WiFi.selectAntenna(ANT_EXTERNAL);

int getWiFiCredentials(){
  WiFiAccessPoint ap[5];
  int found = WiFi.getCredentials(ap, 5);
  for (int i = 0; i < found; i++) {
    Serial.print("ssid: ");
    Serial.println(ap[i].ssid);
    // security is one of WLAN_SEC_UNSEC, WLAN_SEC_WEP, WLAN_SEC_WPA, WLAN_SEC_WPA2
    Serial.print("security: ");
    Serial.println(ap[i].security);
    // cipher is one of WLAN_CIPHER_AES, WLAN_CIPHER_TKIP
    Serial.print("cipher: ");
    Serial.println(ap[i].cipher);
  }
  return 1;
}

int getPhotonIP(){
  IPAddress ip = WiFi.localIP();
  static char ipAddress[24] = "";
  // https://community.particle.io/t/network-localip-to-string-to-get-it-via-spark-variable/2581/5
  sprintf(ipAddress, "%d.%d.%d.%d", ip[0], ip[1], ip[2], ip[3]);
  return 1;
}

int getPhotonMAC(){
  byte mac[6];
  WiFi.macAddress(mac);
  static char macAddress[18] = "";
  sprintf(macAddress, "%02X:%02X:%02X:%02X:%02X:%02X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
  return 1;
}

int getPhotonSSID(){
  String SSID = String(WiFi.SSID());

  byte bssid[6];
  WiFi.BSSID(bssid);
  static char bssID[18] = "";
  sprintf(bssID, "%02X:%02X:%02X:%02X:%02X:%02X", bssid[0], bssid[1], bssid[2], bssid[3], bssid[4], bssid[5]);
  return 1;
}

// void getPhotonInfo(const char *topic, const char *data) {
//   String ant = String(antenna);
//
//   String netInfo = String::format("network:{ip:\"%s\", mac:\"%s\", SSID:\"%s\", BSSID:\"%s\", antenna:\"%s\"}", ipAddress, macAddress, SSID.c_str(), bssID, ant.c_str());
//
//   String devName = String(data);
//
//   String chnr = String(thingChannelNumber);
//   String rssi = String(rssiField);
//   String ping = String(pingField);
//   String devInfo = String::format("info:{ID:\"%s\", deviceName:\"%s\", chnr:\"%s\", rssiField:\"%s\", pingField:\"%s\"}", deviceID.c_str(), devName.c_str(), chnr.c_str(), rssi.c_str(), ping.c_str());
//
//   String infoData = String::format("{%s, %s}", devInfo.c_str(), netInfo.c_str());
//   Particle.publish("photonInfo", infoData, 60, PRIVATE);
// }

// Open a serial terminal and see the device name printed out
void photonsName(const char *topic, const char *data) {
  deviceName = String(data);
}
