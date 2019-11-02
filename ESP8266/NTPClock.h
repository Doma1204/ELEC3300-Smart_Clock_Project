#ifndef _NTPCLOCK_H
#define _NTPCLOCK_H

#include <ESP8266WiFi.h>
#include <WiFiUDP.h>
#include <TimeLib.h>

#define Debug
#ifdef Debug
	#define log(str) Serial.println(str)
	#define log_(str) Serial.print(str)
#else
	#define log(str)
	#define log_(str)
#endif

// UDP setting
#define UDP_Port 8888

// NTP setting
#define NTP_PACKET_SIZE 48
#define NTP_Request_Port 123
#define DefaultNTPServerName "hk.pool.ntp.org"
#define DefaultTimezone (+8)
#define DefaultRequestInterval 300000 // 5 * 60000

class NTPClock {
	private:
		WiFiUDP UDP;
		IPAddress timeServerIP;
		byte NTPBuffer[NTP_PACKET_SIZE];
		uint32_t last_request_tick;
		uint32_t requestInterval;

		int8_t Timezone;
		uint32_t UNIXTime;

		void sendNTPpacket(IPAddress& address);
		bool updateTime(void);

	public:
		NTPClock(void);
		void init(void);
		void updateNTPServerIP(void);
		void update(void);

		int8_t getTimezone(void) const {return this->Timezone;}
		uint32_t getUNIXTime(void) const {return this->UNIXTime;}

		void setTimezone(const int8_t newTimezone) {this->Timezone = newTimezone;}
		void setRequestInterval(const uint32_t newInterval) {this->requestInterval = newInterval * 60000;} // the unit of input newInterval is minute
};

#endif
