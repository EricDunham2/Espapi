#include "Espapi.h"
#include <Vector.h>
/************************************
 *      Acesspoint Functions       *
 ***********************************/

void Espapi::startAP() {
    startAP(ssid, passwd, 11, hidden);
}

void Espapi::startAP(const char* _ssid, const char* _passwd, int _channel, boolean _hidden) {
    WiFi.softAP(_ssid, _passwd, _channel, _hidden);

    ssid = _ssid;
    passwd = _passwd;
    channel = _channel;
    hidden = _hidden;
}

void Espapi::stopAP() {
    wifi_promiscuous_enable(0);

    WiFi.persistent(false);
    WiFi.disconnect(true);

    wifi_set_opmode(STATION_MODE);
}



/************************************
 *      Configuration Functions     *
 ***********************************/

void Espapi::setAccesspointScanning(boolean isScanning) {
    accesspointScanning = isScanning;
}

void Espapi::setStationScanning(boolean isScanning) {
    stationScanning = isScanning;
}

void Espapi::setChannel(int ch) {
    wifi_set_channel(ch);
}

void Espapi::setMode(WiFiMode_t mode) {
    WiFi.mode(mode);
    WiFi.disconnect();
}

void Espapi::setScanInterval(unsigned long interval) {
    scanInterval = interval;
}

void Espapi::handler(uint8_t *buffer, uint16_t length) {
    StaticJsonBuffer<2048> jsonBuffer;
    JsonObject &pktJson = jsonBuffer.createObject();

    struct SnifferPacket *snifferPacket = (struct SnifferPacket*) buffer;

    unsigned int frameControl = ((unsigned int)snifferPacket->data[1] << 8) + snifferPacket->data[0];
    
    uint8_t frameType = (frameControl & 0b0000000000001100) >> 2;
    uint8_t frameSubType = (frameControl & 0b0000000011110000) >> 4;

  if (frameType != TYPE_MANAGEMENT || frameSubType != SUBTYPE_PROBE_REQUEST)
        return;

   //writeQueue.push_back((String)snifferPacket->data);

    char srcAddr[18] = "00:00:00:00:00:00";
    sprintf(srcAddr, "%02x:%02x:%02x:%02x:%02x:%02x", snifferPacket->data[10], snifferPacket->data[11], snifferPacket->data[12], snifferPacket->data[13], snifferPacket->data[14], snifferPacket->data[15]);

    char dstAddr[18] = "00:00:00:00:00:00";
    sprintf(dstAddr, "%02x:%02x:%02x:%02x:%02x:%02x", snifferPacket->data[4], snifferPacket->data[5], snifferPacket->data[6], snifferPacket->data[7], snifferPacket->data[8], snifferPacket->data[9]);

    char bssidAddr[18] = "00:00:00:00:00:00";
    sprintf(dstAddr, "%02x:%02x:%02x:%02x:%02x:%02x", snifferPacket->data[16], snifferPacket->data[17], snifferPacket->data[18], snifferPacket->data[19], snifferPacket->data[20], snifferPacket->data[21]);

  uint8_t SSID_length = snifferPacket->data[25];
    char ssid[33];
    ssid[0] = '\0';

  for(uint16_t i = 26; i < DATA_LENGTH && i < 26 + SSID_length; i++) {
    sprintf(ssid, "%s%c", ssid, snifferPacket->data[i]);
  }

    int rssi = snifferPacket->rx_ctrl.rssi;
    int channel = wifi_get_channel();

    pktJson["data_type"] = "packet";
    pktJson["rssi"] = rssi;
    pktJson["channel"] = channel;
    pktJson["pkt_type"] = "MGMT";
    pktJson["src"] = srcAddr;
    pktJson["dst"] = dstAddr;
    pktJson["bssid"] = bssidAddr;
    pktJson["ssid"] = ssid;

    String out;
    pktJson.printTo(out);
    writeQueue.push_back(out);
}

void Espapi::amap(bool async, bool hidden) {
    wifi_set_opmode(STATION_MODE);

    StaticJsonBuffer<2048> jsonBuffer;

    JsonObject &root = jsonBuffer.createObject();
    root["data_type"] = "accesspoint";
    JsonArray &data = root.createNestedArray("data");

    //params are async and show hidden
    int networks = WiFi.scanNetworks(async, hidden);

    for (int n = 0; n < networks; n++) {
        JsonObject &ap = jsonBuffer.createObject();

        ap["ssid"] = WiFi.SSID(n);
        ap["enc"] = WiFi.encryptionType(n);
        ap["rssi"] = WiFi.RSSI(n);
        ap["bssid"] = WiFi.BSSIDstr(n);
        ap["channel"] = WiFi.channel(n);
        ap["hidden"] = WiFi.isHidden(n);

        data.add(ap);
    }

    String out;
    root.printTo(out);
    writeQueue.push_back(out);
}

void Espapi::send(uint8_t *packet) {
    int packetSize = (int)(sizeof(packet) / sizeof(*packet));
    bool sent = wifi_send_pkt_freedom(packet, packetSize, 0);
    
    for (int i = 0; i < ATTEMPTS && !sent; i++)
        sent = wifi_send_pkt_freedom(packet, packetSize, 0) == 0;
}

void Espapi::startPacketSniff() {
    stopAP();
    wifi_promiscuous_enable(true);
    startAP();
}

void Espapi::stopPacketSniff() {
    stopAP();
    wifi_promiscuous_enable(false);
    startAP();
}

void Espapi::update() {

    if (accesspointScanning == true) {
        amap(false, true);
    }

    if (stationScanning == true) {
        scanStartTime = millis();
        startPacketSniff();
    }

    while (readQueue.size() > 0 && (scanStartTime - millis()) > scanInterval) {
        uint8_t *packet = readQueue.at(0);
        readQueue.remove(0);

        send(packet);
    }
}
/*void connectToWifi(char *ssid, char *password) {
    //Connects to the specified network
    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED)
    {
        delay(1000);
        Serial.println("Connecting..");
    }
}*/
