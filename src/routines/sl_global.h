#pragma once

#ifndef SLGLOBAL_H
#define SLGLOBAL_H

#include "../csurf.h"
#include <string>

using namespace std;

class SLGlobal {
public:
	static const int cc;
	static const int note;
	
	static bool automapStatus;
	static bool transportLock;

	// Tracks bank variables
	static int trackMax;				// Overall tracks amount in the project
	static int trackBankStart;			// Tracks bank
	static int trackBankMax;			// Tracks bank
	static int trackBankSel;			// Currently selected bank

	//Tracks
	static MediaTrack *track;
	static MediaTrack *trackSel;
	static MediaTrack *trackSelPrev;	// First track in previous track selection

	// Track FX
	static int trackFxMax;			// overall track FX amount on the track
	static int trackFxSel;			// currently selected FX
	static int trackFxSelPrev;		// FX on previous track

	// Track FX parameter bank
	static int trackFxPageMax;
	static string trackFxPageSel;

	
	static int rsCmdMain;			// Active
	static int rsCmdE;				// Active
	static int rsCmdP;				// Active
	static int rsCmdF;				// Active
	static int rsCmdSd;				// Active
	static reaper_plugin_info_t *reaper_plugin_info_copy;

};

#endif // !SLGLOBAL_H
