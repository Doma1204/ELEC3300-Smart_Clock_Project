#include "WiFiManager.h"
#include "NTPClock.h"

WiFiManager wifiManager;
NTPClock ntpClock;

void setup() {
	Serial.begin(115200);
	pinMode(LED_BUILTIN, OUTPUT);
	delay(100);

	// settting of WiFi manager
	wifiManager.configSetupWiFiPortalCallbackInterval(500);
	wifiManager.setSetupWiFiPortalCallback(ledToggle);
	wifiManager.configConnectingWiFiCallbackInterval(100);
	wifiManager.setConnectingWiFiCallback(ledToggle);
	wifiManager.autoConnect("ESP Test");

	log("Auto Connect Success");
	log(WiFi.SSID());
	log(WiFi.localIP());

	ntpClock.init();
	digitalWrite(LED_BUILTIN, 1);
}

void loop() {
	ntpClock.update();

	Serial.print(hour());
	Serial.print(":");
	Serial.print(minute());
	Serial.print(":");
	Serial.println(second());
	delay(1000);
}

void ledToggle(void) {
	static bool ledOn;
	ledOn = !ledOn;
	digitalWrite(LED_BUILTIN, ledOn);
}
