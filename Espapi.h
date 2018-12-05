#ifndef Espapi_h
#define Espapi_h

#include <StandardCplusplus.h>
#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <vector>
#include <iterator>
#include <string.h>

#include "esp_wifi_types.h"
extern "C" {
    #include "user_interface.h"
    typedef void( * freedom_outside_cb_t)(uint8 status);
    int wifi_register_send_pkt_freedom_cb(freedom_outside_cb_t cb);
    void wifi_unregister_send_pkt_freedom_cb(void);
    int wifi_send_pkt_freedom(uint8 * buf, int len, bool sys_seq);
}

using namespace std;

#define ATTEMPTS 50
#define ETH_MAC_LEN 6
#define MAC_FMT "%02x:%02x:%02x:%02x:%02x:%02x"

class Espapi {
    public :
        //Espapi();
        void setMode(WiFiMode_t mode);
        void amap(bool async, bool hidden);
        void send(uint8_t* packet);
        void setChannel(int ch);
        void sniff();
        void handler(uint8_t* buf, uint16_t len);
        const char * wifi_sniffer_packet_type2str(wifi_promiscuous_pkt_type_t type);
        vector<String> writeQueue;
};

#endif
