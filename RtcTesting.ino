 //
// Maurice Ribble 
// 4-17-2008
// http://www.glacialwanderer.com/hobbyrobotics

// This code tests the DS1307 Real Time clock on the Arduino board.
// The ds1307 works in binary coded decimal or BCD.  You can look up
// bcd in google if you aren't familior with it.  There can output
// a square wave, but I don't expose that in this code.  See the
// ds1307 for it's full capabilities.



#include <EEPROM.h>
#include <Wire.h>
#include <Time.h>
#include <TimeAlarms.h>
#include <DS1307RTC.h>

#include "Relay.h"
#include "WaterLevelProbe.h"
#include "WifiManager.h"

#include <DallasTemperature.h>
#include <OneWire.h>
#include "TempProbe.h"

#define SETTINGS_START 0;

#define TEMPERATURE_PIN 3 // Temperature Pin
#define TEMPERATURE_PRECISION 9

TempProbeClass TempProbe;


struct SettingsStruct {
	float targetTemperature;
	time_t startLightTime;
	time_t stopLightTime;
	time_t startCo2Time;
	time_t stopCo2Time;
} settings;

//RTC_DS1307 RTC;
//#define DS1307_I2C_ADDRESS 0x68

WaterLevelProbeClass WaterLevelProbe(A0, A2, A1);

Relay FilterRelay(A3, false);
Relay HeatingRelay(A4, true);
Relay PumpRelay(A5, true);
Relay LightRelay(A6, true);
Relay CO2Relay(A7, true);

AlarmID_t temperatureTimer;
AlarmID_t waterLevelTimer;
AlarmID_t startLightTimer;
AlarmID_t stopLightTimer;
AlarmID_t startCO2Timer;
AlarmID_t stopCO2Timer;

time_t startLightTime = AlarmHMS(12, 30, 00);
time_t stopLightTime = AlarmHMS(22, 00, 00);;

time_t startCo2Time = AlarmHMS(13, 30, 00);
time_t stopCo2Time = AlarmHMS(21, 00, 00);;

WifiManager wifiManager;

void saveSettings() {
	for (unsigned int t = 0; t<sizeof(settings); t++)
		EEPROM.write(t, *((char*)&settings + t));
}

void loadSettings() {
	for (unsigned int t = 0; t<sizeof(settings); t++)
		*((char*)&settings + t) = EEPROM.read(t);
}

uint8_t getRelaysStatus() {
	uint8_t status = 0;
	
	status |= (FilterRelay.isOn() << 0);
	status |= (HeatingRelay.isOn() << 1);
	status |= (PumpRelay.isOn() << 2);
	status |= (LightRelay.isOn() << 3);
	status |= (CO2Relay.isOn() << 4);

	return status;
}

void initSetting() {
	settings = {
		25.0f,
		AlarmHMS(12, 30, 00), AlarmHMS(22, 00, 00),
		AlarmHMS(13, 30, 00), AlarmHMS(21, 00, 00)
	};
	saveSettings();
}

void temperatureCheck() {
	if( TempProbe.GetTemp()) {
                int t = (int)(TempProbe.temperature * 100.0f);

                Serial.print("TempProbe s temp =  = ");
		Serial.println(t);

		wifiManager.UpdateStatus((int)(t), WaterLevelProbe.getState(), getRelaysStatus());
        }
}

void waterLevelCheck() {
	WaterLevelProbe.checkLevels();
	if (WaterLevelProbe.getState() == TooLow) {
		Alarm.write(waterLevelTimer, 1);
		PumpRelay.turnOn();
	}
	else if (WaterLevelProbe.getState() == TooHigh) {
		PumpRelay.turnOff();
		Alarm.write(waterLevelTimer, 15);
	}
	Serial.print("Relay Satus = ");
	Serial.println(WaterLevelProbe.getState());
}

void turnLightOn() {
	LightRelay.turnOn();
}

void turnLightOff() {
	LightRelay.turnOff();
}

void turnCo2On() {
	CO2Relay.turnOn();
}

void turnCo2Off() {
	CO2Relay.turnOff();
}

void printNow() {
	Serial.print("### RTC Running ? ");
	Serial.println(RTC.chipPresent());
	Serial.print(RTC.get());
}

const int ledPin = 12;
int ledState = LOW;             // ledState used to set the LED
long previousMillis = 0;        // will store last time LED was updated
long interval = 100;  


void setup()
{
        digitalWrite(13, LOW);
	pinMode(ledPin, OUTPUT);
	Serial.begin(9600);
	Serial.println("BEGIN");


	Serial.println("###  Initializing Settings");
	initSetting();
	Serial.print("Target Temp = "); Serial.println(settings.targetTemperature);
	Serial.print("Light Timers = Start : "); Serial.print(settings.startLightTime); Serial.print(" Stop : ");  Serial.println(settings.stopLightTime);
	Serial.print("CO2 Timers = Start : "); Serial.print(settings.startCo2Time); Serial.print(" Stop : ");  Serial.println(settings.stopCo2Time);

	TempProbe.Init(TEMPERATURE_PIN, settings.targetTemperature, &HeatingRelay);
	
	setSyncProvider(RTC.get);
	setSyncInterval(600);
	printNow();

	temperatureTimer = Alarm.timerRepeat(15, temperatureCheck);
	waterLevelTimer = Alarm.timerRepeat(10, waterLevelCheck);

	startLightTimer = Alarm.alarmRepeat(settings.startLightTime, turnLightOn);
	stopLightTimer = Alarm.alarmRepeat(settings.stopLightTime, turnLightOff);

	startCO2Timer = Alarm.alarmRepeat(settings.startCo2Time, turnCo2On);
	stopCO2Timer = Alarm.alarmRepeat(settings.stopCo2Time, turnCo2Off);

	time_t time = now() - previousMidnight(now());
	
	Serial.print("Now = "); Serial.print(now()); Serial.print("Time = "); Serial.println(time);

	if (time > settings.startLightTime && time < settings.startLightTime) {
		LightRelay.turnOn();
			}
	else
		LightRelay.turnOff();

	if (time > settings.startCo2Time && time < settings.stopCo2Time) {
		CO2Relay.turnOn();
	}
	else
		CO2Relay.turnOff();

        FilterRelay.turnOn();
	wifiManager.init();
}


void loop()
{
	if (Serial.available()) {
		Serial3.write(Serial.read());
	}
      if(Serial3.available()) {
        Serial.print("ESP -> ");
        while (Serial3.available()) 
            Serial.write(Serial3.read());
      }
  	Alarm.delay(10);
  
    unsigned long currentMillis = millis();
 
    if(currentMillis - previousMillis > interval) {
      // save the last time you blinked the LED 
      previousMillis = currentMillis;   
   
      // if the LED is off turn it on and vice-versa:
      if (ledState == LOW)
        ledState = HIGH;
      else
        ledState = LOW;
       
      // set the LED with the ledState of the variable:
      digitalWrite(ledPin, ledState);
   }
  
}
