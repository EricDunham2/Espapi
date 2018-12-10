#include "Espapi.h"

void Espapi::setMode(WiFiMode_t mode) {
    WiFi.mode(mode);
    WiFi.disconnect();
}

void Espapi::startAP() {
    startAP(ssid, passwd, channel, hidden);
}

void Espapi::startAP(char* _ssid, char* _passwd, int _channel, boolean _hidden) {
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

void Espapi::setChannel(int ch) {
    wifi_set_channel(ch);
}

void Espapi::sniff() {
    stopAp();
    wifi_promiscuous_enable(false);

    if (sniffing) { 
        wifi_promiscuous_enable(true);
    }

    startAP();
}

void Espapi::handler(uint8_t* buf, uint16_t len) {
    //wifi_promiscuous_pkt_type_t type = (wifi_promiscuous_pkt_type_t *)len;

    //if (type != WIFI_PKT_MGMT) { return; }
    StaticJsonBuffer<2048> jsonBuffer;

    const wifi_promiscuous_pkt_t *ppkt = (wifi_promiscuous_pkt_t *)buf;
    const wifi_ieee80211_packet_t *ipkt = (wifi_ieee80211_packet_t *)ppkt->payload;
    const wifi_ieee80211_mac_hdr_t *hdr = &ipkt->hdr;

    unsigned rssi = ppkt->rx_ctrl.rssi;
    int channel = ppkt->rx_ctrl.channel;
    String pkt_type = "MGMT";//wifi_sniffer_packet_type2str(type);
    char addr1[ETH_MAC_LEN];
    char addr2[ETH_MAC_LEN];
    char addr3[ETH_MAC_LEN];

    sprintf(
        addr1,
        MAC_FMT,
        hdr->addr1[0],
        hdr->addr1[1],
        hdr->addr1[2],
        hdr->addr1[3],
        hdr->addr1[4],
        hdr->addr1[5]
    );

    sprintf(
        addr2,
        MAC_FMT,
        hdr->addr2[0],
        hdr->addr2[1],
        hdr->addr2[2],
        hdr->addr2[3],
        hdr->addr2[4],
        hdr->addr2[5]
    );

    sprintf(
        addr3,
        MAC_FMT,
        hdr->addr3[0],
        hdr->addr3[1],
        hdr->addr3[2],
        hdr->addr3[3],
        hdr->addr3[4],
        hdr->addr3[5]
    );

    JsonObject &pktJson = jsonBuffer.createObject();

    pktJson["data_type"] = "packet";
    pktJson["pkt_type"] = pkt_type;
    pktJson["rssi"] = rssi;
    pktJson["channel"] = channel;
    pktJson["addr1"] = addr1;
    pktJson["addr2"] = addr2;
    pktJson["addr3"] = addr3;

    String out;
    pktJson.printTo(out);
    writeQueue.push_back(out);
}


const char * Espapi::wifi_sniffer_packet_type2str(wifi_promiscuous_pkt_type_t type)
{
    switch(type) {
        case WIFI_PKT_MGMT: 
            return "MGMT";
        case WIFI_PKT_DATA: 
            return "DATA";
        default:
            case WIFI_PKT_MISC: 
                return "MISC";
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
