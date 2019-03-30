#pragma once

#ifndef SLLED_H
#define SLLED_H

// Third-party libraries
#include <fmt/format.h>
#include <json/json/json.h>
#include <timer/cxxtimer.hpp>


#include "../controls/sl_button.h"
#include "../controls/sl_encoder.h"
#include "../controls/sl_pot.h"
#include "../controls/sl_fader.h"
#include "../routines/sl_global.h"
#include "../routines/sl_convert.h"

//#include "../csurf.h"
//using namespace std;
//using namespace fmt;
//using namespace Json;



class SLLed {
public:

	//SLLed(midi_Output* out) : midi_out(out) {}

	//midi_Output* midi_out = nullptr;

	int ledVuUpdateTime = 25;	// Variable to set LED update rate in liveVU mode (default: 50 = 20 time per second)

	// Create instance of global variables object
	SLGlobal slGlobal;

	// Create LED liveVU update timer
	cxxtimer::Timer ledVuUpdateTimer;

	double MetronomeBeats();
	// Any two Button objects would indicate metronome strong/weak/triplet beats
	void MetronomePattern(midi_Output * out, bool reaPlay, bool reaPause, bool mLed, SLButton & a, SLButton & b);

	void FeedbackReset(midi_Output* out, vector<SLButton> &A, vector<SLButton> &B, vector<SLButton> &C, vector<SLButton> &D, vector<SLEncoder> &E, vector<SLButton> &Pg, vector<SLButton> &Am);

	int Ring(double value, int limitMin);

	void RingMode(int mode, int i, midi_Output* out, vector<SLEncoder> &E);

	void Transport(midi_Output * out, vector<SLButton>& C, vector<SLButton>& D, vector<SLButton>& Tr, bool reaPlay, bool reaPause, bool reaStop, bool reaRecord, bool reaRepeat, bool mLed);


	// SL.P1 (Page up and down) -------------------------
	void UserPg1(midi_Output * out, vector<SLButton>& Pg);
	void FxPg1(midi_Output* out, vector<SLButton> &Pg);
	void InstPg1(midi_Output* out, vector<SLButton> &Pg);
	void MixerPg1(midi_Output* out, vector<SLButton> &Pg);

	// SL.P2 (Page up and down) -----------------------
	void UserPg2(midi_Output * out, vector<SLButton>& Pg);
	void FxPg2(midi_Output* out, vector<SLButton> &Pg);
	void InstPg2(midi_Output* out, vector<SLButton> &Pg);
	void MixerPg2(midi_Output* out, vector<SLButton> &Pg);

	// SL.LED rings -----------------------------------
	void UserR(midi_Output * out, vector<SLEncoder>& E, vector<SLPot>& P, vector<SLFader>& F);
	void FxR(midi_Output* out, vector<SLEncoder> &E, vector<SLPot> &P, vector<SLFader> &F, Json::Value &trPlugs);
	void InstR(midi_Output* out, vector<SLEncoder> &E);
	void MixerR(midi_Output* out, vector<SLEncoder> &E, vector<SLPot> &P, vector<SLFader> &F, bool reaPlay, bool reaPause);


	// SL.Row A ---------------------------------------
	void UserA(midi_Output * out, vector<SLButton>& A);
	void FxA(midi_Output* out, vector<SLButton> &A);
	void InstA(midi_Output* out, vector<SLButton> &A);
	void MixerA(midi_Output* out, vector<SLButton> &A);

	// SL.Row B ---------------------------------------
	void UserB(midi_Output * out, vector<SLButton>& B);
	void FxB(midi_Output* out, vector<SLButton> &B, Json::Value &trPlugs);
	void InstB(midi_Output* out, vector<SLButton> &B);
	void MixerB(midi_Output* out, vector<SLButton> &B);

	// SL.Row C ---------------------------------------
	void UserC(midi_Output * out, vector<SLButton>& C);
	void FxC(midi_Output* out, vector<SLButton> &C, Json::Value &trPlugs);
	void InstC(midi_Output* out, vector<SLButton> &C);
	void MixerC(midi_Output* out, vector<SLButton> &C);

	// SL.Row D ---------------------------------------
	void UserD(midi_Output * out, vector<SLButton>& D);
	void FxD(midi_Output* out, vector<SLButton> &D, Json::Value &trPlugs);
	void InstD(midi_Output* out, vector<SLButton> &D);
	void MixerD(midi_Output* out, vector<SLButton> &D);

};

#endif // !SLLED_H