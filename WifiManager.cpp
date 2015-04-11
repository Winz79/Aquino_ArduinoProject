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


void WifiManager::UpdateTemp(float* temp) {
	if (now() - lastUpdate < 60) {
		Serial.println("Last update is less than 60s => skip");
	}
	else {
		Serial.println("->ESP : sendTemperature(" + String(*temp) + ")");
		Serial3.println("sendTemperature(" + String(*temp) + ")");
	}
}
