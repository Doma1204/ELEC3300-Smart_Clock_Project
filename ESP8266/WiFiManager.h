#ifndef _WIFIMANAGER_H
#define _WIFIMANAGER_H

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <DNSServer.h>

extern "C" {
	#include "user_interface.h"
	#include "wpa2_enterprise.h"
}

#define Debug
#ifdef Debug
	#define log(str) Serial.println(str)
#else
	#define log(str)
#endif

const char PAGE_HEAD[] PROGMEM = "<!DOCTYPE html><html lang='en'><head><meta name='viewport' content='width=device-width, initial-scale=1, user-scalable=no'/><title>{t}</title>";
const char PAGE_SCRIPT[] PROGMEM = "<script>let wifi=[];function wifiScan(){document.getElementById('wifiBtn').innerHTML='Scanning WiFi';var xhr=new XMLHttpRequest();xhr.onreadystatechange=function(){if(this.readyState==4&&this.status==200){wifi=JSON.parse(this.responseText);for(let i=0;i<wifi.length;++i){document.getElementById('WiFi_List').innerHTML+=\"<option value='\" + wifi[i].SSID + \"'>\";} document.getElementById('wifiBtn').innerHTML='Scan WiFi';}};xhr.open('GET','/wifi?scan',true);xhr.send();} function encryptionType(select){console.log(select.value);if(select.value=='None'){document.getElementById('user_name_block').style.display='none';document.getElementById('password_block').style.display='none';}else if(select.value=='WPA2-Enterprize'){document.getElementById('user_name_block').style.display='inherit';document.getElementById('password_block').style.display='inherit';}else{document.getElementById('user_name_block').style.display='none';document.getElementById('password_block').style.display='inherit';}} function isDisplayPassword(checkbox){document.getElementById('password').type=checkbox.checked?'text':'password';}function connectWiFi() { let ssid = document.getElementById('SSID').value; if (!ssid.length) { alert('SSID cannot be empty'); return; } let url = '/wifi?ssid=' + ssid;let type = document.getElementById('encryption_type').value; console.log(type); if (type != 'None') { let password = document.getElementById('password').value; if (!password.length) { alert('Password cannot be empty'); return; }url += '&password=' + password;if (type == 'WPA2-Enterprize') { let username = document.getElementById('user_name').value; if (!username.length) { alert('User name cannot be empty'); return; } url += '&user_name=' + username; } } window.location.href = url;}</script>";
const char PAGE_STYLE[] PROGMEM = "<style>div{padding:5px;font-size:1em}body{text-align:center;font-family:verdana}h1{text-align:center}button{border:0;border-radius:0.3rem;background-color:#1fa3ec;color:#fff;line-height:2.4rem;font-size:1rem;width:100%}input{border:1px solid #a6a6a6;border-radius:3px;outline:none;width:100%;height:1.8em;font-size:1.2em;text-align:left;-webkit-appearance:none}input[type=checkbox]{width:15px;height:15px;-webkit-appearance:checkbox}select{background:transparent;outline:none;width:100%;height:2em;font-size:1.2em;border:1}</style>";
const char PAGE_BODY[] PROGMEM = "</head><body><div style='text-align:left;display:inline-block;min-width:260px;'>";
const char PAGE_ROOT[] PROGMEM = "<h1>{t}</h1><form action='/wifi' method='get'><button>Configure WiFi</button></form><br/>";
const char PAGE_WIFI[] PROGMEM = "<h1>{t}</h1><form action='javascript:connectWiFi()'><p>SSID: <input list='WiFi_List' id='SSID' type='' /> <datalist id='WiFi_List'> </datalist></p><p id='encryption_type_block'> Encryption Type: <select onchange='encryptionType(this)' id='encryption_type'><option value='None'>None</option><option value='WPA2' selected='selected'>WPA2</option><option value='WPA2-Enterprize'>WPA2-Enterprize</option> </select></p><p id='user_name_block' style='display: none;'> User Name: <input type='text' id='user_name'/></p><p id='password_block'> Password: <input type='password' id='password'/> <input type='checkbox' id='showPW' onchange='isDisplayPassword(this)'/> <label for='showPW'>Show Password</label></p> <button>Connect</button></form> <br /> <button onclick='wifiScan()' id='wifiBtn'>Scan WiFi</button>";
const char PAGE_END[] PROGMEM = "</div></body></html>";

typedef void (*WiFiManagerCallback)();

class WiFiManager {
	public:
	WiFiManager();

	bool autoConnect(void);
	bool autoConnect(const char* apSSID, const char* apPassword = NULL);

	wl_status_t connectWiFi(const char* ssid, const char* password = NULL);
	wl_status_t connectWiFi(const char* ssid, const char* userName, const char* password);

	bool startSetUpWiFi(void);
	bool startSetUpWiFi(const char* apSSID, const char *apPassword = NULL);

	bool startConfigPortal(void);
	bool startConfigPortal(const char* apSSID, const char *apPassword = NULL);

	void setPageTitle(String title);

	void configConnectWiFiTimeout(uint32_t timeout);
	void configConnectingWiFiCallbackInterval(uint32_t interval);
	void configSetupWiFiPortalTimeout(uint32_t timeout);
	void configSetupWiFiPortalCallbackInterval(uint32_t interval);

	void setConnectingWiFiCallback(WiFiManagerCallback callback);
	void setSetupWiFiPortalCallback(WiFiManagerCallback callback);

	private:
	std::unique_ptr<ESP8266WebServer> server;
	std::unique_ptr<DNSServer> dnsServer;

	String _ssid;
	String _userName;
	String _password;

	uint32_t _connectWiFiTimeout;
	uint32_t _connectingWiFiCallbackInterval;
	WiFiManagerCallback _connectingWiFiCallback;

	bool _isStartCaptivePortal;
	uint32_t _setupWiFiPortalTimeout;
	uint32_t _setupWiFiPortalCallbackInterval;
	WiFiManagerCallback _setupWiFiPortalCallback;

	String _pageTitle;

	wl_status_t waitForConnectResult(void);

	void handleRoot(void);
	void handleWiFi(void);
	void handleWiFiMain(void);
	void handleWiFiScan(void);
	void handleConnectWiFi(void);
	void handleNotFound(void);
	void handle204(void);
	bool handleCaptivePortal(void);

	void completeScan(int networksFound);

	// helper function handling IP
	bool isIP(String str);
	String ipToString(IPAddress ip);
};


#endif
