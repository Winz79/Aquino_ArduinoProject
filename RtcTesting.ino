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

#include "CmdMessenger.h"


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

Relay Relays[8];

#define FILTER 0
#define HEATING 1
#define PUMP 2
#define LIGHT 3
#define CO2 4

void InitRelays() {
	Relays[FILTER] = Relay(A3, false);//Filter
	Relays[HEATING] = Relay(A4, true);//Heating
	Relays[PUMP] = Relay(A5, true);//Pump
	Relays[LIGHT] = Relay(A6, true);//Light
	Relays[CO2] = Relay(A7, true);//CO2
}

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
  for (unsigned int t = 0; t < sizeof(settings); t++)
    EEPROM.write(t, *((char*)&settings + t));
}

void loadSettings() {
  for (unsigned int t = 0; t < sizeof(settings); t++)
    *((char*)&settings + t) = EEPROM.read(t);
}

uint8_t getRelaysStatus() {
	uint8_t status = 0;
	for (uint8_t i = 0; i < sizeof(Relays) / sizeof(*Relays); i++) {
		status |= (Relays[i].isOn() << i);
	}
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
	if (TempProbe.GetTemp()) {
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
		Relays[PUMP].turnOn();
	}
	else if (WaterLevelProbe.getState() == TooHigh) {
		Relays[PUMP].turnOff();
		Alarm.write(waterLevelTimer, 15);
	}
	Serial.print("Relay Satus = ");
	Serial.println(WaterLevelProbe.getState());
}



void turnLightOn() {
	Relays[LIGHT].turnOn();
}

void turnLightOff() {
	Relays[LIGHT].turnOff();
}

void turnCo2On() {
	Relays[CO2].turnOn();
}

void turnCo2Off() {
  Relays[CO2].turnOff();
}

void printNow() {
	Serial.print("### RTC Running :  ");
	Serial.print(RTC.chipPresent());
	Serial.print("   Now = ");;
	Serial.println(RTC.get());
}


const char *monthName[12] = {
  "Jan", "Feb", "Mar", "Apr", "May", "Jun",
  "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
};

tmElements_t tm;

bool getTime(const char *str)
{
  int Hour, Min, Sec;

  if (sscanf(str, "%d:%d:%d", &Hour, &Min, &Sec) != 3) return false;
  tm.Hour = Hour;
  tm.Minute = Min;
  tm.Second = Sec;
  return true;
}

bool getDate(const char *str)
{
  char Month[12];
  int Day, Year;
  uint8_t monthIndex;

  if (sscanf(str, "%s %d %d", Month, &Day, &Year) != 3) return false;
  for (monthIndex = 0; monthIndex < 12; monthIndex++) {
    if (strcmp(Month, monthName[monthIndex]) == 0) break;
  }
  if (monthIndex >= 12) return false;
  tm.Day = Day;
  tm.Month = monthIndex + 1;
  tm.Year = CalendarYrToTm(Year);
  return true;
}

void SetupRtc() {
  bool parse = false;
  bool config = false;

  // get the date and time the compiler was run
  if (getDate(__DATE__) && getTime(__TIME__)) {
    parse = true;
    // and configure the RTC with this info
    if (RTC.write(tm)) {
      config = true;
    }
  }

  if (parse && config) {
    Serial.print("DS1307 configured Time=");
    Serial.print(__TIME__);
    Serial.print(", Date=");
    Serial.println(__DATE__);
  } else if (parse) {
    Serial.println("DS1307 Communication Error :-{");
    Serial.println("Please check your circuitry");
  } else {
    Serial.print("Could not parse info from the compiler, Time=\"");
    Serial.print(__TIME__);
    Serial.print("\", Date=\"");
    Serial.print(__DATE__);
    Serial.println("\"");
  }
}

CmdMessenger cmdMessenger = CmdMessenger(Serial);

enum
{
	kCommandList, // Command to request list of available commands
	kSetRelay, // Command to request led to be set in specific state
	kSetTime, // Command to request led to be set in to specific brightness
	kSetAlarm, // Command to request led status
};

// Callbacks define on which received commands we take action
void attachCommandCallbacks()
{
	// Attach callback methods
	cmdMessenger.attach(OnUnknownCommand);
	cmdMessenger.attach(kCommandList, OnCommandList);
	cmdMessenger.attach(kSetRelay, OnSetRelay);
	cmdMessenger.attach(kSetTime, OnSetTime);
	cmdMessenger.attach(kSetAlarm, OnSetAlarm);
}

// Show available commands
void ShowCommands()
{
	Serial.println("Available commands");
	Serial.println(" 0; - This command list");
	Serial.println(" 1,<Relay Number>,<Relay state>; - Set relay. 0 = off, 1 = on");
	Serial.print(" 2,<Time>; - Set Time ");
	Serial.println(" 3,<alarm nb>,<start/end>,<time>; - Set Alarm start or end");
}

// Called when a received command has no attached function
void OnUnknownCommand()
{
	Serial.println("This command is unknown!");
	ShowCommands();
}

void OnCommandList() {
	ShowCommands();
}

void OnSetRelay() {
	int16_t relay = cmdMessenger.readInt16Arg();
	bool on = cmdMessenger.readBoolArg();
	if (on) {
		Relays[relay].turnOn();
	}
	else {
		Relays[relay].turnOff();
	}
}

void OnSetTime() {

}

void OnSetAlarm() {
}

void setup()
{
  delay(2000);

  Serial.begin(9600);
  Serial.println("BEGIN");

  InitRelays();
  Relays[FILTER].turnOn();

  Serial.println("###  Initializing Settings");
  initSetting();
  Serial.print("Target Temp = "); Serial.println(settings.targetTemperature);
  Serial.print("Light Timers = Start : "); Serial.print(settings.startLightTime); Serial.print(" Stop : ");  Serial.println(settings.stopLightTime);
  Serial.print("CO2 Timers = Start : "); Serial.print(settings.startCo2Time); Serial.print(" Stop : ");  Serial.println(settings.stopCo2Time);


  TempProbe.Init(TEMPERATURE_PIN, settings.targetTemperature, &(Relays[HEATING]));

  setSyncProvider(RTC.get);
  setSyncInterval(600);
  
  time_t n = RTC.get();
  if (n == 0) {
	  SetupRtc();
  }

  temperatureTimer = Alarm.timerRepeat(15, temperatureCheck);
  waterLevelTimer = Alarm.timerRepeat(10, waterLevelCheck);

  startLightTimer = Alarm.alarmRepeat(settings.startLightTime, turnLightOn);
  stopLightTimer = Alarm.alarmRepeat(settings.stopLightTime, turnLightOff);
  startCO2Timer = Alarm.alarmRepeat(settings.startCo2Time, turnCo2On);
  stopCO2Timer = Alarm.alarmRepeat(settings.stopCo2Time, turnCo2Off);


	time_t time = now() - previousMidnight(now());
	Serial.print("Now = "); Serial.print(now()); Serial.print("Time = "); Serial.println(time);

	  if (time > settings.startLightTime && time < settings.startLightTime) {
		Relays[LIGHT].turnOn();
	  }
	  else
		Relays[LIGHT].turnOff();

	  if (time > settings.startCo2Time && time < settings.stopCo2Time) {
		  Relays[CO2].turnOn();
	  }
	  else
		  Relays[CO2].turnOff();

	  // Adds newline to every command
	  cmdMessenger.printLfCr();
	  // Attach my application's user-defined callback methods
	  attachCommandCallbacks();

	  wifiManager.init();
}


void loop()
{
      if(Serial3.available()) {
        Serial.print("ESP -> ");
        while (Serial3.available()) 
            Serial.write(Serial3.read());
      }

	  cmdMessenger.feedinSerialData();

	  Alarm.delay(10);
  
   
}
