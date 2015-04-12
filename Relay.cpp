// 
// 
// 

#include "Relay.h"

Relay::Relay(uint8_t rPin, bool normallyOpen) {
	isNormallyOpen = normallyOpen;
	pin = rPin;

	_isOn = !normallyOpen;
	pinMode(pin, OUTPUT);
	digitalWrite(pin, !isNormallyOpen);
}

void Relay::turnOn() 
{
	_isOn = true;
	digitalWrite(pin, isNormallyOpen);
}

void Relay::turnOff()
{
	_isOn = false;
	digitalWrite(pin, !isNormallyOpen);
}

bool Relay::isOn() {
	return _isOn;
}

