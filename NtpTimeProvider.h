#include <TimeLib.h>
#include <WiFiUdp.h>
#define NTP_TIME_ZONE 3
#define NTP_UDP_PACKET_SIZE 48
#define NTP_UDP_LOCAL_PORT 8888
#define NTP_UDP_TIMEOUT 2000

typedef enum {asyncNtpIdle, asyncNtpWait, asyncNtpNeedSync} asyncNtpStatus_t;

asyncNtpStatus_t asyncNtpStatus = asyncNtpIdle;
uint32_t beginNtpUdpStart;

WiFiUDP Udp;
uint8_t packetBuffer[NTP_UDP_PACKET_SIZE];

void sendNTPpacket(IPAddress &address) {
  memset(packetBuffer, 0, NTP_UDP_PACKET_SIZE);
  packetBuffer[0] = 0b11100011;
  packetBuffer[1] = 0;
  packetBuffer[2] = 6;
  packetBuffer[3] = 0xEC;
  packetBuffer[12] = 49;
  packetBuffer[13] = 0x4E;
  packetBuffer[14] = 49;
  packetBuffer[15] = 52;
  Udp.beginPacket(address, 123);
  Udp.write(packetBuffer, NTP_UDP_PACKET_SIZE);
  Udp.endPacket();
}

time_t asyncNtpRun() {
  switch (asyncNtpStatus) {
    case asyncNtpNeedSync: {
        if (enWifi && Udp.begin(NTP_UDP_LOCAL_PORT)) {
          IPAddress ntpServerIP;
          WiFi.hostByName("us.pool.ntp.org", ntpServerIP);
          sendNTPpacket(ntpServerIP);
          beginNtpUdpStart = millis();
          asyncNtpStatus = asyncNtpWait;
        }
        break;
      }
    case asyncNtpWait: {
        if (millis() - beginNtpUdpStart >= NTP_UDP_TIMEOUT) {
          asyncNtpStatus = asyncNtpIdle;
          Udp.stop();
        }
        else if (Udp.parsePacket() >= NTP_UDP_PACKET_SIZE) {
          Udp.read(packetBuffer, NTP_UDP_PACKET_SIZE);
          unsigned long secsSince1900;
          secsSince1900 =  (unsigned long)packetBuffer[40] << 24;
          secsSince1900 |= (unsigned long)packetBuffer[41] << 16;
          secsSince1900 |= (unsigned long)packetBuffer[42] << 8;
          secsSince1900 |= (unsigned long)packetBuffer[43];
          asyncNtpStatus = asyncNtpIdle;
          Udp.stop();
          return secsSince1900 - 2208988800UL + NTP_TIME_ZONE * SECS_PER_HOUR;
        }
        break;
      }
  }
  return 0;
}
