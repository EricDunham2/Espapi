#ifndef Espapi_h
#define Espapi_h

#include <vector>
#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <string.h>

#include "esp_wifi_types.h"
extern "C" {
    #include "user_interface.h"
    typedef void( * freedom_outside_cb_t)(uint8 status);
    int wifi_register_send_pkt_freedom_cb(freedom_outside_cb_t cb);
    void wifi_unregister_send_pkt_freedom_cb(void);
    int wifi_send_pkt_freedom(uint8 * buf, int len, bool sys_seq);
}

//using namespace std;

#define ATTEMPTS 50
#define ETH_MAC_LEN 6
#define MAC_FMT "%02x:%02x:%02x:%02x:%02x:%02x"

class Espapi {
    public :
        //Espapi();
        void startAP();
        void stopAP();
        void setMode(WiFiMode_t mode);
        void amap(bool async, bool hidden);
        void send(uint8_t* packet);
        void setChannel(int ch);
        void sniff();
        void handler(uint8_t *buffer, uint16_t length);

        vector<String> writeQueue;
        boolean sniffing = false;

        char* ssid;
        char* passwd;
        int channel;
        boolean hidden;

};

#endif
