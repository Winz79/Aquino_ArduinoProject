// TempProbe.h

#ifndef _TEMPPROBE_h
#define _TEMPPROBE_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif


#include <OneWire.h>
#include <DallasTemperature.h>
#include "Relay.h"

class TempProbeClass
{
 OneWire* oneWire;
 DallasTemperature* sensors;
 DeviceAddress deviceAddress;
 Relay* tempRelay;
 float targetTemp;

 public:
	 float temperature;
	 void Init(uint8_t pin, float temp, Relay* relay);
	 bool GetTemp();
};

extern TempProbeClass TempProbe;

#endif

