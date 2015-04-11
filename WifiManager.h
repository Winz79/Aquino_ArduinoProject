// WifiManager.h


#ifndef _WIFIMANAGER_h
#define _WIFIMANAGER_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#define SSID "Livebox-F444" // insert your SSID
#define PASSWORD "EUREKA79"
#define THINGSPEAK "184.106.153.149" // thingspeak.com

#include <WeeESP8266.h>

class WifiManager
{
	ESP8266* wifi;
public:
	void init();
	void UpdateTemp(float* temp);
};

#endif

