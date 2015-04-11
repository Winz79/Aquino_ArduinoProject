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


class TempProbeClass
{
 OneWire* oneWire;
 DallasTemperature* sensors;
 DeviceAddress deviceAddress;

 public:
	 void Init(uint8_t pin);
	 float GetTemp();
};

extern TempProbeClass TempProbe;

#endif

