#include "sl_led.h"


double SLLed::MetronomeBeats()
{
	int measures = 1;
	int cur;
	double fullbeats = 1;
	int cdenom;
	double mea;
	mea = TimeMap2_timeToBeats(0, GetPlayPosition(), &measures, &cur, &fullbeats, &cdenom);

	return mea;
}

void SLLed::MetronomePattern(midi_Output* out, bool reaPlay, bool reaPause, bool mLed, SLButton &a, SLButton &b) {
	// Metronome indicator whhen metronome on and play (D6/D7 - odd/even beat)
	//if (reaPlay && !reaPause && GetToggleCommandState(40364)) {
	if (reaPlay && !reaPause && mLed) {
		char pattern[128];
		int size = (int)strlen(pattern);

		TimeMap_GetMetronomePattern(0, GetPlayPosition(), pattern, size);

		int measure = (int)MetronomeBeats();
		int halfMeasure = (int)(MetronomeBeats() * 10) % 10;
		int tripMeasure = (int)(MetronomeBeats() * 1000) % 1000;
		bool halfbeat;
		bool triplet;

		(halfMeasure < 5) ? halfbeat = true : halfbeat = false;
		(tripMeasure < 167 || (333 < tripMeasure && tripMeasure < 501) || (666 < tripMeasure && tripMeasure < 835)) ? triplet = true : triplet = false;

		if (pattern[measure] - 48 == 1 && halfbeat) {
			if (!a.led) {
				a.led = !a.led;
				out->Send(slGlobal.cc, a.cc, a.led, 0);
			}
		}
		else if (a.led) {
			a.led = !a.led;
			out->Send(slGlobal.cc, a.cc, a.led, 0);
		}

		if ((pattern[measure] - 48 == 2 && halfbeat) || (pattern[measure] - 48 == 3 && triplet)) {
			if (!b.led) {
				b.led = !b.led;
				out->Send(slGlobal.cc, b.cc, b.led, 0);
			}
		}
		else if (b.led) {
			b.led = !b.led;
			out->Send(slGlobal.cc, b.cc, b.led, 0);
		}
	}
	// Turn off two last buttons in D row (when stop or metronome off)
	else
	{
		if (a.led) {
			a.led = 0;
			out->Send(slGlobal.cc, a.cc, a.led, 0);
		}
		if (b.led) {
			b.led = 0;
			out->Send(slGlobal.cc, b.cc, b.led, 0);
		}
	}
}

void SLLed::FeedbackReset(midi_Output* out, vector<SLButton> &A, vector<SLButton> &B, vector<SLButton> &C, vector<SLButton> &D, vector<SLEncoder> &E, vector<SLButton> &Pg, vector<SLButton> &Am) {
	for (int i = 0; i < 8; i++) {
		A[i].led = 0;
		B[i].led = 0;
		C[i].led = 0;
		D[i].led = 0;
		D.at(i).led = 0;
		//Rs[i].led = 0;

		RingMode(0, i, out, E);
		E[i].led = 0;
	}
	for (int i = 0; i < 4; i++) {
		Pg[i].led = 0;
	}
}

// Enocoder ring utilities
int SLLed::Ring(double value, int limitMin) { // LED Encoder ring convert from 0-127 to 0-11
	int outVal = (int)__maxVal(limitMin, int(ceil((value / 127) * 11)));
	return outVal;
}
void SLLed::RingMode(int mode, int i, midi_Output* out, vector<SLEncoder> &E) { // LED Encoder ring mode
	// Changes ring display mode and also sets flag for last used mode
	out->Send(slGlobal.cc, E[i].cc, mode * 16, 0);
	E[i].mode = mode;
}

// ---------- TRANSPORT -----------------------------
void SLLed::Transport(midi_Output* out, vector<SLButton> &C, vector<SLButton> &D, vector<SLButton> &Tr, bool reaPlay, bool reaPause, bool reaStop, bool reaRecord, bool reaRepeat, bool mLed) {
	if (SLGlobal::transportLock) {
		// Row C
		if (SLGlobal::rsCmdMain == 0) {
			if (C[0].led == C[0].hold) {
				C[0].led = !C[0].led;
				out->Send(slGlobal.cc, C[0].cc, C[0].led, 0);
			}
			if (C[1].led == C[1].hold) {
				C[1].led = !C[1].led;
				out->Send(slGlobal.cc, C[1].cc, C[1].led, 0);
			}
			if (C[2].led != reaStop) {
				C[2].led = reaStop;
				out->Send(slGlobal.cc, C[2].cc, C[2].led, 0);
			}
			if (C[3].led != C[3].hold) {
				C[3].led = C[3].led;
				out->Send(slGlobal.cc, C[3].cc, C[3].led, 0);
			}
			if (C[4].led == C[4].hold) {
				C[4].led = !C[4].led;
				out->Send(slGlobal.cc, C[4].cc, C[4].led, 0);
			}
			if (C[5].led == C[5].hold) {
				C[5].led = !C[5].led;
				out->Send(slGlobal.cc, C[5].cc, C[5].led, 0);
			}
			if (C[6].led == C[6].hold) {
				C[6].led = !C[6].led;
				out->Send(slGlobal.cc, C[6].cc, C[6].led, 0);
			}
			if (C[7].led != (bool)GetToggleCommandState(40364)) { // Metronome State
				C[7].led = !C[7].led;
				out->Send(slGlobal.cc, C[7].cc, C[7].led, 0);
			}
		}
		if (SLGlobal::rsCmdMain == 6) {
			if (C[7].led == C[7].hold) { // Metronome LED indication State
				C[7].led = !C[7].led;
				out->Send(slGlobal.cc, C[7].cc, C[7].led, 0);
			}
		}
		if (SLGlobal::rsCmdMain == 7) {
			if (C[7].led != mLed) { // Metronome LED indication State
				C[7].led = !C[7].led;
				out->Send(slGlobal.cc, C[7].cc, C[7].led, 0);
			}
		}

		// Row D
		if (SLGlobal::rsCmdMain == 0) {

			if (D[0].led == D[0].hold || D[0].led == Tr[0].hold) {
				D[0].led = !D[0].led;
				out->Send(slGlobal.cc, D[0].cc, D[0].led, 0);
			}
			if (D[1].led == D[1].hold || D[1].led == Tr[1].hold) {
				D[1].led = !D[1].led;
				out->Send(slGlobal.cc, D[1].cc, D[1].led, 0);
			}
			if (D[2].led != reaStop) {
				D[2].led = reaStop;
				out->Send(slGlobal.cc, D[2].cc, D[2].led, 0);
			}
			if (D[3].led != reaPlay) {
				D[3].led = reaPlay;
				out->Send(slGlobal.cc, D[3].cc, D[3].led, 0);
			}
			if (D[4].led != reaRepeat) {
				D[4].led = reaRepeat;
				out->Send(slGlobal.cc, D[4].cc, D[4].led, 0);
			}
			if (D[5].led != reaRecord) {
				D[5].led = reaRecord;
				out->Send(slGlobal.cc, D[5].cc, D[5].led, 0);
			}

			// last two transport LEDs would indicate metronome strong/weak/triplet beats
			MetronomePattern(out, reaPlay, reaPause, mLed, D[6], D[7]);
		}
	}
}


// SL.P1 (Page up and down) -------------------------
void SLLed::UserPg1(midi_Output* out, vector<SLButton> &Pg) {
	int tr = CSurf_TrackToID(GetSelectedTrack(0, 0), false);

	//P1 Up button
	if (tr == 1 && Pg[0].led) {
		Pg[0].led = !Pg[0].led;
		out->Send(slGlobal.cc, Pg[0].cc, Pg[0].led, 0);
	}
	else if (tr > 1 && !Pg[0].led) {
		Pg[0].led = !Pg[0].led;
		out->Send(slGlobal.cc, Pg[0].cc, Pg[0].led, 0);
	}

	//P1 Down button
	if (tr == slGlobal.trackMax && Pg[1].led) {
		Pg[1].led = !Pg[1].led;
		out->Send(slGlobal.cc, Pg[1].cc, Pg[1].led, 0);
	}
	else if (tr != slGlobal.trackMax && !Pg[1].led) {
		Pg[1].led = !Pg[1].led;
		out->Send(slGlobal.cc, Pg[1].cc, Pg[1].led, 0);
	}
}
void SLLed::FxPg1(midi_Output* out, vector<SLButton> &Pg) {
	int fx = slGlobal.trackFxSel;

	//P1 Up button
	if (fx == 1 && Pg[0].led) {
		Pg[0].led = !Pg[0].led;
		out->Send(slGlobal.cc, Pg[0].cc, Pg[0].led, 0);
	}
	else if (fx > 1 && !Pg[0].led) {
		Pg[0].led = !Pg[0].led;
		out->Send(slGlobal.cc, Pg[0].cc, Pg[0].led, 0);
	}

	//P1 Down button
	if (fx == slGlobal.trackFxMax && Pg[1].led) {
		Pg[1].led = !Pg[1].led;
		out->Send(slGlobal.cc, Pg[1].cc, Pg[1].led, 0);
	}
	else if (fx != slGlobal.trackFxMax && !Pg[1].led) {
		Pg[1].led = !Pg[1].led;
		out->Send(slGlobal.cc, Pg[1].cc, Pg[1].led, 0);
	}
}
void SLLed::InstPg1(midi_Output* out, vector<SLButton> &Pg) {
	int tr = CSurf_TrackToID(GetSelectedTrack(0, 0), false);

	//P1 Up button
	if (slGlobal.trackBankStart == 1 && Pg[0].led) {
		Pg[0].led = !Pg[0].led;
		out->Send(slGlobal.cc, Pg[0].cc, Pg[0].led, 0);
	}
	else if (slGlobal.trackBankStart != 1 && !Pg[0].led) {
		Pg[0].led = !Pg[0].led;
		out->Send(slGlobal.cc, Pg[0].cc, Pg[0].led, 0);
	}

	//P1 Down button
	if (slGlobal.trackBankStart + 7 >= slGlobal.trackMax && Pg[1].led) {
		Pg[1].led = !Pg[1].led;
		out->Send(slGlobal.cc, Pg[1].cc, Pg[1].led, 0);
	}
	else if (slGlobal.trackBankStart + 7 < slGlobal.trackMax && !Pg[1].led) {
		Pg[1].led = !Pg[1].led;
		out->Send(slGlobal.cc, Pg[1].cc, Pg[1].led, 0);
	}
}
void SLLed::MixerPg1(midi_Output* out, vector<SLButton> &Pg) {
	int tr = CSurf_TrackToID(GetSelectedTrack(0, 0), false);

	//P1 Up button
	if (slGlobal.trackBankStart == 1 && Pg[0].led) {
		Pg[0].led = !Pg[0].led;
		out->Send(slGlobal.cc, Pg[0].cc, Pg[0].led, 0);
	}
	else if (slGlobal.trackBankStart != 1 && !Pg[0].led) {
		Pg[0].led = !Pg[0].led;
		out->Send(slGlobal.cc, Pg[0].cc, Pg[0].led, 0);
	}
	//P1 Down button
	if (slGlobal.trackBankStart + 7 >= slGlobal.trackMax && Pg[1].led) {
		Pg[1].led = !Pg[1].led;
		out->Send(slGlobal.cc, Pg[1].cc, Pg[1].led, 0);
	}
	else if (slGlobal.trackBankStart + 7 < slGlobal.trackMax && !Pg[1].led) {
		Pg[1].led = !Pg[1].led;
		out->Send(slGlobal.cc, Pg[1].cc, Pg[1].led, 0);
	}
}

// SL.P2 (Page up and down) -----------------------
void SLLed::UserPg2(midi_Output* out, vector<SLButton> &Pg) {}
void SLLed::FxPg2(midi_Output* out, vector<SLButton> &Pg) {
	int fxPage = stoi(slGlobal.trackFxPageSel);

	//P2 Up button
	if (fxPage == 1) {
		if (Pg[2].led) {
			Pg[2].led = !Pg[2].led;
			out->Send(slGlobal.cc, Pg[2].cc, Pg[2].led, 0);
		}
	}
	else {
		if (!Pg[2].led) {
			Pg[2].led = !Pg[2].led;
			out->Send(slGlobal.cc, Pg[2].cc, Pg[2].led, 0);
		}
	}

	//P2 Down button
	if (fxPage == slGlobal.trackFxPageMax) {
		if (Pg[3].led) {
			Pg[3].led = !Pg[3].led;
			out->Send(slGlobal.cc, Pg[3].cc, Pg[3].led, 0);
		}
	}
	else {
		if (!Pg[3].led) {
			Pg[3].led = !Pg[3].led;
			out->Send(slGlobal.cc, Pg[3].cc, Pg[3].led, 0);
		}
	}
}
void SLLed::InstPg2(midi_Output* out, vector<SLButton> &Pg) {
	//P2 Up button
	if (!slGlobal.trackBankSel && Pg[2].led) {
		out->Send(slGlobal.cc, Pg[2].cc, 0, 0);
		Pg[2].led = false;
	}
	else if (slGlobal.trackBankSel && !Pg[2].led) {
		out->Send(slGlobal.cc, Pg[2].cc, 1, 0);
		Pg[2].led = true;
	}
	//P2 Down button
	if (slGlobal.trackBankSel == slGlobal.trackBankMax && Pg[3].led) {
		out->Send(slGlobal.cc, Pg[3].cc, 0, 0);
		Pg[3].led = false;
	}
	else if (slGlobal.trackBankSel != slGlobal.trackBankMax && !Pg[3].led) {
		out->Send(slGlobal.cc, Pg[3].cc, 1, 0);
		Pg[3].led = true;
	}
}
void SLLed::MixerPg2(midi_Output* out, vector<SLButton> &Pg) {
	//P2 Up button
	if (slGlobal.trackBankStart == 1 && Pg[2].led) {
		Pg[2].led = !Pg[2].led;
		out->Send(slGlobal.cc, Pg[2].cc, Pg[2].led, 0);
	}
	else if (slGlobal.trackBankStart != 1 && !Pg[2].led) {
		Pg[2].led = !Pg[2].led;
		out->Send(slGlobal.cc, Pg[2].cc, Pg[2].led, 0);
	}
	//P2 Down button
	if (slGlobal.trackBankStart + 7 >= slGlobal.trackMax && Pg[3].led) {
		Pg[3].led = !Pg[3].led;
		out->Send(slGlobal.cc, Pg[3].cc, Pg[3].led, 0);
	}
	else if (slGlobal.trackBankStart + 7 < slGlobal.trackMax && !Pg[3].led) {
		Pg[3].led = !Pg[3].led;
		out->Send(slGlobal.cc, Pg[3].cc, Pg[3].led, 0);
	}
}

// SL.LED rings -----------------------------------
void SLLed::UserR(midi_Output* out, vector<SLEncoder> &E, vector<SLPot> &P, vector<SLFader> &F) {
	for (int i = 0; i < 8; i++) {
		if (E[i].led != 0) {
			E[i].led = 0;
			RingMode(0, i, out, E);
			out->Send(slGlobal.cc, E[i].ring, E[i].led, 0);
		}
	}
}
void SLLed::FxR(midi_Output* out, vector<SLEncoder> &E, vector<SLPot> &P, vector<SLFader> &F, Json::Value &trPlugs) {
	// Track is present
	if (slGlobal.trackSel) {
		int fxNum = slGlobal.trackFxSel - 1;
		string fxNumStr = to_string(fxNum);

		string page = slGlobal.trackFxPageSel;

		for (int i = 0; i < 8; i++) {
			// Pots (FX dry/wet level) 
			if (SLFader::touchGlobal) {

				string pid = to_string(i);

				string ctrlType;
				(trPlugs[fxNumStr]["Page"][page]["ctrlF"]) ? ctrlType = "ctrlF" : ctrlType = "default";
				string ctrlMode = to_string(slGlobal.rsCmdF);

				if (trPlugs[fxNumStr]["Page"][page][ctrlType][ctrlMode][pid]["fNum"]) {
					int parm = int(TrackFX_GetParamNormalized(slGlobal.trackSel, slGlobal.trackFxSel - 1, trPlugs[fxNumStr]["Page"][page][ctrlType][ctrlMode][pid].get("fNum", 0).asInt()) * 11);

					if (E[i].led != parm || E[i].mode != 0) {
						E[i].led = parm;
						RingMode(0, i, out, E);
						out->Send(slGlobal.cc, E[i].ring, E[i].led, 0);
					}
				}
				else {
					if (E[i].led) {
						E[i].led = !E[i].led;
						RingMode(0, i, out, E);
						out->Send(slGlobal.cc, E[i].ring, E[i].led, 0);
					}
				}
			}
			else if (SLPot::touchGlobal) {
				string pid = to_string(i);

				string ctrlType;
				(trPlugs[fxNumStr]["Page"][page]["ctrlP"]) ? ctrlType = "ctrlP" : ctrlType = "default";
				string ctrlMode = to_string(slGlobal.rsCmdP);

				if (trPlugs[fxNumStr]["Page"][page][ctrlType][ctrlMode][pid]["fNum"]) {
					int parm = int(TrackFX_GetParamNormalized(slGlobal.trackSel, slGlobal.trackFxSel - 1, trPlugs[fxNumStr]["Page"][page][ctrlType][ctrlMode][pid].get("fNum", 0).asInt()) * 11);

					if (E[i].led != parm || E[i].mode != 4) {
						E[i].led = parm;
						RingMode(4, i, out, E);
						out->Send(slGlobal.cc, E[i].ring, E[i].led, 0);
					}
				}
				else {
					if (E[i].led) {
						E[i].led = !E[i].led;
						RingMode(0, i, out, E);
						out->Send(slGlobal.cc, E[i].ring, E[i].led, 0);
					}
				}
			}
			else if (slGlobal.rsCmdE == 1) {
				// FX exists
				if (i < TrackFX_GetCount(slGlobal.trackSel)) {

					int dryWetParm = TrackFX_GetNumParams(slGlobal.trackSel, i) - 1;
					int curWet = int(TrackFX_GetParamNormalized(slGlobal.trackSel, i, dryWetParm) * 11);

					if (E[i].led != curWet || E[i].mode != 4) {
						E[i].led = curWet;
						RingMode(4, i, out, E);
						out->Send(slGlobal.cc, E[i].ring, E[i].led, 0);
					}
				}
				// FX doesn't exist
				else {
					if (E[i].led) {
						E[i].led = !E[i].led;
						RingMode(0, i, out, E);
						out->Send(slGlobal.cc, E[i].ring, E[i].led, 0);
					}
				}
			}
			else {
				string pid = to_string(i);
				string ctrlType = "default";
				string ctrlMode = to_string(slGlobal.rsCmdE);

				if (trPlugs[fxNumStr]["Page"][page][ctrlType][ctrlMode][pid]["fNum"]) {
					int parm = int(TrackFX_GetParamNormalized(slGlobal.trackSel, slGlobal.trackFxSel - 1, trPlugs[fxNumStr]["Page"][page][ctrlType][ctrlMode][pid].get("fNum", 0).asInt()) * 11);

					if (E[i].led != parm || E[i].mode != 0) {
						E[i].led = parm;
						RingMode(0, i, out, E);
						out->Send(slGlobal.cc, E[i].ring, E[i].led, 0);
					}
				}
				else {
					if (E[i].led) {
						E[i].led = !E[i].led;
						RingMode(0, i, out, E);
						out->Send(slGlobal.cc, E[i].ring, E[i].led, 0);
					}
				}
			}
		}
	}
}
void SLLed::InstR(midi_Output* out, vector<SLEncoder> &E) {
	for (int i = 0; i < 8; i++) {
		if (E[i].led != 0) {
			E[i].led = 0;
			RingMode(0, i, out, E);
			out->Send(slGlobal.cc, E[i].ring, E[i].led, 0);
		}
	}
}
void SLLed::MixerR(midi_Output* out, vector<SLEncoder> &E, vector<SLPot> &P, vector<SLFader> &F, bool reaPlay, bool reaPause) {
	for (int i = 0; i < 8; i++) {
		int tid = slGlobal.trackBankStart + i;
		MediaTrack *tr = CSurf_TrackFromID(tid, false);

		if (tr) { // Track is present
			if (SLEncoder::touchGlobal) { // Encoders (volume level) 
				int curVol = Ring(volToChar(GetMediaTrackInfo_Value(tr, "D_VOL")), 0);

				if (E[i].led != curVol || E[i].mode != 0) {
					E[i].led = curVol;
					RingMode(0, i, out, E);
					out->Send(slGlobal.cc, E[i].ring, E[i].led, 0);
				}
			}
			else if (SLPot::touchGlobal) { // Pots (panorama level) 
				if (slGlobal.rsCmdP == 0) {
					int curPan = Ring(panToChar(GetMediaTrackInfo_Value(tr, "D_PAN")), 1);

					if (E[i].led != curPan || E[i].mode != 4) {
						E[i].led = curPan;
						RingMode(4, i, out, E);
						out->Send(slGlobal.cc, E[i].ring, E[i].led, 0);
					}
				}
				if (slGlobal.rsCmdP == 7) {
					int curPan = Ring(panToChar(GetMediaTrackInfo_Value(tr, "D_WIDTH")), 1);

					if (E[i].led != curPan || E[i].mode != 4) {
						E[i].led = curPan;
						RingMode(4, i, out, E);
						out->Send(slGlobal.cc, E[i].ring, E[i].led, 0);
					}
				}
			}
			else if (SLFader::touchGlobal) { // Faders (volume level) 
				int curVol = Ring(volToChar(GetMediaTrackInfo_Value(tr, "D_VOL")), 0);

				if (E[i].led != curVol || E[i].mode != 0) {
					E[i].led = curVol;
					RingMode(0, i, out, E);
					out->Send(slGlobal.cc, E[i].ring, E[i].led, 0);
				}
			}
			// Live VU meters 
			else {
				// VU on play
				if (reaPlay && !reaPause) {
					//if (reaPlay && !reaPause && ledRingUpdateTimer.count() > ledRingUpdateTime) {
					int curVU = Ring((volToChar(Track_GetPeakInfo(tr, 0)) + volToChar(Track_GetPeakInfo(tr, 1))) / 2, 0);
					if (E[i].led != curVU && ledVuUpdateTimer.count() > ledVuUpdateTime) {
						E[i].led = curVU;
						RingMode(0, i, out, E);
						out->Send(slGlobal.cc, E[i].ring, E[i].led, 0);
					}
				}
				// Volume on stop/pause
				else {
					int curVol = Ring(volToChar(GetMediaTrackInfo_Value(tr, "D_VOL")), 0);
					if (E[i].led != curVol || E[i].mode != 0) { // Encoders (static level) 
						E[i].led = curVol;
						RingMode(0, i, out, E);
						out->Send(slGlobal.cc, E[i].ring, E[i].led, 0);
					}
				}
			}
		}
		else { // Track doesn't exist
			if (E[i].led != 0) {
				E[i].led = 0;
				RingMode(0, i, out, E);
				out->Send(slGlobal.cc, E[i].ring, E[i].led, 0);
			}
		}
	}
	// Reset live VU refresh timer
	if (ledVuUpdateTimer.count() > ledVuUpdateTime) {
		ledVuUpdateTimer.reset();				// Reset and stop timer
		ledVuUpdateTimer.start();				// Start timer
	}
}

// SL.Row A ---------------------------------------
void SLLed::UserA(midi_Output* out, vector<SLButton> &A) {
	for (int i = 0; i < 8; i++) {
		if (A[i].led) {
			A[i].led = !A[i].led;
			out->Send(slGlobal.cc, A[i].cc, A[i].led, 0);
		}
	}
}
void SLLed::FxA(midi_Output* out, vector<SLButton> &A) {
	if (SLGlobal::rsCmdMain == 0) {
		for (int i = 0; i < 8; i++) {
			if (i == slGlobal.trackFxSel - 1) {
				if (!A[i].led) {
					A[i].led = !A[i].led;
					out->Send(slGlobal.cc, A[i].cc, A[i].led, 0);
				}
			}
			else if (A[i].led) {
				A[i].led = !A[i].led;
				out->Send(slGlobal.cc, A[i].cc, A[i].led, 0);
			}
		}
	}

	if (SLGlobal::rsCmdMain == 7) {
		//for (int i = 0; i < 8; i++) {
		//	if (A[i].led != TrackFX_GetEnabled(slGlobal.trackSel, i)) {
		//		A[i].led = !A[i].led;
		//		out->Send(slGlobal.cc, A[i].cc, A[i].led, 0);
		//	}
		//}
	}
}
void SLLed::InstA(midi_Output* out, vector<SLButton> &A) {
	if (A[0].led) {
		A[0].led = !A[0].led;
		out->Send(slGlobal.cc, A[0].cc, A[0].led, 0);
	}
	if (A[1].led) {
		A[1].led = !A[1].led;
		out->Send(slGlobal.cc, A[1].cc, A[1].led, 0);
	}
	if (A[2].led) {
		A[2].led = !A[2].led;
		out->Send(slGlobal.cc, A[2].cc, A[2].led, 0);
	}
	if (A[3].led) {
		A[3].led = !A[3].led;
		out->Send(slGlobal.cc, A[3].cc, A[3].led, 0);
	}
	// Markers: Edit time signature marker near cursor
	if (A[4].led == A[4].hold) {
		A[4].led = !A[4].led;
		out->Send(slGlobal.cc, A[4].cc, A[4].led, 0);
	}
	// Time selection: Set start point
	if (A[5].led == A[5].hold) {
		A[5].led = !A[5].led;
		out->Send(slGlobal.cc, A[5].cc, A[5].led, 0);
	}
	// Time selection: Set end point
	if (A[6].led == A[6].hold) {
		A[6].led = !A[6].led;
		out->Send(slGlobal.cc, A[6].cc, A[6].led, 0);
	}

	if (A[7].led == A[7].hold) {
		A[7].led = !A[7].led;
		out->Send(slGlobal.cc, A[7].cc, A[7].led, 0);
	}

	//for (int i = 0; i < 8; i++) {
	//	if (A[i].led) {
	//		A[i].led = !A[i].led;
	//		out->Send(slGlobal.cc, A[i].cc, A[i].led, 0);
	//	}
	//}
}
void SLLed::MixerA(midi_Output* out, vector<SLButton> &A) {
	for (int i = 0; i < 8; i++) {
		int tid = slGlobal.trackBankStart + i;
		MediaTrack *tr = CSurf_TrackFromID(tid, false);

		if (slGlobal.rsCmdMain == 0 && A[i].led != (bool)GetMediaTrackInfo_Value(tr, "I_SELECTED")) {
			A[i].led = !A[i].led;
			out->Send(slGlobal.cc, A[i].cc, A[i].led, 0);
		}

		// Show track in Track view
		else if (slGlobal.rsCmdMain == 2 && A[i].led != (bool)GetMediaTrackInfo_Value(tr, "B_SHOWINTCP")) {
			A[i].led = !A[i].led;
			out->Send(slGlobal.cc, A[i].cc, A[i].led, 0);
		}

		// Toggle PHASE for track in current bank
		else if (slGlobal.rsCmdMain == 6 && A[i].led != (bool)GetMediaTrackInfo_Value(tr, "B_PHASE")) {
			A[i].led = !A[i].led;
			out->Send(slGlobal.cc, A[i].cc, A[i].led, 0);
		}

		// MUTE track
		else if (slGlobal.rsCmdMain == 7 && A[i].led != (bool)GetMediaTrackInfo_Value(tr, "B_MUTE")) {
			A[i].led = !A[i].led;
			out->Send(slGlobal.cc, A[i].cc, A[i].led, 0);
		}
	}
}

// SL.Row B ---------------------------------------
void SLLed::UserB(midi_Output* out, vector<SLButton> &B) {
	for (int i = 0; i < 8; i++) {
		if (B[i].led) {
			B[i].led = !B[i].led;
			out->Send(slGlobal.cc, B[i].cc, B[i].led, 0);
		}
	}
}
void SLLed::FxB(midi_Output* out, vector<SLButton> &B, Json::Value &trPlugs) {
	for (int i = 0; i < 8; i++) {
		int fxNum = slGlobal.trackFxSel - 1;
		string pid = to_string(i);
		string ctrlType = "ctrlB";
		string ctrlNum = to_string(SLGlobal::rsCmdMain);

		if (trPlugs[to_string(fxNum)][ctrlType][ctrlNum][pid]["fNum"]) {
			if (B[i].led != bool(round(TrackFX_GetParamNormalized(slGlobal.trackSel, fxNum, trPlugs[to_string(fxNum)][ctrlType][ctrlNum][pid].get("fNum", 0).asInt()) * 127))) {
				B[i].led = !B[i].led;
				out->Send(slGlobal.cc, B[i].cc, B[i].led, 0);
			}
		}
		else if (B[i].led) {
			B[i].led = !B[i].led;
			out->Send(slGlobal.cc, B[i].cc, B[i].led, 0);
		}
	}
}
void SLLed::InstB(midi_Output* out, vector<SLButton> &B) {
	if (B[0].led == B[0].hold) {
		B[0].led = !B[0].led;
		out->Send(slGlobal.cc, B[0].cc, B[0].led, 0);
	}
	if (B[1].led == B[1].hold) {
		B[1].led = !B[1].led;
		out->Send(slGlobal.cc, B[1].cc, B[1].led, 0);
	}
	if (B[2].led == B[2].hold) {
		B[2].led = !B[2].led;
		out->Send(slGlobal.cc, B[2].cc, B[2].led, 0);
	}
	if (B[3].led == B[3].hold) {
		B[3].led = !B[3].led;
		out->Send(slGlobal.cc, B[3].cc, B[3].led, 0);
	}
	// Markers: Delete time signature marker near cursor
	if (B[4].led == B[4].hold) {
		B[4].led = !B[4].led;
		out->Send(slGlobal.cc, B[4].cc, B[4].led, 0);
	}
	// Set edit cursor at time sel start
	if (B[5].led == B[5].hold) {
		B[5].led = !B[5].led;
		out->Send(slGlobal.cc, B[5].cc, B[5].led, 0);
	}
	// Set edit cursor at time sel end
	if (B[6].led == B[6].hold) {
		B[6].led = !B[6].led;
		out->Send(slGlobal.cc, B[6].cc, B[6].led, 0);
	}

	if (B[7].led == B[7].hold) {
		B[7].led = !B[7].led;
		out->Send(slGlobal.cc, B[7].cc, B[7].led, 0);
	}
}
void SLLed::MixerB(midi_Output* out, vector<SLButton> &B) {
	for (int i = 0; i < 8; i++) {
		int tid = slGlobal.trackBankStart + i;
		MediaTrack *tr = CSurf_TrackFromID(tid, false);

		// Toggle RECORD ARM for track in current bank
		if (slGlobal.rsCmdMain == 0 && B[i].led != (bool)GetMediaTrackInfo_Value(tr, "I_RECARM")) {
			B[i].led = (bool)GetMediaTrackInfo_Value(tr, "I_RECARM");
			out->Send(slGlobal.cc, B[i].cc, B[i].led, 0);
		}

		// Show track in Mixer
		else if (slGlobal.rsCmdMain == 2 && B[i].led != (bool)GetMediaTrackInfo_Value(tr, "B_SHOWINMIXER")) {
			B[i].led = !B[i].led;
			out->Send(slGlobal.cc, B[i].cc, B[i].led, 0);
		}

		// Toggle RECORD MONITOR for track in current bank
		else if (slGlobal.rsCmdMain == 4 && B[i].led != (bool)GetMediaTrackInfo_Value(tr, "I_RECMON")) {
			B[i].led = (bool)GetMediaTrackInfo_Value(tr, "I_RECMON");
			out->Send(slGlobal.cc, B[i].cc, B[i].led, 0);
		}

		// Toggle PHASE for track in current bank
		else if (slGlobal.rsCmdMain == 5 && B[i].led != (bool)GetMediaTrackInfo_Value(tr, "B_PHASE")) {
			B[i].led = !B[i].led;
			out->Send(slGlobal.cc, B[i].cc, B[i].led, 0);
		}

		// Cycle AUTOMATION MODE for track in current bank (trim/off, read, touch, write, latch)
		else if (slGlobal.rsCmdMain == 6 && B[i].led != (bool)GetMediaTrackInfo_Value(tr, "I_AUTOMODE")) {
			B[i].led = GetMediaTrackInfo_Value(tr, "I_AUTOMODE");
			out->Send(slGlobal.cc, B[i].cc, B[i].led, 0);
		}

		// Toggle SOLO for track in current bank
		else if (slGlobal.rsCmdMain == 7 && B[i].led != (bool)GetMediaTrackInfo_Value(tr, "I_SOLO")) {
			B[i].led = (bool)GetMediaTrackInfo_Value(tr, "I_SOLO");
			out->Send(slGlobal.cc, B[i].cc, B[i].led, 0);
		}
	}
}

// SL.Row C ---------------------------------------
void SLLed::UserC(midi_Output* out, vector<SLButton> &C) {
	if (!SLGlobal::transportLock) {
		for (int i = 0; i < 8; i++) {
			if (C[i].led) {
				C[i].led = !C[i].led;
				out->Send(slGlobal.cc, C[i].cc, C[i].led, 0);
			}
		}
	}
}
void SLLed::FxC(midi_Output* out, vector<SLButton> &C, Json::Value &trPlugs) {
	if (!SLGlobal::transportLock) {
		for (int i = 0; i < 8; i++) {
			int fxNum = slGlobal.trackFxSel - 1;
			string pid = to_string(i);
			string ctrlType = "ctrlC";
			string ctrlNum = to_string(SLGlobal::rsCmdMain);

			if (trPlugs[to_string(fxNum)][ctrlType][ctrlNum][pid]["fNum"]) {
				if (C[i].led != bool(round(TrackFX_GetParamNormalized(slGlobal.trackSel, fxNum, trPlugs[to_string(fxNum)][ctrlType][ctrlNum][pid].get("fNum", 0).asInt()) * 127))) {
					C[i].led = !C[i].led;
					out->Send(slGlobal.cc, C[i].cc, C[i].led, 0);
				}
			}
			else if (C[i].led) {
				C[i].led = !C[i].led;
				out->Send(slGlobal.cc, C[i].cc, C[i].led, 0);
			}
		}
	}
}
void SLLed::InstC(midi_Output* out, vector<SLButton> &C) {
	if (!SLGlobal::transportLock) {
		if (C[0].led) {
			C[0].led = !C[0].led;
			out->Send(slGlobal.cc, C[0].cc, C[0].led, 0);
		}
		if (C[1].led) {
			C[1].led = !C[1].led;
			out->Send(slGlobal.cc, C[1].cc, C[1].led, 0);
		}
		if (C[2].led) {
			C[2].led = !C[2].led;
			out->Send(slGlobal.cc, C[2].cc, C[2].led, 0);
		}
		if (C[3].led) {
			C[3].led = !C[3].led;
			out->Send(slGlobal.cc, C[3].cc, C[3].led, 0);
		}
		if (C[4].led) {
			C[4].led = !C[4].led;
			out->Send(slGlobal.cc, C[4].cc, C[4].led, 0);
		}
		if (C[5].led) {
			C[5].led = !C[5].led;
			out->Send(slGlobal.cc, C[5].cc, C[5].led, 0);
		}
		// SWS: Goto/select prev marker/region
		if (C[6].led == C[6].hold) {
			C[6].led = !C[6].led;
			out->Send(slGlobal.cc, C[6].cc, C[6].led, 0);
		}
		// SWS: Goto/select next marker/region
		if (C[7].led == C[7].hold) {
			C[7].led = !C[7].led;
			out->Send(slGlobal.cc, C[7].cc, C[7].led, 0);
		}
	}
}
void SLLed::MixerC(midi_Output* out, vector<SLButton> &C) {
	if (!slGlobal.transportLock) {
		for (int i = 0; i < 8; i++) {

			int tid = slGlobal.trackBankStart + i;
			MediaTrack *tr = CSurf_TrackFromID(tid, false);

			if (slGlobal.rsCmdMain == 0) {
				if (C[i].led != (bool)GetMediaTrackInfo_Value(tr, "B_MUTE")) {
					C[i].led = !C[i].led;
					out->Send(slGlobal.cc, C[i].cc, C[i].led, 0);
				}
			}
			//Indicate markers 1-8 presence
			if (slGlobal.rsCmdMain == 6) {
				int num_markersOut;
				int num_regionsOut;
				CountProjectMarkers(0, &num_markersOut, &num_regionsOut);
				if (i < num_markersOut) {
					if (!C[i].led) {
						C[i].led = true;
						out->Send(slGlobal.cc, C[i].cc, C[i].led, 0);
					}
				}
				else if (C[i].led) {
					C[i].led = false;
					out->Send(slGlobal.cc, C[i].cc, C[i].led, 0);
				}
			}
			// Indicate regions 1-8 presence
			if (slGlobal.rsCmdMain == 7) {
				int num_markersOut;
				int num_regionsOut;
				CountProjectMarkers(0, &num_markersOut, &num_regionsOut);
				if (i < num_regionsOut) {
					if (!C[i].led) {
						C[i].led = true;
						out->Send(slGlobal.cc, C[i].cc, C[i].led, 0);
					}
				}
				else if (C[i].led) {
					C[i].led = false;
					out->Send(slGlobal.cc, C[i].cc, C[i].led, 0);
				}
			}
		}
	}
}

// SL.Row D ---------------------------------------
void SLLed::UserD(midi_Output* out, vector<SLButton> &D) {
	if (!SLGlobal::transportLock) {
		for (int i = 0; i < 8; i++) {
			if (D[i].led) {
				D[i].led = !D[i].led;
				out->Send(slGlobal.cc, D[i].cc, D[i].led, 0);
			}
		}
	}
}
void SLLed::FxD(midi_Output* out, vector<SLButton> &D, Json::Value &trPlugs) {
	if (!SLGlobal::transportLock) {
		for (int i = 0; i < 8; i++) {
			int fxNum = slGlobal.trackFxSel - 1;
			string pid = to_string(i);
			string ctrlType = "ctrlD";
			string ctrlNum = to_string(SLGlobal::rsCmdMain);

			if (trPlugs[to_string(fxNum)][ctrlType][ctrlNum][pid]["fNum"]) {
				if (D[i].led != bool(round(TrackFX_GetParamNormalized(slGlobal.trackSel, fxNum, trPlugs[to_string(fxNum)][ctrlType][ctrlNum][pid].get("fNum", 0).asInt()) * 127))) {
					D[i].led = !D[i].led;
					out->Send(slGlobal.cc, D[i].cc, D[i].led, 0);
				}
			}
			else if (D[i].led) {
				D[i].led = !D[i].led;
				out->Send(slGlobal.cc, D[i].cc, D[i].led, 0);
			}
		}
	}
}
void SLLed::InstD(midi_Output* out, vector<SLButton> &D) {
	if (!SLGlobal::transportLock) {
		if (D[0].led) {
			D[0].led = !D[0].led;
			out->Send(slGlobal.cc, D[0].cc, D[0].led, 0);
		}
		if (D[1].led) {
			D[1].led = !D[1].led;
			out->Send(slGlobal.cc, D[1].cc, D[1].led, 0);
		}
		if (D[2].led) {
			D[2].led = !D[2].led;
			out->Send(slGlobal.cc, D[2].cc, D[2].led, 0);
		}
		if (D[3].led) {
			D[3].led = !D[3].led;
			out->Send(slGlobal.cc, D[3].cc, D[3].led, 0);
		}
		
		if (D[4].led) {
			D[4].led = !D[4].led;
			out->Send(slGlobal.cc, D[4].cc, D[4].led, 0);
		}
		
		if (D[5].led) {
			D[5].led = !D[5].led;
			out->Send(slGlobal.cc, D[5].cc, D[5].led, 0);
		}
		// SWS : Select prev region
		if (D[6].led == D[6].hold) {
			D[6].led = !D[6].led;
			out->Send(slGlobal.cc, D[6].cc, D[6].led, 0);
		}
		// SWS : Select next region
		if (D[7].led == D[7].hold) {
			D[7].led = !D[7].led;
			out->Send(slGlobal.cc, D[7].cc, D[7].led, 0);
		}
	}
}
void SLLed::MixerD(midi_Output* out, vector<SLButton> &D) {
	if (!slGlobal.transportLock) {
		for (int i = 0; i < 8; i++) {

			int tid = slGlobal.trackBankStart + i;
			MediaTrack *tr = CSurf_TrackFromID(tid, false);

			if (slGlobal.rsCmdMain == 0) {
				if (D[i].led != (bool)GetMediaTrackInfo_Value(tr, "I_SOLO")) {
					D[i].led = (bool)GetMediaTrackInfo_Value(tr, "I_SOLO");
					out->Send(slGlobal.cc, D[i].cc, D[i].led, 0);
				}
			}

			if (slGlobal.rsCmdMain == 4) {
				if (D[i].led != (bool)GetMediaTrackInfo_Value(tr, "I_SOLO")) {
					D[i].led = (bool)GetMediaTrackInfo_Value(tr, "I_SOLO");
					out->Send(slGlobal.cc, D[i].cc, D[i].led, 0);
				}
			}

			// Indicate markers 9-16 presence
			if (slGlobal.rsCmdMain == 6) {
				int num_markersOut;
				int num_regionsOut;
				CountProjectMarkers(0, &num_markersOut, &num_regionsOut);
				if (i + 8 < num_markersOut) {
					if (D[i].led != true) {
						D[i].led = true;
						out->Send(slGlobal.cc, D[i].cc, D[i].led, 0);
					}
				}
				else if (D[i].led == true) {
					D[i].led = false;
					out->Send(slGlobal.cc, D[i].cc, D[i].led, 0);
				}
			}

			// Indicate regions 9-16 presence
			if (slGlobal.rsCmdMain == 7) {
				int num_markersOut;
				int num_regionsOut;
				CountProjectMarkers(0, &num_markersOut, &num_regionsOut);
				if (i + 8 < num_regionsOut) {
					if (D[i].led != true) {
						D[i].led = true;
						out->Send(slGlobal.cc, D[i].cc, D[i].led, 0);
					}
				}
				else if (D[i].led == true) {
					D[i].led = false;
					out->Send(slGlobal.cc, D[i].cc, D[i].led, 0);
				}
			}
		}
	}
}