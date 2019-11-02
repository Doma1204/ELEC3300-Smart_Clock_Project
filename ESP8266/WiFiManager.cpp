#include "WiFiManager.h"

WiFiManager::WiFiManager():
_ssid(""),
_userName(""),
_password(""),
_connectWiFiTimeout(10000),
_connectingWiFiCallbackInterval(50),
_connectingWiFiCallback(NULL),
_isStartCaptivePortal(true),
_setupWiFiPortalTimeout(0),
_setupWiFiPortalCallbackInterval(50),
_setupWiFiPortalCallback(NULL),
_pageTitle("")
{}

/**
 * Start auto connect procedure
 * Access point mode will be turn on with default ap ssid when auto connect fail,
 * and a server and captive portal is on for requesting Wi-Fi connection.
 * @return if Wi-Fi is connected
 */
bool WiFiManager::autoConnect(void) {
	String apSSID = "ESP" + String(ESP.getChipId());
	return this->autoConnect(apSSID.c_str(), NULL);
}

/**
 * Start auto connect procedure
 * Access point mode will be turn on with custom ap ssid and password when auto connect fail,
 * and a server and captive portal is on for requesting Wi-Fi connection.
 * @return if Wi-Fi is connected
 */
bool WiFiManager::autoConnect(const char* apSSID, const char* apPassword) {
	WiFi.mode(WIFI_STA);
	WiFi.setAutoConnect(true);
	log("Start Auto Connect");
	if (this->connectWiFi("", "") == WL_CONNECTED) {
		return true;
	}

	return this->startSetUpWiFi(apSSID, apPassword);
}

/**
 * Connect Wi-Fi with given ssid and password
 * @param ssid: const char*
 * @param password: optional const char*
 * @return Wi-Fi connection status
 */
// TODO: make static sta ip
wl_status_t WiFiManager::connectWiFi(const char* ssid, const char* password) {
	if (WiFi.status() == WL_CONNECTED) {
		log("WiFi already connected");
		return WL_CONNECTED;
	}

	if (strlen(ssid)) {
		WiFi.begin(ssid, password);
	} else {
		if (WiFi.SSID()) {
			WiFi.begin();
		} else {
			return WL_CONNECT_FAILED;
		}
	}
	log("Start connecting WiFi");

	return this->waitForConnectResult();
}

/**
 * Connect Wi-Fi with given ssid, username and password
 * @param ssid: const char*
 * @param userName: const char*
 * @param password: const char*
 * @return Wi-Fi connection status
 */
wl_status_t WiFiManager::connectWiFi(const char* ssid, const char* userName, const char* password) {
	wifi_set_opmode(STATION_MODE);  // It cannot be ap mode or dual mode

	struct station_config wifi_config;
	memset(&wifi_config, 0, sizeof(wifi_config));
	strcpy((char*)wifi_config.ssid, ssid);

	wifi_station_set_config(&wifi_config);

	wifi_station_clear_cert_key();
	wifi_station_clear_enterprise_ca_cert();
	wifi_station_set_wpa2_enterprise_auth(1);
	wifi_station_set_enterprise_identity((uint8_t*)userName, strlen(userName));
	wifi_station_set_enterprise_username((uint8_t*)userName, strlen(userName));
	wifi_station_set_enterprise_password((uint8_t*)password, strlen(password));
	wifi_station_connect();

	return this->waitForConnectResult();
}


wl_status_t WiFiManager::waitForConnectResult(void) {
	uint32_t initTime = this->_connectWiFiTimeout ? millis() + this->_connectWiFiTimeout : 0;
	wl_status_t status = WL_DISCONNECTED;
	while(!this->_connectWiFiTimeout || millis() < initTime) {
		status = WiFi.status();
		if (status == WL_CONNECTED || status == WL_CONNECT_FAILED) {
			break;
		}

		if (this->_connectingWiFiCallback) {
			this->_connectingWiFiCallback();
		}
		delay(this->_connectingWiFiCallbackInterval);
	}
	return status;
}

bool WiFiManager::startSetUpWiFi(void) {
	String apSSID = "ESP" + String(ESP.getChipId());
	return this->startSetUpWiFi(apSSID.c_str(), NULL);
}

// TODO: make static ap ip
// TODO: add mDNS support
bool WiFiManager::startSetUpWiFi(const char* apSSID, const char *apPassword) {
	log("Start WiFi Manager");

	this->dnsServer.reset(new DNSServer());
	this->server.reset(new ESP8266WebServer(80));


	if (!WiFi.isConnected()) {
		log("Set AP Mode");
		WiFi.persistent(false);
		WiFi.disconnect();
		WiFi.mode(WIFI_AP);
		WiFi.persistent(true);
	} else {
		log("Set AP_STA Mode");
		WiFi.mode(WIFI_AP_STA);
	}

	if (!apPassword || strlen(apPassword) < 8 || strlen(apPassword) > 63) {
		log("No AP password or invalid password");
		WiFi.softAP(apSSID);
	} else {
		log("Set AP Password");
		WiFi.softAP(apSSID, apPassword);
	}
	log("Start softAP");

	if (this->_isStartCaptivePortal) {
		log("Set DNS server as captive portal");
		log(WiFi.softAPIP());
		this->dnsServer->setErrorReplyCode(DNSReplyCode::NoError);
		this->dnsServer->start(53, "*", WiFi.softAPIP());
	}

	// TODO: setup webpage request
	this->server->on("/", std::bind(&WiFiManager::handleRoot, this));
	this->server->on(String(F("/wifi")), std::bind(&WiFiManager::handleWiFi, this));
	this->server->onNotFound(std::bind(&WiFiManager::handleNotFound, this));
	this->server->begin();

	uint32_t initTime = this->_setupWiFiPortalTimeout ? millis() + this->_setupWiFiPortalTimeout : 0;

	while (!this->_setupWiFiPortalTimeout || millis() > initTime) {
		if (_isStartCaptivePortal) {
			this->dnsServer->processNextRequest();
		}
		this->server->handleClient();

		// TODO: see if it is possible to not change to STA before WiFi is connected.
		if (this->_ssid.length()) {
			WiFi.mode(WIFI_STA);
			if (this->_userName == "") {
				log("none or password");
				this->connectWiFi(this->_ssid.c_str(), this->_password.c_str());
			} else {
				log("user");
				this->connectWiFi(this->_ssid.c_str(), this->_userName.c_str(), this->_password.c_str());
			}

			this->waitForConnectResult();
			this->_ssid = "";
			break;
		}

		if (this->_setupWiFiPortalCallback) {
			this->_setupWiFiPortalCallback();
		}
		delay(this->_setupWiFiPortalCallbackInterval);
	}

	this->server.reset();
	if (this->_isStartCaptivePortal) {
		this->dnsServer.reset();
	}

	return WiFi.status() == WL_CONNECTED;
}

void WiFiManager::setPageTitle(String title) {
	this->_pageTitle = title;
}

void WiFiManager::configConnectWiFiTimeout(uint32_t timeout) {
	this->_connectWiFiTimeout = timeout;
}

void WiFiManager::configConnectingWiFiCallbackInterval(uint32_t interval) {
	this->_connectingWiFiCallbackInterval = interval;
}

void WiFiManager::configSetupWiFiPortalTimeout(uint32_t timeout) {
	this->_setupWiFiPortalTimeout = timeout;
}

void WiFiManager::configSetupWiFiPortalCallbackInterval(uint32_t interval) {
	this->_setupWiFiPortalCallbackInterval = interval;
}


void WiFiManager::setConnectingWiFiCallback(WiFiManagerCallback callback) {
	this->_connectingWiFiCallback = callback;
}

void WiFiManager::setSetupWiFiPortalCallback(WiFiManagerCallback callback) {
	this->_setupWiFiPortalCallback = callback;
}

void WiFiManager::handleRoot(void) {
	if (this->handleCaptivePortal()) {
		return;
	}

	log("loading page");

	String page = FPSTR(PAGE_HEAD);
	page += FPSTR(PAGE_STYLE);
	page += FPSTR(PAGE_BODY);
	page += FPSTR(PAGE_ROOT);
	page += FPSTR(PAGE_END);
	page.replace("{t}", this->_pageTitle);

	// this->server->sendHeader("Content-Length", String(page.length()));
	this->server->sendHeader("Cache-Control", "no-cache, no-store, must-revalidate");
	this->server->sendHeader("Pragma", "no-cache");
	this->server->sendHeader("Expires", "-1");
	this->server->send(200, "text/html", page);
}

void WiFiManager::handleWiFi(void) {
	log("handle wifi");
	if (this->server->args()) {
		if (this->server->argName(0).equals("scan")) {
			log("scan");
			this->handleWiFiScan();
		} else {
			log("connect");
			this->handleConnectWiFi();
		}
	} else {
		log("main");
		this->handleWiFiMain();
	}
}

void WiFiManager::handleWiFiMain(void) {
	String page = FPSTR(PAGE_HEAD);
	page += FPSTR(PAGE_SCRIPT);
	page += FPSTR(PAGE_STYLE);
	page += FPSTR(PAGE_BODY);
	page += FPSTR(PAGE_WIFI);
	page += FPSTR(PAGE_END);
	page.replace("{t}", this->_pageTitle);
	server->sendHeader("Content-Length", String(page.length()));
	this->server->send(200, "text/html", page);
}

// helper function for sorting WiFi base on strength
int wifiSort(const void* wifi1, const void* wifi2) {
	return WiFi.RSSI(*((int*) wifi2)) - WiFi.RSSI(*((int*) wifi1));
}

void WiFiManager::handleWiFiScan(void) {
	log("Scanning WiFi");
	int8_t networksFound = WiFi.scanNetworks();

	int8_t wifi[networksFound];
	for (int8_t i = 0; i < networksFound; wifi[i] = i++);
	// Sort RSSI
	qsort(wifi, networksFound, sizeof(wifi[0]), wifiSort);
	// Remove duplicates
	String ssid;
	for (int8_t i = 0; i < networksFound; ++i) {
		if (wifi[i] == -1) continue;
		ssid = WiFi.SSID(wifi[i]);
		for (int8_t j = i + 1; j < networksFound; ++j) {
			if (ssid == WiFi.SSID(wifi[j])) wifi[j] = -1;
		}
	}

	// Convert the WiFi list into JSON
	String response = "[";
	for (int8_t i = 0; i < networksFound; ++i) {
		if (wifi[i] < 0) continue;
		response += "{\"SSID\":\"" + WiFi.SSID(wifi[i]) + "\"";
		response += ",\"Channel\":" + String(WiFi.channel(wifi[i]));
		response += ",\"RSSI\":" + String(WiFi.RSSI(wifi[i]));
		response += ",\"Encryption\":" + String(WiFi.encryptionType(wifi[i]));
		response += "}";
		if (i != networksFound - 1) response += ",";
	}
	response += "]";
	WiFi.scanDelete();
	this->server->send(200, "text/plain", response);
}

void WiFiManager::handleConnectWiFi(void) {
	this->_ssid = this->server->arg("ssid");
	this->_userName = this->server->arg("user_name");
	this->_password = this->server->arg("password");
}


void WiFiManager::handleNotFound(void) {

}

void WiFiManager::handle204(void) {

}

bool WiFiManager::handleCaptivePortal(void) {
	log("handleCaptivePortal");
	if (!this->isIP(this->server->hostHeader())) {
		log("Redirecting Page");
		log(String("http://") + this->ipToString(this->server->client().localIP()));
		this->server->sendHeader("Location", String("http://") + this->ipToString(this->server->client().localIP()), true);
		this->server->send(302, "text/plain", "");
		this->server->client().stop();
		return true;
	}
	log("No Redirect");
	return false;
}

bool WiFiManager::isIP(String str) {
	for (uint8_t i = 0; i < str.length(); ++i) {
		char c = str.charAt(i);
		if (c != '.' && (c < '0' || c > '9')) {
			return false;
		}
	}
	return true;
}

String WiFiManager::ipToString(IPAddress ip) {
	String ipStr = "";
	for (uint8_t i = 0; i < 3; ++i) {
    	ipStr += String((ip >> (8 * i)) & 0xFF) + ".";
	}
	ipStr += String(((ip >> 8 * 3)) & 0xFF);
	return ipStr;
}

