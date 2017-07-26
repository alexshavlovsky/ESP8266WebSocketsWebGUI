/*
  ModbusTcpSlave.h -- Modbus TCP Slave (Server) for ESP8266 Arduino IDE
 
  Copyright (c) 2017 Alex Shavlovsky. All rights reserved.
 
  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

/*
  1. Only modbus function (0x03) is implemented
  2. Full input requests semantic parsing and error handling
  3. Input requests statistics
  4. Up to 125 registers per message
  5. Up to 800 messages per second (2 clients, 16 registers request, 1 request per 2 ms per client)
  6. Up to 5 TCP clients keep alive connection
*/


#ifndef MODBUSTCPSLAVE_H_
#define MODBUSTCPSLAVE_H_

#include <ESP8266WiFi.h>

#define swapb(x) (((uint16_t)x >> 8) + (((uint16_t)x & 0x00FF) << 8))
//tcp server setup
#define DEF_TCP_PORT 502
#define MAX_CLNT_NUM 2
#define CON_KEEP_ALIVE 500 //ms
//protocol setup
#define PROT_ID 0
#define MBAP_LEN 7
#define REGS_MAX_REQ 16 //protocol reference max 125
#define MAX_PDU_LEN REGS_MAX_REQ*2+3
//error codes
#define MB_EC_ILLEGAL_FUNCTION 1
#define MB_EC_ILLEGAL_DATA_ADDRESS 2
#define MB_EC_ILLEGAL_DATA_VALUE 3

#define REGS_NUM 16

struct mb_slave_status_t {
    uint32_t total_requests;
    uint32_t ok_requests;
    uint8_t clients_num;
    uint8_t last_error_code;
};

struct mb_tcp_adu_t {
    uint16_t trans_id;
    uint16_t prot_id;
    uint16_t mes_len;
    uint8_t unit_id;
    uint8_t pdu[MAX_PDU_LEN];
};

class ModbusTcpSlave {

    uint8_t UNIT_ID;
    uint16_t TCP_PORT;

    WiFiServer MBServer = WiFiServer(DEF_TCP_PORT);
    WiFiClient mb_client[MAX_CLNT_NUM];
    uint8_t cltnAlive[MAX_CLNT_NUM];
    uint32_t time_estab[MAX_CLNT_NUM];

    bool serve_client(WiFiClient client) {
        while(client.available()) {
            mb_tcp_adu_t mbap;
            uint8_t err_packet = 0;
            uint8_t err_code = 0;
            uint16_t i = 0;
            uint16_t pdu_len;
            while(client.available() && (i < MBAP_LEN)) *((uint8_t*)&mbap + i++) = client.read();
            pdu_len = swapb(mbap.mes_len) - 1;
            if(i != MBAP_LEN) err_packet = 1;
            else if(swapb(mbap.prot_id) != PROT_ID) err_packet = 2;
            else if(mbap.unit_id != UNIT_ID) err_packet = 3;
            else if(pdu_len > MAX_PDU_LEN) err_packet = 4;
            else {
                i = 0;
                while(client.available() && (i < pdu_len)) mbap.pdu[i++] = client.read();
                if(i != pdu_len) err_packet = 5;
            }
            if(!err_packet) {
                switch(mbap.pdu[0]) {
                case 0x03: {
                    uint16_t start_addr = ((uint16_t)mbap.pdu[1] << 8) + (uint16_t)mbap.pdu[2];
                    uint16_t regs_num = ((uint16_t)mbap.pdu[3] << 8) + (uint16_t)mbap.pdu[4];
                    if(!(regs_num >= 1 && regs_num <= REGS_MAX_REQ)) err_code = MB_EC_ILLEGAL_DATA_VALUE;
                    else if((start_addr + regs_num - 1) >= REGS_NUM) err_code = MB_EC_ILLEGAL_DATA_ADDRESS;
                    else {
                        pdu_len = 2 + 2 * regs_num;
                        mbap.pdu[1] = 2 * regs_num;
                        for(i = 0; i < regs_num; i++) {
                            mbap.pdu[2 + i * 2] = regs[start_addr + i] >> 8;
                            mbap.pdu[3 + i * 2] = regs[start_addr + i] & 0xFF;
                        }
                        mbap.mes_len = swapb(pdu_len + 1);
                        client.write((uint8_t*)&mbap, pdu_len + 7);
                    }
                    break;
                }
                default:
                    err_code = MB_EC_ILLEGAL_FUNCTION;
                }
                if(err_code) {
                    pdu_len = 2;
                    mbap.pdu[0] = mbap.pdu[0] + 0x80;
                    mbap.pdu[1] = err_code;
                    mbap.mes_len = swapb(pdu_len + 1);
                    client.write((uint8_t*)&mbap, pdu_len + 7);
                }
            }
            status.last_error_code=err_code+err_packet*10;
            status.total_requests++;
            if(err_packet || err_code) return false;
            status.ok_requests++;
        }
        return true;
    }

public:
    uint16_t regs[REGS_NUM];

    mb_slave_status_t status;

    ModbusTcpSlave(uint16_t TcpPort,uint8_t UnitId):TCP_PORT(TcpPort),UNIT_ID(UnitId) {
        MBServer=WiFiServer(TCP_PORT);
        status.total_requests=0;
        status.ok_requests=0;
        status.clients_num=0;
        status.last_error_code=0;
        for(int i=0; i<MAX_CLNT_NUM; i++) cltnAlive[i]=0;
    }

    void begin() {
        MBServer.begin();
    }

    bool run() {
	bool evnt=false;
        status.clients_num=0;
        for(int i = 0; i < MAX_CLNT_NUM; i++) {
            if(cltnAlive[i]&&millis() > time_estab[i] + CON_KEEP_ALIVE) { // if connection timeout - disconnect this client
                cltnAlive[i]=0;
                mb_client[i].stop();
                mb_client[i]=WiFiClient();
            }
            yield();
            if(!cltnAlive[i]&&MBServer.hasClient()) { // if this slot is empty and there is a new client
                cltnAlive[i]=1;
                time_estab[i] = millis();  // start the keep-alive timer
                mb_client[i] = MBServer.available(); // get a new client from the server
            }
            yield();
            if(cltnAlive[i]) {
                if(mb_client[i].available()) {  // if there is any data from the client
		    evnt=true;
                    if(serve_client(mb_client[i])) time_estab[i] = millis(); // if request parsed - reset the keep-alive timer
                    else { // if error - disconnect this client
                        cltnAlive[i]=0;
                        mb_client[i].stop();
                        mb_client[i]=WiFiClient();
                    }
                }
            }
            if(cltnAlive[i])status.clients_num++;
        }
        if(status.clients_num==MAX_CLNT_NUM) {
            while(MBServer.hasClient()&&MBServer.available()) yield();   //clear the queue and refuse the connection to avoid a queue overflow
        }
	return evnt;
    }
};


#endif //MODBUSTCPSLAVE_H_
