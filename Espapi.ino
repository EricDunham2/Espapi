#include "Espapi.h"

#define SEND "send"
#define AMAP "amap"
#define SNIFF "sniff"
#define PING "ping"
#define DEFAULT_CHANNEL 11

using namespace std;

//IPAddress apIP(192, 168, 4, 1);
//IPAddress netMsk(255, 255, 255, 0);
Espapi api;

void setup() {
    Serial.begin(115200);

    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(13, LOW);

    WiFi.mode(WIFI_OFF);
    wifi_set_opmode(STATION_MODE);

    wifi_set_promiscuous_rx_cb([](uint8_t* buf, uint16_t len){
        api.handler(buf, len);
    });

    api.setChannel(DEFAULT_CHANNEL);
    //WiFi.softAPConfig(apIP, apIP, netMsk);
    String ssid = "Lambs to the cosmic slaughter";
    String pass = "Rick And Mortison";

    api.startAP(ssid.c_str(), pass.c_str(), 11, false);
    delay(100);
}

void loop() {
    //if (Serial.available() > 0) {
        api.update();

        while (api.writeQueue.size() > 0) {
          String output = api.writeQueue.at(0);
          api.writeQueue.erase(api.writeQueue.begin(), api.writeQueue.begin() + 1);
          
          Serial.print("\x02");
          Serial.print(output);
          Serial.print("\x03");
        }

        char command[256];
        command[Serial.readBytesUntil('\n', command, 256)] = '\0';

        char * pch;
        pch = strtok(command, " ");
        
        if (pch != NULL) { send(pch, command); }

        digitalWrite(13, LOW);
    //}
}

void send(char* cmd, char* args) {
    uint8_t* data;
    char* arg;

    //No cmd provided
    if (!cmd || cmd == "") { return; }

    //Not enough args
    if (strcmp(cmd, SEND) == 0 && args == "" || args == NULL) { return; }

    arg = strtok(args, " ");

    //Find the args if any
    while (arg != NULL) {
        if (strcmp(arg, "-c") == 0) {
            int channel = (int)strtok(NULL, " ");
            api.setChannel(channel);
        } else if (strcmp(arg, "-d") == 0) {
            data = (uint8_t*)strtok(NULL, " ");
        } else if (strcmp(arg, "-i") == 0) {
            unsigned long interval = (unsigned long)strtok(NULL, " ");
            api.setScanInterval(interval);
        }

        arg = strtok(NULL, " ");
    }

    //Send command
    if (strcmp(cmd, PING) == 0) {
        Serial.write("\x02{type:\"test\",data:\"pong\"}\x03");
    } else if (strcmp(cmd, AMAP) == 0) {
        //add a start/stop param
        api.setAccesspointScanning(true);
    } else if (strcmp(cmd, SNIFF) == 0) {
        //add a start/stop param
        api.setStationScanning(true);
    } else if (strcmp(cmd, SEND) == 0) {
        api.readQueue.push_back(data);
    }
}
