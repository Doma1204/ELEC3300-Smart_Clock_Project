# ELEC3300-Smart_Clock_Project
A self propose project for HKUST ELEC3300 Course. The smart clock is a IOT device with various kinds of function.

### Time
The smart clock is connected to the Internet via ESP8266. It fetches Internet time from NTP server. Therefore, the time is always accurate.

### Weather and Forecast
The smart clock also uses ESP8266 to get Hong Kong current weather by using Hong Kong Observatory API. Weather information can be shown on the clock.

### Temperature
Besides weather from Internet, it can also measure the surrounding temperature and display it on the clock.

## Display
There are two display on the clock, a modularizable RGB panel and a 2.4 inch TFT.

### Modularizable RGB panel
A panel module is a 8x8 WS2812 RGB panel, with a STMF0 chip attached. You can freely connect any number of module and build whatever shape you like. The panel are connected to the mainboard via I2C. It has a panel detection system in order to detect the number of panel connected, their position, shape, and orientation. For details of the system, please refer to

### TFT Display
The TFT display is attached to the mainboard as a secondary display. It mainly shows temperatures and weather information.
