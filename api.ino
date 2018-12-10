#include "Espapi.h"

#define SEND "send"
#define AMAP "amap"
#define SNIFF "sniff"
#define PING "ping"
#define DEFAULT_CHANNEL 11
#define PASSWD "Dan Brown Da Vinci Code"
#define SSID "D. Arthur"//"Lambs to the cosmic slaughter"

//IPAddress apIP(192, 168, 4, 1);
//IPAddress netMsk(255, 255, 255, 0);
Espapi api;

void setup() {
    Serial.begin(115200);

    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(13, HIGH);

    WiFi.mode(WIFI_OFF);
    wifi_set_opmode(STATION_MODE);
    wifi_set_promiscuous_rx_cb([](uint8_t* buf, uint16_t len){ api.handler(buf, len); });
    api.setChannel(DEFAULT_CHANNEL);


    //WiFi.softAPConfig(apIP, apIP, netMsk);
    ap.startAP(SSID, PASSWD, 11, false);
    delay(100);
}

void loop() {
    if (Serial.available() > 0) {

        char command[256];
        command[Serial.readBytesUntil('\n', command, 256)] = '\0';

        char * pch;
        pch = strtok(command, " ");
        if (pch != NULL) { send(pch, command); }

        while (api.writeQueue.size() > 0) {
          String output = api.writeQueue.at(0);
          api.writeQueue.erase(ap.writeQueue.front, ap.writeQueue.front + 1);

          Serial.print("\x02");
          Serial.print(output);
          Serial.print("\x03");
        }
    }
}

void send(char* cmd, char* args) {

    int channel = DEFAULT_CHANNEL;
    uint8_t* data;
    char* arg;
    boolean stop = false;

    //No cmd provided
    if (!cmd || cmd == "") { return; }

    //Not enough args
    if (strcmp(cmd, SEND) == 0 && args == "" || args == NULL) { return; }

    arg = strtok(args, " ");

    //Find the args if any
    while (arg != NULL) {
        if (strcmp(arg, "-c") == 0) {
            channel = (int)strtok(NULL, " ");
        } else if (strcmp(arg, "-d") == 0) {
            data = (uint8_t*)strtok(NULL, " ");
        } else if (strcmp(arg, "-s") == 0) {
            stop = true;
        }

        arg = strtok(NULL, " ");
    }

    api.setChannel(channel);

    //Send command
    if (strcmp(cmd, PING) == 0) {
        Serial.write("\x02{type:\"test\",data:\"pong\"}\x03");
    } else if (strcmp(cmd, AMAP) == 0) {
        api.amap(false, true);
    } else if (strcmp(cmd, SNIFF) == 0) {
        if (stop) { api.sniff}
        api.sniff();
    } else if (strcmp(cmd, SEND) == 0) {
        api.send(data);
    }
}
