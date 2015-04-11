// 
// 
// 

#include "WaterLevelProbe.h"

#define POWER_DELAY 10 //delay between sending current and measuring
#define SENSIBILITY 100 //thresold above wich the pin will be cooonsidered as on (inside water) 0 - 1024


WaterLevelProbeClass::WaterLevelProbeClass(uint8_t lowPin, uint8_t highPin, uint8_t powPin)
{
	lowLvlPin = lowPin;
	highLvlPin = highPin;
	powerPin = powPin;

	pinMode(powerPin, OUTPUT);
	digitalWrite(powerPin, LOW);
}


void WaterLevelProbeClass::checkLevels() {
	Serial.print("Checking Water Levels : ");
	digitalWrite(powerPin, HIGH);
	delay(POWER_DELAY);
	Serial.print(analogRead(lowLvlPin));
	bool low = analogRead(lowLvlPin) > SENSIBILITY;
	Serial.print(" - ");
	Serial.print(analogRead(highLvlPin));
	bool high = analogRead(highLvlPin) > SENSIBILITY;
	
	digitalWrite(powerPin, LOW);
	
	Serial.print(" low=");
	Serial.print(low);
	Serial.print(" high=");
	Serial.print(high);

	if (low && high)
		state = TooHigh;
	else if (!low && !high)
		state = TooLow;
	else
		state = OK;
}

