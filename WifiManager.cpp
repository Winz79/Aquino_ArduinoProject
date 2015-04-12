// 
// 
// 

#include "WifiManager.h"
#include <Time.h>
#include <DS1307RTC.h>

time_t lastUpdate;

void WifiManager::init()
{
	Serial3.begin(9600);
}


void WifiManager::UpdateStatus(int temp, uint8_t levelStatus, uint8_t relayStatus) {
	if (now() - lastUpdate < 20) {
		Serial.println("Last update is less than 60s => skip");
	}
	else {
                lastUpdate = RTC.get();
		Serial.println("-> ESP  sendTemperature(" + String(temp) + "," + String(levelStatus) + "," + String(relayStatus) + ")");

		Serial3.flush();
		Serial3.println("sendTemperature(" + String(temp) + "," + String(levelStatus) + "," + String(relayStatus) +")");
	}
}
