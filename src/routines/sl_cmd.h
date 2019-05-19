#pragma once

#ifndef SLCMD_H
#define SLCMD_H

// Third-party libraries
#include <fmt/format.h>
//#include <json/json.h>
#include <json/json/json.h>
#include <timer/cxxtimer.hpp>


#include "../controls/sl_button.h"
#include "../controls/sl_encoder.h"
#include "../controls/sl_pot.h"
#include "../controls/sl_fader.h"
#include "../controls/sl_speedDial.h"
#include "../controls/sl_drumpad.h"

#include "../routines/sl_global.h"
#include "../routines/sl_convert.h"

class SLCmd
{
public:
	SLGlobal slGlobal;

	// Search action id by name and run it
	//template <typename T>
	void RunCommand(const string& cmd) { Main_OnCommand(NamedCommandLookup(cmd.c_str()), -1); }
	void RunCommand(const string& cmd, const int flag) { Main_OnCommand(NamedCommandLookup(cmd.c_str()), flag); }
	void RunCommand(const int& cmd) { Main_OnCommand(cmd, 0); }
	void RunCommand(const int& cmd, const int flag) { Main_OnCommand(cmd, flag); }

	void FxButton(string ctrlType, int fxid, int pid, Json::Value &trPlugs);

	void Transport(vector<SLButton>& Tr, vector<SLButton>& C, bool mLed);

	void UserSD(SLSpeedDial &Sd);
	void FxSD(SLSpeedDial & Sd);
	void InstSD(SLSpeedDial & Sd);
	void MixerSD(SLSpeedDial & Sd);

	void UserPg1(vector<SLButton>& Pg);
	void FxPg1(vector<SLButton> &Pg);
	void InstPg1(vector<SLButton> &Pg);
	void MixerPg1(vector<SLButton> &Pg);

	void UserPg2(vector<SLButton>& Pg);
	void FxPg2(vector<SLButton> &Pg);
	void InstPg2(vector<SLButton> &Pg);
	void MixerPg2(vector<SLButton> &Pg);

	void UserE(vector<SLEncoder>& E);
	void FxE(vector<SLEncoder>& E, Json::Value &trPlugs);
	void InstE(vector<SLEncoder>& E);
	void MixerE(vector<SLEncoder>& E);

	void UserP(vector<SLPot>& P);
	void FxP(vector<SLPot>& P, Json::Value &trPlugs);
	void InstP(vector<SLPot>& P);
	void MixerP(vector<SLPot>& P);

	void UserF(vector<SLFader>& F);
	void FxF(vector<SLFader>& F, Json::Value & trPlugs);
	void InstF(vector<SLFader>& F);
	void MixerF(vector<SLFader>& F);

	void UserA(vector<SLButton>& A);
	void FxA(vector<SLButton> &A);
	void InstA(vector<SLButton> &A);
	void MixerA(vector<SLButton> &A);

	void UserB(vector<SLButton>& B);
	void FxB(vector<SLButton> &B, Json::Value &trPlugs);
	void InstB(vector<SLButton> &B);
	void MixerB(vector<SLButton> &B);

	void UserC(vector<SLButton>& C);
	void FxC(vector<SLButton> &C, Json::Value &trPlugs);
	void InstC(vector<SLButton> &C);
	void MixerC(vector<SLButton> &C);

	void UserD(vector<SLButton>& D);
	void FxD(vector<SLButton> &D, Json::Value &trPlugs);
	void InstD(vector<SLButton> &D);
	void MixerD(vector<SLButton> &D);

	void UserDr(MIDI_event_t * evt, vector<SLDrumpad> &Dr, SLSpeedDial &Sd);

	void FxDr(MIDI_event_t * evt, vector<SLDrumpad> &Dr, SLSpeedDial &Sd);

	void InstDr(MIDI_event_t * evt, vector<SLDrumpad> &Dr, SLSpeedDial &Sd);

	void MixerDr(MIDI_event_t * evt, vector<SLDrumpad> & Dr, SLSpeedDial & Sd);

};

#endif 