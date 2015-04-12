// 
// 
// 

#include "WifiManager.h"
#include <Time.h>

time_t lastUpdate;

void WifiManager::init()
{
	Serial3.begin(9600);
}


void WifiManager::UpdateStatus(int temp, uint8_t levelStatus, uint8_t relayStatus) {
	if (now() - lastUpdate < 60) {
		Serial.println("Last update is less than 60s => skip");
	}
	else {
		Serial.println("ESP -> sendTemperature(" + String(temp) + "," + String(levelStatus) + "," + String(relayStatus) + ")");
		Serial3.println("sendTemperature(" + String(temp) + "," + String(levelStatus) + "," + String(relayStatus) +")");
	}
}
