//
// Maurice Ribble 
// 4-17-2008
// http://www.glacialwanderer.com/hobbyrobotics

// This code tests the DS1307 Real Time clock on the Arduino board.
// The ds1307 works in binary coded decimal or BCD.  You can look up
// bcd in google if you aren't familior with it.  There can output
// a square wave, but I don't expose that in this code.  See the
// ds1307 for it's full capabilities.



#include <Wire.h>
#include <Time.h>
#include <TimeAlarms.h>
#include <DS1307RTC.h>

#include "Relay.h"
#include "WaterLevelProbe.h"
#include <WeeESP8266.h>
#include <doxygen.h>
#include "WifiManager.h"

#include <DallasTemperature.h>
#include <OneWire.h>
#include "TempProbe.h"


#define TEMPERATURE_PIN 3 // Temperature Pin
#define TEMPERATURE_PRECISION 9
TempProbeClass TempProbe;


//RTC_DS1307 RTC;
//#define DS1307_I2C_ADDRESS 0x68

WaterLevelProbeClass WaterLevelProbe(A0, A1, A2);

Relay HeatingRelay(A12, true);
Relay LightRelay(A9, true);
Relay FilterRelay(A10, false);
Relay PumpRelay(A11, true);

float targetTemperature;

AlarmID_t temperatureTimer;
AlarmID_t waterLevelTimer;
AlarmID_t startLightTimer;
AlarmID_t stopLightTimer;


time_t startLightTime = AlarmHMS(00, 00, 00);
time_t stopLightTime = AlarmHMS(23, 59, 00);;


WifiManager wifiManager;


void temperatureCheck() {
	float temp = TempProbe.GetTemp();
	Serial.print(" Temp : ");
	Serial.print(temp);
	Serial.println(" C ");
	if (temp < targetTemperature)
		HeatingRelay.turnOn();
	else
		HeatingRelay.turnOff();
	wifiManager.UpdateTemp(temp);
}

void waterLevelCheck() {
	WaterLevelProbe.checkLevels();
	if (WaterLevelProbe.state == TooLow) {
		Alarm.write(waterLevelTimer, 1);
		PumpRelay.turnOn();
	}
	else if (WaterLevelProbe.state == TooHigh) {
		PumpRelay.turnOff();
		Alarm.write(waterLevelTimer, 15);
	}
	Serial.println(WaterLevelProbe.state);
}

void turnLightOn() {
	LightRelay.turnOn();
}

void turnLightOff() {
	LightRelay.turnOff();
}


void setup()
{
	
	Serial.begin(9600);
	Serial.println("BEGIN");

	TempProbe.Init(TEMPERATURE_PIN);
	targetTemperature = 24.5;
	
	setSyncProvider(RTC.get);
	setSyncInterval(600);

	temperatureTimer = Alarm.timerRepeat(15, temperatureCheck);
	waterLevelTimer = Alarm.timerRepeat(10, waterLevelCheck);
	startLightTimer = Alarm.alarmRepeat(startLightTime, turnLightOn);
	stopLightTimer = Alarm.alarmRepeat(stopLightTime, turnLightOff);
	time_t time = now() - previousMidnight(now());

	Serial.println(startLightTime); 
	Serial.println(time);
	Serial.println(stopLightTime);


	if (time > startLightTime && time < stopLightTime) {
		LightRelay.turnOn();
			}
	else
		LightRelay.turnOff();

	wifiManager.init();
}


void loop()
{
	/*
	time_t now = RTC.get();
	Serial.print(" RTC Running ? ");
	Serial.println(RTC.chipPresent());
	Serial.print(year(), DEC);
	Serial.print('/');
	Serial.print(month(), DEC);
	Serial.print('/');
	Serial.print(day(), DEC);
	Serial.print(' ');
	Serial.print(hour(), DEC);
	Serial.print(':');
	Serial.print(minute(), DEC);
	Serial.print(':');
	Serial.print(second(), DEC);
	Serial.println();
	*/

	if (Serial.available()) {
		Serial3.write(Serial.read());
	}
	if (Serial3.available()) {
		Serial.write(Serial3.read());
	}
}