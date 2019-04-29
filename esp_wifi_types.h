// Copyright 2015-2016 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.


#ifndef __ESP_WIFI_TYPES_H__
#define __ESP_WIFI_TYPES_H__

#define DATA_LENGTH           112

#define TYPE_MANAGEMENT       0x00
#define TYPE_CONTROL          0x01
#define TYPE_DATA             0x02
#define SUBTYPE_PROBE_REQUEST 0x04

struct RxControl {
	signed rssi:8; // signal intensity of packet
	unsigned rate:4;
	unsigned is_group:1;
	unsigned:1;
	unsigned sig_mode:2; // 0:is 11n packet; 1:is not 11n packet;
	unsigned legacy_length:12; // if not 11n packet, shows length of packet.
	unsigned damatch0:1;
	unsigned damatch1:1;
	unsigned bssidmatch0:1;
	unsigned bssidmatch1:1;
	unsigned MCS:7; // if is 11n packet, shows the modulation and code used (range from 0 to 76)
	unsigned CWB:1; // if is 11n packet, shows if is HT40 packet or not
	unsigned HT_length:16;// if is 11n packet, shows length of packet.
	unsigned Smoothing:1;
	unsigned Not_Sounding:1;
	unsigned:1;
	unsigned Aggregation:1;
	unsigned STBC:2;
	unsigned FEC_CODING:1; // if is 11n packet, shows if is LDPC packet or not.
	unsigned SGI:1;
	unsigned rxend_state:8;
	unsigned ampdu_cnt:8;
	unsigned channel:4; //which channel this packet in.
	unsigned:12;
};

struct SnifferPacket {
	struct RxControl rx_ctrl;
	uint8_t data[DATA_LENGTH];
	uint16_t cnt;
	uint16_t len;
};

#endif /* __ESP_WIFI_TYPES_H__ */