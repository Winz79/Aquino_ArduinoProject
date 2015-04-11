// 
// 
// 

#include "WifiManager.h"
#include <Time.h>

time_t lastUpdate;

void WifiManager::init()
{
	Serial3.begin(9600);
	/*
	wifi = new ESP8266(Serial3);
	
	wifi->begin();

	if (wifi->kick())
		Serial.println("Wifi OK");
	else {
		Serial.println("Wifi ERROR, try resetting");
		if (wifi->restart()) {
			Serial.println("Restart OK");
		}
		else {
			Serial.println("Restart ERROR");
		};
	}

	Serial.println(wifi->getVersion());
	wifi->enableMUX();
	wifi->setOprToStationSoftAP();

	if (wifi->joinAP(SSID, PASSWORD)) {
		Serial.print("Joined AP, IP = ");
		Serial.println(wifi->getLocalIP());
	}
	*/
}



void WifiManager::UpdateTemp(float temp) {
	if (now() - lastUpdate < 60) {
		Serial.println("Last update is less than 60s => skip");
		//return;
	}
	Serial.println("Sending to Thingspeak");
	Serial1.flush();
		wifi->getIPStatus();
		if (wifi->createTCP(0,THINGSPEAK, 80)) {
			String query = "GET /update?key=PGL2SMS81JUSLF98&field1=" + String(temp) + " \n\n";
			query.length();
			Serial.println(query);
			if (wifi->send(0, (const uint8_t*)query.c_str(), query.length())) {
				Serial.println("Things speak update sent");
				lastUpdate = now();
			}
			else
				Serial.println("Things speak update ERROR");
			//wifi->releaseTCP(0);
		}
		else {
			Serial.println("TCP creation ERROR");
			
		}
	}
