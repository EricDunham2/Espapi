
#ifndef Espapi_h
#define Espapi_h

#include <Arduino.h>
#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <string.h>
#include <Vector.h>

#include "esp_wifi_types.h"
extern "C" {
    #include "user_interface.h"
    typedef void( * freedom_outside_cb_t)(uint8 status);
    int wifi_register_send_pkt_freedom_cb(freedom_outside_cb_t cb);
    void wifi_unregister_send_pkt_freedom_cb(void);
    int wifi_send_pkt_freedom(uint8 * buf, int len, bool sys_seq);
}

#define ATTEMPTS 50
#define ETH_MAC_LEN 6
#define MAC_FMT "%02x:%02x:%02x:%02x:%02x:%02x"

class Espapi {
    public:
        Vector<String> writeQueue;
        Vector<uint8_t*> readQueue;
        //Accesspoint Properties
        const char* ssid;
        const char* passwd;
        int channel;
        boolean hidden;

        //Api state peroperties
        boolean accesspointScanning = false;
        boolean stationScanning = false;
        unsigned long scanInterval = 5000;
        unsigned long scanStartTime;

        //Methods
        //Accesspoint functions
        void startAP();
        void startAP(const char* _ssid, const char* _passwd, int _channel, boolean _hidden);
        void stopAP();

        void setAccesspointScanning(boolean isScanning);
        void setStationScanning(boolean isScanning);
        void setChannel(int ch);
        void setMode(WiFiMode_t mode);
        void setScanInterval(unsigned long interval);
        void handler(uint8_t* buf, uint16_t len);

        void update();

        void amap(bool async, bool hidden);
        void send(uint8_t* packet);
        void startPacketSniff();
        void stopPacketSniff();
};

#endif
