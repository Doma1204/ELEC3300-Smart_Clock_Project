#include "NTPClock.h"

NTPClock::NTPClock(void): Timezone(DefaultTimezone), requestInterval(DefaultRequestInterval) {}

void NTPClock::init(void) {
	// Start UDP
	// log("Starting UDP");
	UDP.begin(UDP_Port);

	// log_("Local Port: ");
	// log(UDP.localPort());

	// Get the IP address of the NTP server
	this->updateNTPServerIP();

	// First time to get the current time from the NTP server
	this->sendNTPpacket(timeServerIP);
	last_request_tick = millis();
	uint32_t firstRequestInterval = 1000;

	while (true) {
		uint32_t curTick = millis();

		if (this->updateTime()) break;

		if (curTick - last_request_tick >= firstRequestInterval) {
			this->sendNTPpacket(timeServerIP);
			last_request_tick = curTick;
		}
	}
}

void NTPClock::updateNTPServerIP(void) {
	if (!WiFi.hostByName(DefaultNTPServerName, timeServerIP)) {
		// log("DNS loopup failed. Rebooting");
		Serial.flush();
		ESP.restart();
	}

	// log("Time Server IP: " + timeServerIP);
}

void NTPClock:: update(void) {
	uint32_t curTick = millis();

	if (curTick - last_request_tick >= requestInterval) {
		this->sendNTPpacket(timeServerIP);
		last_request_tick = curTick;
	}

	this->updateTime();
}

//Private Functions
void NTPClock::sendNTPpacket(IPAddress& address) {
//	// log("Sending NTP request...");

	memset(NTPBuffer, 0, NTP_PACKET_SIZE);  // set all bytes in the buffer to 0
	// Initialize values needed to form NTP request
	NTPBuffer[0] = 0b11100011;   // LI, Version, Mode
	NTPBuffer[1] = 0;     // Stratum, or type of clock
	NTPBuffer[2] = 6;     // Polling Interval
	NTPBuffer[3] = 0xEC;  // Peer Clock Precision
	// 8 bytes of zero for Root Delay & Root Dispersion
	NTPBuffer[12]  = 49;
	NTPBuffer[13]  = 0x4E;
	NTPBuffer[14]  = 49;
	NTPBuffer[15]  = 52;
	// send a packet requesting a timestamp:
	UDP.beginPacket(address, NTP_Request_Port); // NTP requests are to port 123
	UDP.write(NTPBuffer, NTP_PACKET_SIZE);
	UDP.endPacket();
}

bool NTPClock:: updateTime() {
	if (UDP.parsePacket() == 0)  return false; // If there's not response (yet)

	UDP.read(NTPBuffer, NTP_PACKET_SIZE); // read the packet into the buffer

	// Combine the 4 timestamp bytes into one 32-bit number
	uint32_t NTPTime = (NTPBuffer[40] << 24) | (NTPBuffer[41] << 16) | (NTPBuffer[42] << 8) | NTPBuffer[43];

	// Convert NTP time to a UNIX timestamp, Unix time starts on Jan 1 1970. That's 2208988800 seconds in NTP time
	const uint32_t seventyYears = 2208988800UL;

	// subtract seventy years and add the timezone
	this->UNIXTime = NTPTime - seventyYears + DefaultTimezone * 3600;

//	// log("Timezone");
//	// log_("NTP Response: ");
//	// log(this->UNIXTime);

	setTime(this->UNIXTime);
	return true;
}
