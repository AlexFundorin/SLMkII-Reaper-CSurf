#pragma once

#ifndef CSURF_SL_H
#define CSURF_SL_H

#ifndef _CSURF_H_
#include "csurf.h"
#endif

// Standard libraries
#include <memory>
#include <functional>
#include <vector>
#include <regex>
#include <algorithm> 
#include <experimental/filesystem>
#include <cstdio>
#include <ctime>
#include <string>

#include <iostream>
#include <fstream>
#include <sstream>


// Third-party libraries
#include <fmt/format.h>
#include <json/json/json.h>
#include <timer/cxxtimer.hpp>

//#include <nlohmann/json.hpp>

// External classes/structs/functions
#include "routines/sl_convert.h"
#include "routines/sl_sysex.h"
#include "routines/sl_led.h"
#include "routines/sl_display.h"
#include "routines/sl_cmd.h"
#include "routines/sl_global.h"
#include "routines/sl_modeSelector.h"

#include "controls/sl_button.h"
#include "controls/sl_encoder.h"
#include "controls/sl_pot.h"
#include "controls/sl_fader.h"
#include "controls/sl_drumpad.h"
#include "controls/sl_speedDial.h"
#include "controls/sl_touchpad.h"

#include "gui/sl_gui.cpp"

#include "../src/reaper/rea_track.h"
#include "../src/reaper/rea_data.h"


namespace fs = std::experimental::filesystem;

int lcdUpdateTime = 100;	// Variable to set LCD update rate (default: 100 = 10 time per second)
bool mLed = true;	// Toggle display metronome beats at start - RS7 + C5 (default: false = not blinking at start)

// Global static definitions
const int SLGlobal::cc = 191;
const int SLGlobal::note = 159;

bool SLGlobal::automapStatus = false;
bool SLGlobal::transportLock = false;

int SLGlobal::trackMax = 0;		// Overall tracks amount in the project
int SLGlobal::trackBankStart = 1;			// Tracks bank
int SLGlobal::trackBankMax = 0;			// Tracks bank
int SLGlobal::trackBankSel = 0;		// Currently selected bank

MediaTrack* SLGlobal::track;
MediaTrack* SLGlobal::trackSel;
MediaTrack* SLGlobal::trackSelPrev;		// First track in previous track selection

int SLGlobal::trackFxMax = 0;
int SLGlobal::trackFxSel = 1;
int SLGlobal::trackFxSelPrev = 1;


int SLGlobal::trackFxPageMax = 0;
string SLGlobal::trackFxPageSel = "1";

int SLGlobal::rsCmdMain = 0;		// Active
int SLGlobal::rsCmdE = 0;		// Active
int SLGlobal::rsCmdP = 0;		// Active
int SLGlobal::rsCmdF = 0;			// Active
int SLGlobal::rsCmdSd = 0;			// Active


// Global static controls definitions
bool SLEncoder::touchGlobal = false;
bool SLPot::touchGlobal = false;
bool SLFader::touchGlobal = false;


// Check if file exists (can be open)
bool fexists(const string& filename) {
	ifstream ifile(filename.c_str());
	return (bool)ifile;
}

// Main CSurf class (might move it to the header file, later)
class CSurf_SL : public IReaperControlSurface
{
public:


	int m_midi_in_dev, m_midi_out_dev;
	int m_offset, m_size;
	midi_Output *midi_out;
	midi_Input *midi_in;

	// Create instance of sysex class
	SLGlobal slGlobal;
	SLSysex slSysex;
	SLLed slLed;
	SLDisplay slDisplay;
	SLCmd slCmd;
	SLModeSelector slModeSelector;
	SLGui slGui;

	// Juce GUI framework
	//DocumentWindow SLWindow;
	//SLWindow* g_plugin_chain_window;
	//SLWindow SLViewGUI;


	// Userplugins folder path 
	string userPath = format("{}\\UserPlugins\\", GetResourcePath()); // set plugmaps path
	string plugmapsPath;
	string plugmapsUserPath;
	string fxDevPath;
	string fxFilename;
	string fxDevUserPath;
	string fxUserFilename;
	Json::Value cfgFx;
	Json::Value projPlugs;
	Json::Value trPlugs;


	// Create controls instances (with CC numbers)

	// 112-119 - Ring Light, 120-127 - Ring Mode, (from PC)

	std::vector <SLEncoder> slE = { { 120, 112 },{ 121, 113 },{ 122, 114 },{ 123, 115 },{ 124, 116 },{ 125, 117 },{ 126, 118 },{ 127, 119 } };
	std::vector <SLPot> slP = { 8, 9, 10, 11, 12, 13, 14, 15 };
	std::vector <SLFader> slF = { 16, 17, 18, 19, 20, 21, 22, 23 };

	std::vector <SLButton> slA = { 24, 25, 26, 27, 28, 29, 30, 31 };
	std::vector <SLButton> slB = { 32, 33, 34, 35, 36, 37, 38, 39 };
	std::vector <SLButton> slC = { 40, 41, 42, 43, 44, 45, 46, 47 };
	std::vector <SLButton> slD = { 48, 49, 50, 51, 52, 53, 54, 55 };

	SLTouchpad slT{ 68, 69 };

	std::vector <SLButton> slTr = { 72, 73, 74, 75, 77, 76, 78, 79 };	// Transport: Rew, Fwd, Stop, Play, Loop, Rec. (7 and 8 aren't used. Only for loops integrity and led feedback)
	std::vector <SLButton> slAm = { 72, 73, 74, 75, 76, 77, 78, 79 };	// Automap: Learn, View, User, FX, Inst, Mixer, All LEDs off, TLock

	std::vector <SLButton> slRs = { 80, 81, 82, 83, 84, 85, 86, 87 }; // Row select
	std::vector <SLButton> slPg = { 88, 89, 90, 91 };					// Page (P1 and P2)

	SLSpeedDial slSd;

	// Midi notes
	std::vector <SLDrumpad> slDr = { 36, 37, 38, 39, 40, 41, 42, 43 };// Drumpads

	// Reaper tracks (all parameters)
	//std::vector <ReaTrack> reaTrack{1024};
	std::vector <ReaTrack> reaTrack = std::vector <ReaTrack>{ 1024 };

private:
	// Some WDL magic
	WDL_String descspace;
	char configtmp[1024];

	// Preferred surface mode at start: USER - 0, FX - 1, INST - 2, MIXER - 3
	int surfaceMode;
	int surfaceModeLast;

	// Row Select modes for A, B, C, D buttons, Drumpads, LCD
	int rsDefMain = 0;	// Default
	int rsCurMain;		// Selected

	// Row Select modes for encoders
	int rsDefE = 0;		// Default
	int rsCurE;			// Selected

	// Row Select modes for pots
	int rsDefP = 0;		// Default
	int rsCurP;			// Selected

	// Row Select modes for faders
	int rsDefF = 0;		// Default
	int rsCurF;			// Selected

	// Row Select modes for Speed Dial
	int rsDefSd = 0;	// Default
	int rsCurSd;		// Selected
	

	// Secondary modes
	bool secondModeUser = false;	// User mode
	bool secondModeFx = false;		// FX mode internal selector (true: FX parameters / false: FX chain
	bool secondModeInst = false;	// Inst mode
	bool secondModeMixer = false;	// Mixer mode

	// Variable to run startup function once
	bool runStart = true;

	// Variable to set LCD update rate (100 = 10 time per second)
	//int lcdUpdateTime = 100;
	string top_msg_old = "";
	string btm_msg_old = "";

	// Variable to set LCD update rate in live mode (100 = 10 time per second)
	//int ledVuUpdateTime = 50;





	const int tLockReq = 103;		// Request Transport-Lock status CC number


	/*
	Other CC Commands

	1 - Modwheel

	64 - Sustain pedal
	65 - Expression pedal

	92 - Alerts from surface:

	Msg3 = 0 - MIDI Channel Changed manually,
		   1 - Keyboard Transpose changed,
		   2 - Octave setting changed,
		   3 - AfterTouch changed,
		   4 - Velocity Curve changed.

	94, 95 - Tempo MS and LS bytes from SL to PC
	96 - LH Row - Select LEDs Bit map
	97 - RH Row - Select LEDs Bit map
	99 -  Echo CC Message Request (deprecated)
	*/



	// Row select hold and double clik
	int pressRs = 0;
	int dblRs = 0;

	// Track FX bank variables
	int slTrackFxBank = 0;		// tracks FX bank
	int slTrackFxBankSel = 0;		// tracks FX bank

	//bool mLed = false;		// Toggle display metronome beats at start (RS7 + C5)

	//void MetronomePattern(SLButton & a, SLButton & b);

	void slStartup();
	void banksDefine();

	void resizeTracks();
	

	// Parsers
	//void SysexParser(MIDI_event_t * evt);
	void MidiParser(MIDI_event_t * evt);
	void CmdParser();

	void FxParser();

	// Row select
	void RowSelectLed();
	void RowSelectCmd();
	// Learn, View
	void LearnViewLed();
	void LearnViewCmd();
	// Drumpads
	void DrumpadsCmd(MIDI_event_t * evt);
	// Mode selector
	void ModeSelectorLcd();
	void ModeSelectorLed();
	void ModeSelectorCmd();

	void LcdUserMode();

	void LcdFxMode();

	void LcdInstMode();

	void LcdMixerMode();

	// Actions upon incoming MIDI messages from SL
	void OnMIDIEvent(MIDI_event_t *evt);

public:
	// Class constructor
	CSurf_SL(int offset, int size, int indev, int outdev, int * errStats);
	// Class destructor
	~CSurf_SL(); // override


	const char *GetTypeString() { return "NovationSL"; }
	const char *GetDescString()
	{
		descspace.Set("Novation SL");
		char tmp[512];
		//sprintf_s(tmp, " (dev %d,%d)", m_midi_in_dev, m_midi_out_dev);
		sprintf_s(tmp, " (%s)", "fundorin");
		descspace.Append(tmp);
		return descspace.Get();
	}
	const char *GetConfigString() // string of configuration data
	{
		sprintf_s(configtmp, "%d %d %d %d", m_offset, m_size, m_midi_in_dev, m_midi_out_dev);
		return configtmp;
	}

	// I think this is executed upon opening/closing plugin settings in Reaper Preferences
	void CloseNoReset()
	{
		delete midi_out;
		delete midi_in;
		midi_out = 0;
		midi_in = 0;
		//runStart = true;
	}

	// automatically called 30x/sec or so
	void Run();

	// these will be called by the host when states change etc
	void SetSurfaceVolume(MediaTrack *trackid, double volume);
	void SetSurfacePan(MediaTrack *trackid, double pan);
	void SetSurfaceMute(MediaTrack *trackid, bool mute);
	void SetSurfaceSelected(MediaTrack *trackid, bool selected);
	void SetSurfaceSolo(MediaTrack *trackid, bool solo);
	void SetSurfaceRecArm(MediaTrack *trackid, bool recarm);

	void SetPlayState(bool play, bool pause, bool rec);
	void SetRepeatState(bool rep);

	void SetTrackTitle(MediaTrack *trackid, const char *title);
	void SetAutoMode(int mode);
};

#endif // !CSURF_SL_H