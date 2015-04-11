// Relay.h

#ifndef _RELAY_h
#define _RELAY_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

class Relay
{
 protected:
	 bool isNormallyOpen = true;
	 uint8_t pin;
	 bool _isOn;
 public:
	Relay(uint8_t, bool);
	void init();
	void turnOn();
	void turnOff();
	bool isOn();
};

#endif

