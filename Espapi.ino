#include "Espapi.h"
#include <Vector.h>
#include <string>
//Commands
#define PING "ping"
#define SETUP "setup"
#define SEND "send"
#define SCAN "scan"
#define SNIFF "sniff"

//Arguments
#define ASYNC "async"
#define HIDDEN "hidden"
#define HOP "hop"
#define CHANNEL "channel"
#define INTERVAL "interval"
#define BUFFER "buffer"
#define PASSWORD "password"
#define SSID "ssid"

//Defaults
#define DEFAULT_CHANNEL 11
#define DEFAULT_PASSWD "Mortison D"
#define DEFAULT_SSID "Lans To The Cosmic Slaughter"//"Lambs to the cosmic slaughter"

IPAddress apIP(192, 168, 4, 1);
IPAddress netMsk(255, 255, 255, 0);

bool async = false;
bool scanHidden = false;
bool apHidden = false;
bool hop = false;
int channel = DEFAULT_CHANNEL;
int interval = 1000;
uint8_t *buffer;
String password = DEFAULT_PASSWD;
String ssid = DEFAULT_SSID;

using namespace std;

Espapi api;

Vector<String> split(char* str, char* delimiter) {
  Vector<String> tokens;
  char * token;

  token = strtok(str, delimiter);

  while (token != NULL) {
    tokens.push_back((String)token);
    token = strtok(NULL, delimiter);
  }

  return tokens;
}

void setup() {
  Serial.begin(115200);

  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(13, LOW);

  WiFi.mode(WIFI_OFF);
  wifi_set_opmode(STATION_MODE);

  wifi_set_promiscuous_rx_cb([](uint8_t* buf, uint16_t len) {
    api.handler(buf, len);
  });

  api.setChannel(DEFAULT_CHANNEL);

  WiFi.softAPConfig(apIP, apIP, netMsk);

  api.startAP(ssid.c_str(), password.c_str(), 11, false);
  delay(100);
}

void loop() {
  digitalWrite(13, LOW);
  api.update();
  digitalWrite(13, HIGH);

  char data[256];
  data[Serial.readBytesUntil('\n', data, 256)] = '\0';

  send(data);
}

void send(char* data) {
 
  char delimS[2] = " ";
  Vector<String> parts = split(data, delimS);
  
  String strCmd = parts.at(0);

  char command[strCmd.length() + 1];
  strcpy(command, strCmd.c_str());

  parts.remove(0);

  while (parts.size() > 0) {
    String part = parts.at(0);
    parts.remove(0);

  char delimS[2] = " ";
  Vector<String> subParts = split(data, delimS);

    String argStr = subParts.at(0);

    char arg[argStr.length() + 1];
    strcpy(arg, argStr.c_str());

    if (strcmp(arg, ASYNC) == 0) {
      async = toBool(subParts.at(1));
    } else if (strcmp(arg, HIDDEN) == 0) {
      if (strcmp(command, SCAN)) {
        scanHidden = toBool(subParts.at(1));
      } else {
        apHidden = toBool(subParts.at(1));
      }
    } else if (strcmp(arg, HOP) == 0) {
      hop = toBool(subParts.at(1));
    } else if (strcmp(arg, CHANNEL) == 0) {
      channel =  subParts.at(1).toInt();
    } else if (strcmp(arg, INTERVAL) == 0) {
      interval = subParts.at(1).toInt();
    } else if (strcmp(arg, BUFFER) == 0) {
      buffer = (uint8_t*)subParts.at(1).toInt();
    } else if (strcmp(arg, PASSWORD) == 0) {
      password = subParts.at(1);
    } else if (strcmp(arg, SSID) == 0) {
      ssid = subParts.at(1);
    }
  }

  api.setChannel(channel);

  if (strcmp(command, PING) == 0) {
    api.writeQueue.push_back("\x02{type:\"info\",data:\"pong\"}\x03");
  } else if (strcmp(command, SETUP) == 0) {
    api.stopAP();
    api.startAP(ssid.c_str(), password.c_str(), channel, apHidden);
  } else if (strcmp(command, SEND) == 0) {
    if ((int)sizeof(buffer) == 0) {
      api.writeQueue.push_back("\x02{type:\"info\",data:\"Packet data must be sent for this command\n send -buffer=[byte array]\"}\x03");
      return;
    }
    api.send(buffer);
  } else if (strcmp(command, SCAN) == 0) {
    api.amap(async, scanHidden);
  } else if (strcmp(command, SNIFF) == 0) {
    api.startPacketSniff();
  }
}

void printOutput() {
  while (api.writeQueue.size() > 0) {
    String output = api.writeQueue.at(0);
    api.writeQueue.remove(0);

    Serial.print("\x02");
    Serial.print(output);
    Serial.print("\x03");
  }
}

bool toBool(String boolStr) {
  boolStr.toLowerCase();
  

  if (strcmp(boolStr.c_str(), "false") == 0) {
    return false;
  } else if (strcmp(boolStr.c_str(), "true") == 0) {
    return true;
  }

  return false;
}

/*
  Commands
    scan - Finds accesspoints
        Augments:
            -async=true/false
            -hidden=true/false
            -channel=1-12
            -hop=true/false
    sniff - Turns packet sniffer on
        Augments:
            -interval=1000
            -channel=1-12
            -hop=true/false
    send - Sends packet buffer
        Augments:
            -interval=1000
            -channel=1-12
            -buffer=[Data Array]
    setup - Sets up the accesspoint
        Augments:
            -channel=1-12
            -hidden=true/false
            -password=Something
            -ssid=Something
    ping - Test connection
*/
