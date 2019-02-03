
#include "XRSound.h"

typedef enum {
	// APU
	APU_START_ID = 0,
	APU_RUNNING_ID,
	APU_SHUTDOWN_ID,

	// Hydraulics
	CANOPY_ID,
	CARGO_ID,
	HOVER_DOOR_ID,
	RETRO_DOOR_ID,
	AIRBRAKE_ID,

	// Landing gear
	GEAR_WHINE_ID,
	GEAR_UP_ID,
	GEAR_UP_LOCKED_ID,
	GEAR_DOWN_ID,
	GEAR_DOWN_LOCKED_ID,
	SWITCH_ON_ID,
	SWITCH_OFF_ID
} const soundId;

const char* const APU_START_PATH = "XRSound/Default/APU Startup.wav";
const char* const APU_RUNNING_PATH = "XRSound/Default/APU Run.wav";
const char* const APU_SHUTDOWN_PATH = "XRSound/Default/APU Shutdown.wav";

const char* const HYDRAULIC_PATH = "XRSound/Default/Hydraulics1.wav";

const char* const GEAR_WHINE_PATH = "XRSound/Default/Gear Whine.wav";
const char* const GEAR_UP_PATH = "XRSound/Default/Gear Up.wav";
const char* const GEAR_UP_LOCKED_PATH = "XRSound/Default/Gear Up and Locked.wav";
const char* const GEAR_DOWN_PATH = "XRSound/Default/Gear Down.wav";
const char* const GEAR_DOWN_LOCKED_PATH = "XRSound/Default/Gear Down and Locked.wav";

const char* const SWITCH_ON_PATH = "XRSound/Default/SwitchOn1.wav";
const char* const SWITCH_OFF_PATH = "XRSound/Default/SwitchOff1.wav";
