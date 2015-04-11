// 
// 
// 

#include "Relay.h"

Relay::Relay(uint8_t rPin, bool normallyOpen) {
	isNormallyOpen = normallyOpen;
	pin = rPin;
	isOn = !normallyOpen;
	
	pinMode(pin, OUTPUT);
	digitalWrite(pin, isNormallyOpen);
}

void Relay::turnOn() 
{
	digitalWrite(pin, !isNormallyOpen);
}

void Relay::turnOff()
{
	digitalWrite(pin, isNormallyOpen);
}

