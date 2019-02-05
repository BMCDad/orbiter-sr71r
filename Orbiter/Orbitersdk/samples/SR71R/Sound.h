
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

	// Switches
	SWITCH_ON_ID,
	SWITCH_OFF_ID,

	// Autopilot
	AP_ON_ID,
	AP_OFF_ID,

	// Cross-feed and dump
	CRS_FED_MAIN_ID,
	CRS_FED_RCS_ID,
	FUEL_FLOW_ID,
	FUEL_DUMP_ID,
	MAIN_FULL_ID,
	RCS_FULL_ID,

	// LOX and LH2 supply
	LOX_SUPPLY_ID,
	LH2_SUPPLY_ID,
	LOX_FULL_ID,

	// Warnings
	MAIN_LOW_ID,
	MAIN_DEP_ID,
	RCS_LOW_ID,
	RCS_DEP_ID,
	LOX_LOW_ID,
	LOX_DEP_ID
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

const char* const AP_ON_PATH = "XRSound/Default/Autopilot On.wav";
const char* const AP_OFF_PATH = "XRSound/Default/Autopilot Off.wav";

const char* const CRS_FED_MAIN_PATH = "XRSound/Default/Cross-Feed Main.wav";
const char* const CRS_FED_RCS_PATH = "XRSound/Default/Cross-Feed RCS.wav";
const char* const FUEL_FLOW_PATH = "XRSound/Default/Fuel Flow.wav";
const char* const FUEL_DUMP_PATH = "XRSound/Default/Warning Fuel Dump.wav";
const char* const MAIN_FULL_PATH = "XRSound/Default/Main Fuel Tanks Full.wav";
const char* const RCS_FULL_PATH = "XRSound/Default/RCS Fuel Tanks Full.wav";

const char* const LOX_FULL_PATH = "XRSound/Default/LOX Tanks Full.wav";

const char* const MAIN_LOW_PATH = "XRSound/Default/Warning Main Fuel Low.wav";
const char* const MAIN_DEP_PATH = "XRSound/Default/Warning Main Fuel Depleted.wav";
const char* const RCS_LOW_PATH = "XRSound/Default/Warning RCS Fuel Low.wav";
const char* const RCS_DEP_PATH = "XRSound/Default/Warning RCS Fuel Depleted.wav";

const char* const LOX_LOW_PATH = "XRSound/Default/Warning Oxygen Low.wav";
const char* const LOX_DEP_PATH = "XRSound/Default/Warning Oxygen Depleted.wav";