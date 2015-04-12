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

class WifiManager
{
public:
	void init();
	void UpdateStatus(int temp, uint8_t levelStatus, uint8_t relayStatus);
};

#endif

