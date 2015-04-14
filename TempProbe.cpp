// 
// 
// 

#include "TempProbe.h"
#define TEMPERATURE_PRECISION 12

void TempProbeClass::Init(uint8_t tempPin, float temp, Relay* relay)
{
	tempRelay = relay;
	targetTemp = temp;

	oneWire = new OneWire(tempPin);
	sensors = new DallasTemperature(oneWire);

	
	sensors->begin();

	// Grab a count of devices on the wire
	uint8_t numberOfDevices = sensors->getDeviceCount();

	// locate devices on the bus
	Serial.print("TempProbe : Locating devices...");

	Serial.print("Found ");
	Serial.print(numberOfDevices, DEC);
	Serial.println(" devices.");

	// report parasite power requirements
	Serial.print("Parasite power is: ");
	if (sensors->isParasitePowerMode()) Serial.println("ON");
	else Serial.println("OFF");


	// Loop through each device, print out address
	for (int i = 0; i<numberOfDevices; i++)
	{
		// Search the wire for address
		if (sensors->getAddress(deviceAddress, i))
		{

			Serial.print("Found device ");
			Serial.print(i, DEC);
			Serial.print(" with address: ");
//			printAddress(deviceAddress);
			Serial.println();

			Serial.print("Setting resolution to ");
//			Serial.println(TEMPERATURE_PRECISION, DEC);

			// set the resolution to TEMPERATURE_PRECISION bit (Each Dallas/Maxim device is capable of several different resolutions)
			sensors->setResolution(deviceAddress, TEMPERATURE_PRECISION);

			Serial.print("Resolution actually set to: ");
			Serial.print(sensors->getResolution(deviceAddress), DEC);
			Serial.println();

			break;
		}
		else{
			Serial.print("Found ghost device at ");
			Serial.print(i, DEC);
			Serial.print(" but could not detect address. Check power and cabling");
		}
	}

}


bool TempProbeClass::GetTemp() {
	Serial.print("Request temp ... "); 
	if( sensors->requestTemperaturesByAddress(deviceAddress) )
	{
		temperature = sensors->getTempC(deviceAddress);
	
		Serial.print("SUCCESS Temp = ");
		Serial.print(temperature);
		Serial.println(" C");

		if (temperature < targetTemp) {
			if (!tempRelay->isOn())
				Serial.println("Turn heater on");
			tempRelay->turnOn();
		}
		else {
			if (tempRelay->isOn())
				Serial.println("Turn heater off");
			tempRelay->turnOff();
		}
		return true;
	}
	else {
		Serial.print(" FAILURE Try finding the first probe ");
		if(sensors->getAddress(deviceAddress, 0))
			Serial.println(" SUCCESS");
		else
			Serial.println(" FAILURE");
		return false;
	}
}






