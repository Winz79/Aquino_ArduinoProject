// WaterLevelProbe.h

#ifndef _WATERLEVELPROBE_h
#define _WATERLEVELPROBE_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

enum WaterLevelState
{
	TooLow = 0,
	OK = 1,
	TooHigh = 3
};

class WaterLevelProbeClass
{
         WaterLevelState state;
protected:
	 uint8_t highLvlPin;
	 uint8_t lowLvlPin;
	 uint8_t powerPin;
 public:
	 
	 WaterLevelProbeClass(uint8_t, uint8_t , uint8_t);
	 void checkLevels();
         uint8_t getState();
};
#endif

