#include "sl_cmd.h"

using namespace Json;

// ---------- FX MODE FUNCTIONS ---------------------
void SLCmd::FxButton(string ctrlType, int fxid, int pid, Value &trPlugs) {
	string fxNum = to_string(fxid);
	string page = slGlobal.trackFxPageSel;
	string ctrlShift = to_string(SLGlobal::rsCmdMain);
	string fxP = to_string(pid);

	if (trPlugs[fxNum]["Page"][page][ctrlType][ctrlShift][fxP]["fNum"]) {

		//char str[512];
		//sprintf(str, "exist\n");
		//ShowConsoleMsg(str);

		int fNum = trPlugs[fxNum]["Page"][page][ctrlType][ctrlShift][fxP].get("fNum", 0).asInt();

		double minVal = trPlugs[fxNum]["Page"][page][ctrlType][ctrlShift][fxP].get("pValMin", 0.0).asDouble();
		double maxVal = trPlugs[fxNum]["Page"][page][ctrlType][ctrlShift][fxP].get("pValMax", 1.0).asDouble();

		bool toggle = trPlugs[fxNum]["Page"][page][ctrlType][ctrlShift][fxP].get("pToggle", false).asBool();
		int steps = trPlugs[fxNum]["Page"][page][ctrlType][ctrlShift][fxP].get("pStepNum", 0).asInt();
		bool rev = trPlugs[fxNum]["Page"][page][ctrlType][ctrlShift][fxP].get("pReverse", false).asBool();

		double stepSize = (maxVal - minVal) / steps;

		double val = TrackFX_GetParamNormalized(slGlobal.trackSel, fxid, fNum);

		if (toggle || !steps) {
			TrackFX_SetParamNormalized(slGlobal.trackSel, fxid, fNum, !val);
		}
		else if (steps) {
			double newVal;
			(rev) ? newVal = val - stepSize : newVal = val + stepSize;

			if (val != maxVal) {
				if (newVal >= maxVal) newVal = maxVal;
			}
			else {
				if (newVal >= maxVal) newVal = minVal;
			}
			TrackFX_SetParamNormalized(slGlobal.trackSel, fxid, fNum, newVal);

			//char str[512];
			//sprintf(str, "v-%f\t n-%f\t s-%f\n", val, newVal, stepSize);
			//ShowConsoleMsg(str);
		}
	}
}

// ---------- TRANSPORT -----------------------------
void SLCmd::Transport(vector<SLButton> &Tr, vector<SLButton> &C, bool mLed) {
	if (SLGlobal::rsCmdMain == 0) {
		if (Tr[0].click) RunCommand(40042); // Transport: Go to start of project
		if (Tr[0].clickHold) RunCommand(40042); // Transport: Go to start of project
		if (Tr[0].dblClick) RunCommand("_SWS_SELPREVREG");	// SWS: Select previous region
		if (Tr[0].dblClickHold) RunCommand(40042); // Transport: Go to start of project

		if (Tr[1].click) RunCommand(40043); // Transport: Go to end of project
		if (Tr[1].clickHold) RunCommand(40043); // Transport: Go to end of project
		if (Tr[1].dblClick) RunCommand("_SWS_SELNEXTREG");	// SWS: Select next region
		if (Tr[1].dblClickHold) RunCommand(40043); // Transport: Go to end of project

		// Transport: Stop
		if (Tr[2].on) {
			Tr[2].on = false;
			RunCommand(1016); // Transport: Stop
		}
		// Transport: Play
		if (Tr[3].on) {
			Tr[3].on = false;
			RunCommand(40073); // Transport: Play/pause
		}
		// Transport: Repeat
		if (Tr[4].on) {
			Tr[4].on = false;
			RunCommand(1068); // Transport: Toggle repeat
		}
		// Transport: Record
		if (Tr[5].on) {
			Tr[5].on = false;
			RunCommand(1013); // Transport: Record
		}
	}
	if (SLGlobal::rsCmdMain == 1) {
		if (Tr[0].click) RunCommand(40104);		// View : Move cursor left one pixel
		if (Tr[1].click) RunCommand(40105);		// View : Move cursor right one pixel
		if (Tr[2].click) RunCommand(41314);		// Track: Toggle lock / unlock track controls
		if (Tr[3].on) RunCommand("_S&M_DUMMY_TGL1");	// Dummy action
		if (Tr[4].on) RunCommand("_S&M_DUMMY_TGL1");	// Dummy action
		if (Tr[5].on) RunCommand("_S&M_DUMMY_TGL1");	// Dummy action
	}
	if (SLGlobal::rsCmdMain == 2) {
		if (Tr[0].click) RunCommand(41666);		// View : Move cursor left 8 pixels
		if (Tr[1].click) RunCommand(41667);		// View : Move cursor right 8 pixels
		if (Tr[2].on) RunCommand("_S&M_DUMMY_TGL1");	// Dummy action
		if (Tr[3].on) RunCommand("_S&M_DUMMY_TGL1");	// Dummy action
		if (Tr[4].on) RunCommand("_S&M_DUMMY_TGL1");	// Dummy action
		if (Tr[5].on) RunCommand("_S&M_DUMMY_TGL1");	// Dummy action
	}
	if (SLGlobal::rsCmdMain == 3) {
		if (Tr[0].click) RunCommand(40646);		// View : Move cursor left to grid division
		if (Tr[1].click) RunCommand(40647);		// View : Move cursor right to grid division
		if (Tr[2].on) RunCommand("_S&M_DUMMY_TGL1");	// Dummy action
		if (Tr[3].on) RunCommand("_S&M_DUMMY_TGL1");	// Dummy action
		if (Tr[4].on) RunCommand("_S&M_DUMMY_TGL1");	// Dummy action
		if (Tr[5].on) RunCommand("_S&M_DUMMY_TGL1");	// Dummy action
	}
	if (SLGlobal::rsCmdMain == 4) {
		if (Tr[0].click) RunCommand(40084);		// Transport: Rewind a little bit
		if (Tr[1].click) RunCommand(40085);		// Transport : Fast forward a little bit
		if (Tr[2].on) RunCommand(40668);		// Transport: Stop(DELETE all recorded media)
		if (Tr[3].on) RunCommand("_S&M_DUMMY_TGL1");	// Dummy action
		if (Tr[4].on) RunCommand("_S&M_DUMMY_TGL1");	// Dummy action
		if (Tr[5].on) RunCommand("_S&M_DUMMY_TGL1");	// Dummy action
	}
	if (SLGlobal::rsCmdMain == 7) {
		if (Tr[0].on) RunCommand("_S&M_DUMMY_TGL1");	// Dummy action
		if (Tr[1].on) RunCommand("_S&M_DUMMY_TGL1");	// Dummy action
		if (Tr[2].on) RunCommand("_S&M_DUMMY_TGL1");	// Dummy action
		if (Tr[3].on) RunCommand("_S&M_DUMMY_TGL1");	// Dummy action
		if (Tr[4].on) RunCommand("_S&M_DUMMY_TGL1");	// Dummy action
		if (Tr[5].on) RunCommand("_S&M_DUMMY_TGL1");	// Dummy action
	}

	if (SLGlobal::transportLock) {
		if (SLGlobal::rsCmdMain == 1) {
			// Track : Unmute all tracks
			if (C[0].on) {
				C[0].on = false;
				RunCommand(40339);
			}
			// Track: Unsolo all tracks
			if (C[1].on) {
				C[1].on = false;
				RunCommand(40340);
			}
			// Track: Unarm all tracks for recording
			if (C[2].on) {
				C[2].on = false;
				RunCommand(40491);
			}
			// Track : Unfreeze tracks(restore previously saved items and FX)
			if (C[3].on) {
				C[3].on = false;
				RunCommand(41644);
			}

			if (C[4].on) RunCommand("_S&M_DUMMY_TGL1");	// Dummy action
			if (C[5].on) RunCommand("_S&M_DUMMY_TGL1");	// Dummy action
			if (C[6].on) RunCommand("_S&M_DUMMY_TGL1");	// Dummy action
			if (C[7].on) RunCommand("_S&M_DUMMY_TGL1");	// Dummy action
		}
		else if (SLGlobal::rsCmdMain == 6) {
			if (C[0].on) RunCommand("_S&M_DUMMY_TGL1");	// Dummy action
			if (C[1].on) RunCommand("_S&M_DUMMY_TGL1");	// Dummy action
			if (C[2].on) RunCommand("_S&M_DUMMY_TGL1");	// Dummy action
			if (C[3].on) RunCommand("_S&M_DUMMY_TGL1");	// Dummy action

			if (C[4].on) RunCommand("_S&M_DUMMY_TGL1");	// Dummy action
			if (C[5].on) RunCommand("_S&M_DUMMY_TGL1");	// Dummy action
			if (C[6].on) RunCommand("_S&M_DUMMY_TGL1");	// Dummy action
			if (C[7].on) {
				C[7].on = false;
				RunCommand(1134);		// Transport: Tap tempo
			}
		}
		else if (SLGlobal::rsCmdMain == 7) {
			if (C[0].on) {
				C[0].on = false;
				RunCommand(6);			// Track: Toggle mute for selected tracks
			}
			if (C[1].on) {
				C[1].on = false;
				RunCommand(7);			// Track: Toggle solo for selected tracks
			}
			if (C[2].on) {
				C[2].on = false;
				RunCommand(9);			// Track: Toggle record arm for selected tracks
			}
			if (C[3].on) {
				C[3].on = false;
				RunCommand(8);			// Track: Toggle FX bypass for selected tracks
			}
			if (C[4].on) {
				C[4].on = false;
				RunCommand("_S&M_DUMMY_TGL1");	// Dummy action
			}
			if (C[5].on) {
				C[5].on = false;
				RunCommand("_S&M_DUMMY_TGL1");	// Dummy action
			}
			if (C[6].on) {
				C[6].on = false;
				RunCommand("_SWS_SELPREVMORR"); // SWS: Goto/select prev marker/region
			}
			if (C[7].on) {
				C[7].on = false;
				RunCommand("_SWS_SELNEXTMORR"); // SWS: Goto/select next marker/region
			}
		}
		else {
			if (C[0].click) Undo_DoUndo2(0);			// Transport: undo
			if (C[0].clickHold) RunCommand(40026);		// File: Save project
			if (C[0].dblClick) Undo_DoUndo2(0);			// Transport: undo
			if (C[0].dblClickHold) RunCommand(40022);	// File: Save project as...

			if (C[1].click) Undo_DoRedo2(0);			// Transport: redo
			if (C[1].click) Undo_DoRedo2(0);			// Transport: redo
			if (C[1].dblClick) Undo_DoRedo2(0);							// Transport: redo
																		//if (C[1].dblClickHold) Undo_DoRedo2(0);							// Transport: redo

			if (C[2].on) RunCommand(40668);			// Transport: Stop (DELETE all recorded media)

			if (C[3].click) RunCommand(1042);			// Track : Cycle folder collapsed state
			if (C[3].clickHold) RunCommand(1041);			// Track: Cycle track folder state
																	//if (C[3].dblClick) RunCommand(40001, 0);			// Track: Insert new track
																	//if (C[3].dblClickHold) RunCommand(40701, 0);		// Insert virtual instrument on new track...

			if (C[4].click) RunCommand(40454);			// Screenset: Load window set #01
			if (C[4].clickHold) RunCommand(40474);		// Screenset: Save window set #01

			if (C[5].click) RunCommand(40455);			// Screenset: Load window set #02
			if (C[5].clickHold) RunCommand(40475);		// Screenset: Save window set #02

			if (C[6].click) RunCommand(40456);			// Screenset: Load window set #03
			if (C[6].clickHold) RunCommand(40476);		// Screenset: Save window set #03

			if (C[7].click) RunCommand(40364);				// Metronome toggle
			if (C[7].clickHold) mLed = !mLed;		// Led metronome toggle
		}
	}
}

// SL.Speed Dial ------------------------------------
void SLCmd::UserSD(SLSpeedDial &Sd) {
	if (slGlobal.rsCmdSd == 1) {
		// View : Scroll view up/down
		if (!Sd.hold) {
			if (Sd.left) RunCommand(40138);
			if (Sd.right) RunCommand(40139);
		}
		// View : Zoom vertically
		if (Sd.hold) {
			if (Sd.left) RunCommand(40112);
			if (Sd.right) RunCommand(40111);
		}
	}
	else if (slGlobal.rsCmdSd == 2) {
		// View : Scroll view left/right
		if (!Sd.hold) {
			if (Sd.left) CSurf_OnScroll(-Sd.left, 0);
			if (Sd.right) CSurf_OnScroll(Sd.right, 0);
		}
		// View: Zoom horizontally
		if (Sd.hold) {
			if (Sd.left) RunCommand(1011);
			if (Sd.right) RunCommand(1012);
		}
	}
	else if (slGlobal.rsCmdSd == 3) {
		// Exclusive select track
		if (!Sd.hold) {
			int tr = CSurf_TrackToID(GetSelectedTrack(0, 0), false);

			if (Sd.left) {
				tr -= 1;
				SetOnlyTrackSelected(CSurf_TrackFromID(__minVal(__maxVal(tr, 1), slGlobal.trackMax), false));
			}
			if (Sd.right) {
				tr += 1;
				SetOnlyTrackSelected(CSurf_TrackFromID(__minVal(__maxVal(tr, 1), slGlobal.trackMax), false));
			}
		}
		// Track: Go to next/prev track (leaving other tracks selected)
		if (Sd.hold) {
			if (Sd.left) RunCommand(40288);
			if (Sd.right) RunCommand(40287);
		}
	}
	else if (slGlobal.rsCmdSd == 5) {
		// Master track volume
		if (!Sd.hold && (Sd.left || Sd.right)) {
			MediaTrack *tr = CSurf_TrackFromID(0, false);

			if (Sd.left) {
				int amt = __maxVal(0, volToChar(GetMediaTrackInfo_Value(tr, "D_VOL")) - Sd.left);
				SetMediaTrackInfo_Value(tr, "D_VOL", charToVol(amt));
			}

			if (Sd.right) {
				int amt = __minVal(127, volToChar(GetMediaTrackInfo_Value(tr, "D_VOL")) + Sd.right);
				SetMediaTrackInfo_Value(tr, "D_VOL", charToVol(amt));
			}
		}

		// EDIT THESE AND ADD MONO MODE FOR MASTER
		if (Sd.click) RunCommand(41884);		// Monitoring FX: Toggle bypass
		if (Sd.clickHold) RunCommand(41884);		// Monitoring FX: Toggle bypass
		if (Sd.dblClick) RunCommand(41882);	// View: Show monitoring FX chain
		if (Sd.dblClickHold) RunCommand(41882);	// View: Show monitoring FX chain
	}
	else if (slGlobal.rsCmdSd == 6) {
		// SWS: Goto/select prev/next marker/region
		if (!Sd.hold) {
			if (Sd.left) RunCommand("_SWS_SELPREVMORR");
			if (Sd.right) RunCommand("_SWS_SELNEXTMORR");
		}
		// SWS : Select prev/next region
		if (Sd.hold) {
			if (Sd.left) RunCommand("_SWS_SELPREVREG");
			if (Sd.right) RunCommand("_SWS_SELNEXTREG");
		}
	}
	else if (slGlobal.rsCmdSd == 7) {
		// Move cursor and time select
		if (!Sd.hold) {
			//if (Sd.left) MoveCursorAndSel(-1);
			//if (Sd.right) MoveCursorAndSel(1);
		}

	}
	// Mouse emulation for speed dial
	else {
		// Left click
		if (Sd.on) {
			Sd.on = false;
			mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
		}
		if (Sd.off) {
			Sd.off = false;
			mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
		}
		// Mouse wheel (with acceleration)
		if (Sd.left) mouse_event(MOUSEEVENTF_WHEEL, 0, 0, -120 * Sd.left, 0);
		if (Sd.right) mouse_event(MOUSEEVENTF_WHEEL, 0, 0, 120 * Sd.right, 0);
	}
}
void SLCmd::FxSD(SLSpeedDial &Sd) {
	if (slGlobal.rsCmdSd == 1) {
		// View : Scroll view up/down
		if (!Sd.hold) {
			if (Sd.left) RunCommand(40138);
			if (Sd.right) RunCommand(40139);
		}
		// View : Zoom vertically
		if (Sd.hold) {
			if (Sd.left) RunCommand(40112);
			if (Sd.right) RunCommand(40111);
		}
	}
	else if (slGlobal.rsCmdSd == 2) {
		// View : Scroll view left/right
		if (!Sd.hold) {
			if (Sd.left) CSurf_OnScroll(-Sd.left, 0);
			if (Sd.right) CSurf_OnScroll(Sd.right, 0);
		}
		// View: Zoom horizontally
		if (Sd.hold) {
			if (Sd.left) RunCommand(1011);
			if (Sd.right) RunCommand(1012);
		}
	}
	else if (slGlobal.rsCmdSd == 3) {
		// Exclusive select track
		if (!Sd.hold) {
			int tr = CSurf_TrackToID(GetSelectedTrack(0, 0), false);

			if (Sd.left) {
				tr -= 1;
				SetOnlyTrackSelected(CSurf_TrackFromID(__minVal(__maxVal(tr, 1), slGlobal.trackMax), false));
			}
			if (Sd.right) {
				tr += 1;
				SetOnlyTrackSelected(CSurf_TrackFromID(__minVal(__maxVal(tr, 1), slGlobal.trackMax), false));
			}
		}
		// Track: Go to next/prev track (leaving other tracks selected)
		if (Sd.hold) {
			if (Sd.left) RunCommand(40288);
			if (Sd.right) RunCommand(40287);
		}
	}
	else if (slGlobal.rsCmdSd == 5) {
		// Master track volume
		if (!Sd.hold && (Sd.left || Sd.right)) {
			MediaTrack *tr = CSurf_TrackFromID(0, false);

			if (Sd.left) {
				int amt = __maxVal(0, volToChar(GetMediaTrackInfo_Value(tr, "D_VOL")) - Sd.left);
				SetMediaTrackInfo_Value(tr, "D_VOL", charToVol(amt));
			}

			if (Sd.right) {
				int amt = __minVal(127, volToChar(GetMediaTrackInfo_Value(tr, "D_VOL")) + Sd.right);
				SetMediaTrackInfo_Value(tr, "D_VOL", charToVol(amt));
			}
		}

		// EDIT THESE AND ADD MONO MODE FOR MASTER
		if (Sd.click) RunCommand(41884);		// Monitoring FX: Toggle bypass
		if (Sd.clickHold) RunCommand(41884);		// Monitoring FX: Toggle bypass
		if (Sd.dblClick) RunCommand(41882);	// View: Show monitoring FX chain
		if (Sd.dblClickHold) RunCommand(41882);	// View: Show monitoring FX chain
	}
	else if (slGlobal.rsCmdSd == 6) {
		// SWS: Goto/select prev/next marker/region
		if (!Sd.hold) {
			if (Sd.left) RunCommand("_SWS_SELPREVMORR");
			if (Sd.right) RunCommand("_SWS_SELNEXTMORR");
		}
		// SWS : Select prev/next region
		if (Sd.hold) {
			if (Sd.left) RunCommand("_SWS_SELPREVREG");
			if (Sd.right) RunCommand("_SWS_SELNEXTREG");
		}
	}
	else if (slGlobal.rsCmdSd == 7) {
		// Move cursor and time select
		if (!Sd.hold) {
			//if (Sd.left) MoveCursorAndSel(-1);
			//if (Sd.right) MoveCursorAndSel(1);
		}

	}
	else {
		// Move cursor by grid
		if (!Sd.hold) {
			if (Sd.left) ApplyNudge(0, 2, 6, 2, Sd.left, true, 0);
			if (Sd.right) ApplyNudge(0, 2, 6, 2, Sd.right, false, 0);
		}
		// SWS: Move cursor and time selection by grid
		if (Sd.hold) {
			if (Sd.left) RunCommand("_SWS_MOVECURSELLEFT");
			if (Sd.right) RunCommand("_SWS_MOVECURSELRIGHT");
		}
	}
}
void SLCmd::InstSD(SLSpeedDial &Sd) {
	if (slGlobal.rsCmdSd == 1) {
		// View : Scroll view up/down
		if (!Sd.hold) {
			if (Sd.left) RunCommand(40138);
			if (Sd.right) RunCommand(40139);
		}
		// View : Zoom vertically
		if (Sd.hold) {
			if (Sd.left) RunCommand(40112);
			if (Sd.right) RunCommand(40111);
		}
	}
	else if (slGlobal.rsCmdSd == 2) {
		// View : Scroll view left/right
		if (!Sd.hold) {
			if (Sd.left) CSurf_OnScroll(-Sd.left, 0);
			if (Sd.right) CSurf_OnScroll(Sd.right, 0);
		}
		// View: Zoom horizontally
		if (Sd.hold) {
			if (Sd.left) RunCommand(1011);
			if (Sd.right) RunCommand(1012);
		}
	}
	else if (slGlobal.rsCmdSd == 3) {
		// Exclusive select track
		if (!Sd.hold) {
			int tr = CSurf_TrackToID(GetSelectedTrack(0, 0), false);

			if (Sd.left) {
				tr -= 1;
				SetOnlyTrackSelected(CSurf_TrackFromID(__minVal(__maxVal(tr, 1), slGlobal.trackMax), false));
			}
			if (Sd.right) {
				tr += 1;
				SetOnlyTrackSelected(CSurf_TrackFromID(__minVal(__maxVal(tr, 1), slGlobal.trackMax), false));
			}
		}
		// Track: Go to next/prev track (leaving other tracks selected)
		if (Sd.hold) {
			if (Sd.left) RunCommand(40288);
			if (Sd.right) RunCommand(40287);
		}
	}
	else if (slGlobal.rsCmdSd == 5) {
		// Master track volume
		if (!Sd.hold && (Sd.left || Sd.right)) {
			MediaTrack *tr = CSurf_TrackFromID(0, false);

			if (Sd.left) {
				int amt = __maxVal(0, volToChar(GetMediaTrackInfo_Value(tr, "D_VOL")) - Sd.left);
				SetMediaTrackInfo_Value(tr, "D_VOL", charToVol(amt));
			}

			if (Sd.right) {
				int amt = __minVal(127, volToChar(GetMediaTrackInfo_Value(tr, "D_VOL")) + Sd.right);
				SetMediaTrackInfo_Value(tr, "D_VOL", charToVol(amt));
			}
		}

		// EDIT THESE AND ADD MONO MODE FOR MASTER
		if (Sd.click) RunCommand(41884);		// Monitoring FX: Toggle bypass
		if (Sd.clickHold) RunCommand(41884);		// Monitoring FX: Toggle bypass
		if (Sd.dblClick) RunCommand(41882);	// View: Show monitoring FX chain
		if (Sd.dblClickHold) RunCommand(41882);	// View: Show monitoring FX chain
	}
	else if (slGlobal.rsCmdSd == 6) {
		// SWS: Goto/select prev/next marker/region
		if (!Sd.hold) {
			if (Sd.left) RunCommand("_SWS_SELPREVMORR");
			if (Sd.right) RunCommand("_SWS_SELNEXTMORR");
		}
		// SWS : Select prev/next region
		if (Sd.hold) {
			if (Sd.left) RunCommand("_SWS_SELPREVREG");
			if (Sd.right) RunCommand("_SWS_SELNEXTREG");
		}
	}
	else if (slGlobal.rsCmdSd == 7) {
		// Move cursor and time select
		if (!Sd.hold) {
			//if (Sd.left) MoveCursorAndSel(-1);
			//if (Sd.right) MoveCursorAndSel(1);
		}

	}
	else {
		// Move cursor by grid
		if (!Sd.hold) {
			if (Sd.left) ApplyNudge(0, 2, 6, 2, Sd.left, true, 0);
			if (Sd.right) ApplyNudge(0, 2, 6, 2, Sd.right, false, 0);
		}
		// SWS: Move cursor and time selection by grid
		if (Sd.hold) {
			if (Sd.left) RunCommand("_SWS_MOVECURSELLEFT");
			if (Sd.right) RunCommand("_SWS_MOVECURSELRIGHT");
		}
	}
}
void SLCmd::MixerSD(SLSpeedDial &Sd) {
	if (slGlobal.rsCmdSd == 1) {
		// View : Scroll view up/down
		if (!Sd.hold) {
			if (Sd.left) RunCommand(40138);
			if (Sd.right) RunCommand(40139);
		}
		// View : Zoom vertically
		if (Sd.hold) {
			if (Sd.left) RunCommand(40112);
			if (Sd.right) RunCommand(40111);
		}
	}
	else if (slGlobal.rsCmdSd == 2) {
		// View : Scroll view left/right
		if (!Sd.hold) {
			if (Sd.left) CSurf_OnScroll(-Sd.left, 0);
			if (Sd.right) CSurf_OnScroll(Sd.right, 0);
		}
		// View: Zoom horizontally
		if (Sd.hold) {
			if (Sd.left) RunCommand(1011);
			if (Sd.right) RunCommand(1012);
		}
	}
	else if (slGlobal.rsCmdSd == 3) {
		// Exclusive select track
		if (!Sd.hold) {
			int tr = CSurf_TrackToID(GetSelectedTrack(0, 0), false);

			if (Sd.left) {
				tr -= 1;
				SetOnlyTrackSelected(CSurf_TrackFromID(__minVal(__maxVal(tr, 1), slGlobal.trackMax), false));
			}
			if (Sd.right) {
				tr += 1;
				SetOnlyTrackSelected(CSurf_TrackFromID(__minVal(__maxVal(tr, 1), slGlobal.trackMax), false));
			}
		}
		// Track: Go to next/prev track (leaving other tracks selected)
		if (Sd.hold) {
			if (Sd.left) RunCommand(40288);
			if (Sd.right) RunCommand(40287);
		}
	}
	else if (slGlobal.rsCmdSd == 5) {
		// Master track volume
		if (!Sd.hold && (Sd.left || Sd.right)) {
			MediaTrack *tr = CSurf_TrackFromID(0, false);

			if (Sd.left) {
				int amt = __maxVal(0, volToChar(GetMediaTrackInfo_Value(tr, "D_VOL")) - Sd.left);
				SetMediaTrackInfo_Value(tr, "D_VOL", charToVol(amt));
			}

			if (Sd.right) {
				int amt = __minVal(127, volToChar(GetMediaTrackInfo_Value(tr, "D_VOL")) + Sd.right);
				SetMediaTrackInfo_Value(tr, "D_VOL", charToVol(amt));
			}
		}

		// EDIT THESE AND ADD MONO MODE FOR MASTER
		if (Sd.click) RunCommand(41884);		// Monitoring FX: Toggle bypass
		if (Sd.clickHold) RunCommand(41884);		// Monitoring FX: Toggle bypass
		if (Sd.dblClick) RunCommand(41882);	// View: Show monitoring FX chain
		if (Sd.dblClickHold) RunCommand(41882);	// View: Show monitoring FX chain
	}
	else if (slGlobal.rsCmdSd == 6) {
		// Adjust Solo in Front volume
		if (Sd.click) RunCommand(40745);		// Options: Solo in front
		if (Sd.left || Sd.right) {
			int sz;
			int* pSoloInFront = (int*)get_config_var("solodimdb10", &sz);
			if (Sd.left) *pSoloInFront + Sd.left * 10 < -600 ? *pSoloInFront = -600 : *pSoloInFront = *pSoloInFront - Sd.left * 10;
			if (Sd.right) *pSoloInFront + Sd.right * 10 > -1 ? *pSoloInFront = -1 : *pSoloInFront = *pSoloInFront + Sd.right * 10;
		}


		//// SWS: Goto/select prev/next marker/region
		//if (!Sd.hold) {
		//	if (Sd.left) RunCommand("_SWS_SELPREVMORR");
		//	if (Sd.right) RunCommand("_SWS_SELNEXTMORR");
		//}
		//// SWS : Select prev/next region
		//if (Sd.hold) {
		//	if (Sd.left) RunCommand("_SWS_SELPREVREG");
		//	if (Sd.right) RunCommand("_SWS_SELNEXTREG");
		//}
	}
	else if (slGlobal.rsCmdSd == 7) {
		// Reset global playrate (switch between halftime and normal speed)
		if (Sd.click) {
			double playRate = Master_GetPlayRate(0);
			if (playRate != 1) CSurf_OnPlayRateChange(1);
			else CSurf_OnPlayRateChange(0.5);
		}
		// Adjust global playrate
		if (Sd.left || Sd.right) {
			double playRate = Master_GetPlayRate(0);
			if (Sd.left) CSurf_OnPlayRateChange(playRate - double(Sd.left) / 100);
			if (Sd.right) CSurf_OnPlayRateChange(playRate + double(Sd.right) / 100);
		}
	}
	else {
		// Move cursor by grid
		if (!Sd.hold) {
			if (Sd.left) ApplyNudge(0, 2, 6, 2, Sd.left, true, 0);
			if (Sd.right) ApplyNudge(0, 2, 6, 2, Sd.right, false, 0);
		}
		// SWS: Move cursor and time selection by grid
		if (Sd.hold) {
			if (Sd.left) RunCommand("_SWS_MOVECURSELLEFT");
			if (Sd.right) RunCommand("_SWS_MOVECURSELRIGHT");
		}
	}
}

// SL.P1 (Page up and down) -------------------------
void SLCmd::UserPg1(vector<SLButton> &Pg) {}
void SLCmd::FxPg1(vector<SLButton> &Pg) {
	// Select Next/Prev FX
	if (Pg[0].on) {
		Pg[0].on = false;
		slGlobal.trackFxSel -= 1;			// Prev track - P2 up
		if (slGlobal.trackFxSel < 1) slGlobal.trackFxSel = 1;
		if (slGlobal.rsCmdMain != 7) {
			TrackFX_SetOpen(slGlobal.trackSel, slGlobal.trackFxSel - 1, true);
			RunCommand("_S&M_WNCLS6");
		}
	}
	if (Pg[1].on) {
		Pg[1].on = false;
		slGlobal.trackFxSel += 1;			// Next track - P2 down
		if (slGlobal.trackFxSel > slGlobal.trackFxMax) slGlobal.trackFxSel = slGlobal.trackFxMax;
		if (slGlobal.rsCmdMain != 7) {
			TrackFX_SetOpen(slGlobal.trackSel, slGlobal.trackFxSel - 1, true);
			RunCommand("_S&M_WNCLS6");
		}
	}
}
void SLCmd::InstPg1(vector<SLButton> &Pg) {
	// Prev bank - P2 up
	if (Pg[0].on) {
		Pg[0].on = false;
		slGlobal.trackBankStart -= 1;
		if (slGlobal.trackBankStart < 1) slGlobal.trackBankStart = 1;									// Restrict negative number bank
	}
	// Next bank - P2 down
	if (Pg[1].on) {
		Pg[1].on = false;
		slGlobal.trackBankStart += 1;
		if (slGlobal.trackBankStart > slGlobal.trackMax) slGlobal.trackBankStart = slGlobal.trackMax;					// Restrict non-existing bank
	}
}
void SLCmd::MixerPg1(vector<SLButton> &Pg) {
	// Prev bank - P2 up
	if (Pg[0].on) {
		Pg[0].on = false;
		slGlobal.trackBankStart -= 1;
		if (slGlobal.trackBankStart < 1) slGlobal.trackBankStart = 1;									// Restrict negative number bank
	}
	// Next bank - P2 down
	if (Pg[1].on) {
		Pg[1].on = false;
		slGlobal.trackBankStart += 1;
		if (slGlobal.trackBankStart + 7 > slGlobal.trackMax) slGlobal.trackBankStart = slGlobal.trackMax - 7;					// Restrict non-existing bank
	}
}

// SL.P2 (Page up and down) -------------------------
void SLCmd::UserPg2(vector<SLButton> &Pg) {}
void SLCmd::FxPg2(vector<SLButton> &Pg) {
	if (Pg[2].on) {
		Pg[2].on = false;
		slGlobal.trackFxPageSel = to_string(stoi(slGlobal.trackFxPageSel) - 1);
		if (stoi(slGlobal.trackFxPageSel) < 1) slGlobal.trackFxPageSel = "1";								// Restrict negative number page
	}
	if (Pg[3].on) {
		Pg[3].on = false;
		slGlobal.trackFxPageSel = to_string(stoi(slGlobal.trackFxPageSel) + 1);
		if (stoi(slGlobal.trackFxPageSel) > slGlobal.trackFxPageMax) slGlobal.trackFxPageSel = to_string(slGlobal.trackFxPageMax);	// Restrict non-existing page
	}
}
void SLCmd::InstPg2(vector<SLButton> &Pg) {

	if (Pg[2].on) {
		Pg[2].on = false;
		slGlobal.trackBankStart -= 8;															// Prev bank - P2 up
		if (slGlobal.trackBankStart < 1) slGlobal.trackBankStart = 1;									// Restrict negative number bank
		if (slGlobal.trackBankStart < CSurf_TrackToID(GetMixerScroll(), false)) {
			SetMixerScroll(CSurf_TrackFromID(slGlobal.trackBankStart, false));
		}
	}
	if (Pg[3].on) {
		Pg[3].on = false;
		if (slGlobal.trackBankStart + 7 < slGlobal.trackMax) {
			slGlobal.trackBankStart += 8;															// Next bank - P2 down	
			if (slGlobal.trackBankStart + 7 < CSurf_TrackToID(GetMixerScroll(), false)) {
				SetMixerScroll(CSurf_TrackFromID(slGlobal.trackBankStart, false));
			}
		}
	}
}
void SLCmd::MixerPg2(vector<SLButton> &Pg) {

	if (Pg[2].on) {
		Pg[2].on = false;
		slGlobal.trackBankStart -= 8;															// Prev bank - P2 up
		if (slGlobal.trackBankStart < 1) slGlobal.trackBankStart = 1;									// Restrict negative number bank
		if (slGlobal.trackBankStart < CSurf_TrackToID(GetMixerScroll(), false)) {
			SetMixerScroll(CSurf_TrackFromID(slGlobal.trackBankStart, false));
		}
	}
	if (Pg[3].on) {
		Pg[3].on = false;
		if (slGlobal.trackBankStart + 7 < slGlobal.trackMax) {
			slGlobal.trackBankStart += 8;															// Next bank - P2 down	
			if (slGlobal.trackBankStart + 7 < CSurf_TrackToID(GetMixerScroll(), false)) {
				SetMixerScroll(CSurf_TrackFromID(slGlobal.trackBankStart, false));
			}
		}
	}
}

// SL.Encoders ------------------------------------
void SLCmd::UserE(vector<SLEncoder> &E) {}
void SLCmd::FxE(vector<SLEncoder> &E, Value &trPlugs) {
	for (int i = 0; i < 8; i++) {
		if (E[i].left || E[i].right) {
			if (slGlobal.rsCmdE == 1) {
				if (slGlobal.trackSel) {
					int dryWetParm = TrackFX_GetNumParams(slGlobal.trackSel, i) - 1;
					int curWet = int(round(TrackFX_GetParamNormalized(slGlobal.trackSel, i, dryWetParm) * 127));
					if (E[i].left) {
						int amt = __maxVal(0, curWet - E[i].left);
						TrackFX_SetParamNormalized(slGlobal.trackSel, i, dryWetParm, double(amt) / 127);
					}
					if (E[i].right) {
						int amt = __minVal(127, curWet + E[i].right);
						TrackFX_SetParamNormalized(slGlobal.trackSel, i, dryWetParm, double(amt) / 127);
					}
				}
			}
			else {
				string page = slGlobal.trackFxPageSel;

				string ctrlType = "default";
				string ctrlMode = to_string(slGlobal.rsCmdE);

				int fxid = slGlobal.trackFxSel - 1;
				string fxNum = to_string(fxid);

				string fxP = to_string(i);

				if (trPlugs[fxNum]["Page"][page][ctrlType][ctrlMode][fxP]["fNum"]) {
					double val = TrackFX_GetParamNormalized(slGlobal.trackSel, fxid, trPlugs[fxNum]["Page"][page][ctrlType][ctrlMode][fxP].get("fNum", 0).asInt());
					//double val = TrackFX_GetParamNormalized(slGlobal.trackSel, fxid, pid);
					if (E[i].left) {
						double newval = __maxVal(trPlugs[fxNum]["Page"][page][ctrlType][ctrlMode][fxP].get("pValMin", 0.0).asDouble(), val - (double(E[i].left) / 127));
						TrackFX_SetParamNormalized(slGlobal.trackSel, fxid, trPlugs[fxNum]["Page"][page][ctrlType][ctrlMode][fxP].get("fNum", 0).asInt(), newval);
					}
					if (E[i].right) {
						double newval = __minVal(trPlugs[fxNum]["Page"][page][ctrlType][ctrlMode][fxP].get("pValMax", 1.0).asDouble(), val + (double(E[i].right) / 127));
						TrackFX_SetParamNormalized(slGlobal.trackSel, fxid, trPlugs[fxNum]["Page"][page][ctrlType][ctrlMode][fxP].get("fNum", 0).asInt(), newval);
					}
				}
			}
		}
	}
}
void SLCmd::InstE(vector<SLEncoder> &E) {


	//if (E[5].left) ApplyNudge(0, 2, 6, 2, E[5].left, true, 0);
	//if (E[5].right) ApplyNudge(0, 2, 6, 2, E[5].right, false, 0);

	if (E[0].left || E[0].right) {
		// Snap left trim to grid
		if (E[0].left) ApplyNudge(0, 2, 1, 2, E[0].left, true, 0);
		if (E[0].right) ApplyNudge(0, 2, 1, 2, E[0].right, false, 0);

	}

	if (E[1].left || E[1].right) {

		// Snap right edge to grid
		if (E[1].left) ApplyNudge(0, 2, 3, 2, E[1].left, true, 0);
		if (E[1].right) ApplyNudge(0, 2, 3, 2, E[1].right, false, 0);
	}

	if (E[2].left || E[2].right) {
		if (slGlobal.rsCmdE == 7) {
			// Scroll content snapped to grid	
			if (E[2].left) ApplyNudge(0, 2, 4, 2, E[2].left, true, 0);
			if (E[2].right) ApplyNudge(0, 2, 4, 2, E[2].right, false, 0);
		}
		else {
			// Nudge position
			if (E[2].left) ApplyNudge(0, 2, 0, 2, E[2].left, true, 0);
			if (E[2].right) ApplyNudge(0, 2, 0, 2, E[2].right, false, 0);
		}
	}

	if (E[3].left || E[3].right) {
		if (slGlobal.rsCmdE == 7) {
			// Duplicate items by measure
			if (E[3].left) ApplyNudge(0, 2, 5, 16, E[3].left * 4, true, 0);
			if (E[3].right) ApplyNudge(0, 2, 5, 16, E[3].right * 4, false, 0);
		}
		else {
			// Duplicate items by length
			if (E[3].left) ApplyNudge(0, 2, 5, 20, E[3].left, true, 0);
			if (E[3].right) ApplyNudge(0, 2, 5, 20, E[3].right, false, 0);
		}
	}


	//Adjust project's BPM
	if (E[4].left || E[4].right) {
		if (E[4].left) {
			if (slGlobal.rsCmdE == 6) RunCommand(41136); // Tempo: Decrease current project tempo 10 BPM
			else if (slGlobal.rsCmdE == 7) RunCommand(41138); // Tempo: Decrease current project tempo 0.1 BPM
			else RunCommand(41130); // Tempo: Decrease current project tempo 01 BPM
		}
		if (E[4].right) {
			if (slGlobal.rsCmdE == 6) RunCommand(41135); // Tempo: Increase current project tempo 10 BPM
			else if (slGlobal.rsCmdE == 7) RunCommand(41137); // Tempo: Increase current project tempo 0.1 BPM
			else RunCommand(41129); // Tempo: Increase current project tempo 01 BPM
		}
	}

	// Time selection start adjust
	if (E[5].left || E[5].right) {

		double startLoop;
		double endLoop;
		int cml;
		double fullbeats;
		GetSet_LoopTimeRange2(0, false, false, &startLoop, &endLoop, false);
		TimeMap2_timeToBeats(0, startLoop, NULL, &cml, &fullbeats, NULL);

		if (E[5].left) {
			if (slGlobal.rsCmdE == 6) startLoop = TimeMap2_beatsToTime(0, fullbeats - double(E[5].left) * cml, NULL);
			else if (slGlobal.rsCmdE == 7) startLoop = TimeMap2_beatsToTime(0, fullbeats - double(E[5].left) / 100, NULL);
			else startLoop = TimeMap2_beatsToTime(0, fullbeats - double(E[5].left), NULL);
		}

		if (E[5].right) {
			if (slGlobal.rsCmdE == 6) startLoop = TimeMap2_beatsToTime(0, fullbeats + double(E[5].right) * cml, NULL);
			else if (slGlobal.rsCmdE == 7) startLoop = TimeMap2_beatsToTime(0, fullbeats + double(E[5].right) / 100, NULL);
			else startLoop = TimeMap2_beatsToTime(0, fullbeats + double(E[5].right), NULL);
		}

		startLoop = __max(0, startLoop);
		GetSet_LoopTimeRange2(0, true, false, &startLoop, &endLoop, false);
	}

	// Time selection end adjust
	if (E[6].left || E[6].right) {

		double startLoop;
		double endLoop;
		int cml;
		double fullbeats;
		GetSet_LoopTimeRange2(0, false, false, &startLoop, &endLoop, false);
		TimeMap2_timeToBeats(0, endLoop, NULL, &cml, &fullbeats, NULL);

		if (E[6].left) {
			if (slGlobal.rsCmdE == 6) endLoop = TimeMap2_beatsToTime(0, fullbeats - double(E[6].left) * cml, NULL);
			else if (slGlobal.rsCmdE == 7) endLoop = TimeMap2_beatsToTime(0, fullbeats - double(E[6].left) / 100, NULL);
			else endLoop = TimeMap2_beatsToTime(0, fullbeats - double(E[6].left), NULL);
		}

		if (E[6].right) {
			if (slGlobal.rsCmdE == 6) endLoop = TimeMap2_beatsToTime(0, fullbeats + double(E[6].right) * cml, NULL);
			else if (slGlobal.rsCmdE == 7) endLoop = TimeMap2_beatsToTime(0, fullbeats + double(E[6].right) / 100, NULL);
			else endLoop = TimeMap2_beatsToTime(0, fullbeats + double(E[6].right), NULL);
		}
		endLoop = __max(0, endLoop);
		GetSet_LoopTimeRange2(0, true, false, &startLoop, &endLoop, false);
	}



	// Adjust project grid
	if (E[7].left || E[7].right) {
		double divisionInOut;
		int swingmodeInOut;
		double swingamtInOut;
		double newVal;
		GetSetProjectGrid(0, false, &divisionInOut, &swingmodeInOut, &swingamtInOut);

		//Grid: Adjust by 0.5
		if (E[7].left) {
			newVal = __max(0.0078125, divisionInOut / 2);
			GetSetProjectGrid(0, true, &newVal, &swingmodeInOut, &swingamtInOut);
		}
		//Grid: Adjust by 2
		if (E[7].right) {
			newVal = __min(4.0, divisionInOut * 2);
			GetSetProjectGrid(0, true, &newVal, &swingmodeInOut, &swingamtInOut);
		}
	}
}
void SLCmd::MixerE(vector<SLEncoder> &E) {
	for (int i = 0; i < 8; i++) {
		if (E[i].left || E[i].right) {
			int tid = slGlobal.trackBankStart + i;
			MediaTrack *tr = CSurf_TrackFromID(tid, false);

			if (tr) {
				if (slGlobal.rsCmdE == 0) {
					if (E[i].left) {
						int amt = __maxVal(0, volToChar(GetMediaTrackInfo_Value(tr, "D_VOL")) - E[i].left);
						SetMediaTrackInfo_Value(tr, "D_VOL", charToVol(amt));
						//CSurf_OnVolumeChangeEx(tr, charToVol(amt), 0, false);  // Not affecting other selected tracks
					}

					if (E[i].right) {
						int amt = __minVal(127, volToChar(GetMediaTrackInfo_Value(tr, "D_VOL")) + E[i].right);
						SetMediaTrackInfo_Value(tr, "D_VOL", charToVol(amt));
						//CSurf_OnVolumeChangeEx(tr, charToVol(amt), 0, false); // Not affecting other selected tracks
					}
				}
				else {
					for (int j = 0; j < 8; ++j) {
						if (slGlobal.rsCmdE == j + 1 && GetTrackNumSends(tr, 0) > j) {
							double vol;
							double pan;
							GetTrackSendUIVolPan(tr, j, &vol, &pan);

							if (E[i].left) SetTrackSendUIVol(tr, j, vol - double(E[i].left) / 127, 0);

							if (E[i].right) SetTrackSendUIVol(tr, j, vol + double(E[i].right) / 127, 0);
						}
					}
				}
			}
		}
	}
}

// SL.Pots ----------------------------------------
void SLCmd::UserP(vector<SLPot> &P) {}
void SLCmd::FxP(vector<SLPot> &P, Value &trPlugs) {
	for (int i = 0; i < 8; i++) {
		if (P[i].move != -1) {
			// First assignment
			if (P[i].prev == -1) P[i].prev = P[i].move;

			string page = slGlobal.trackFxPageSel;

			string ctrlType;
			string ctrlMode = to_string(slGlobal.rsCmdE);

			int fxid = slGlobal.trackFxSel - 1;
			string fxNumStr = to_string(fxid);

			(trPlugs[fxNumStr]["Page"][page]["ctrlP"]) ? ctrlType = "ctrlP" : ctrlType = "default";

			string fxP = to_string(i);

			if (trPlugs[fxNumStr]["Page"][page][ctrlType][ctrlMode][fxP]["fNum"]) {
				int val = int(round(TrackFX_GetParamNormalized(slGlobal.trackSel, fxid, trPlugs[fxNumStr]["Page"][page][ctrlType][ctrlMode][fxP].get("fNum", 0).asInt()) * 127));
				if (P[i].prev == val || (P[i].prev < val && P[i].move >= val) || (P[i].prev > val && P[i].move <= val)) {
					TrackFX_SetParamNormalized(slGlobal.trackSel, fxid, trPlugs[fxNumStr]["Page"][page][ctrlType][ctrlMode][fxP].get("fNum", 0).asInt(), double(P[i].move) / 127);
				}
			}
			P[i].prev = P[i].move;
		}
	}
}
void SLCmd::InstP(vector<SLPot> &P) {}
void SLCmd::MixerP(vector<SLPot> &P) {
	for (int i = 0; i < 8; i++) {
		if (P[i].move != -1) {
			if (P[i].prev == -1) P[i].prev = P[i].move;			// First assignment

			if (slGlobal.rsCmdP == 0) {
				int tid = slGlobal.trackBankStart + i;	// Project's track number (1-...)

				MediaTrack *tr = CSurf_TrackFromID(tid, false);							// Project's track number from ID
				double curPan = GetMediaTrackInfo_Value(tr, "D_PAN");			// Current pan value from Reaper
				double prev = charToPan(P[i].prev);
				double move = charToPan(P[i].move);

				if (tr) {

					if (
						(prev < curPan && move > curPan) ||
						(prev > curPan && move < curPan) ||
						(prev == curPan)) {
						SetMediaTrackInfo_Value(tr, "D_PAN", charToPan(P[i].move));
					}
				}
			}
			if (slGlobal.rsCmdP == 7) {
				int tid = slGlobal.trackBankStart + i;		// Project's track number (1-...)

				MediaTrack *tr = CSurf_TrackFromID(tid, false);							// Project's track number from ID
				int curPan = panToChar(GetMediaTrackInfo_Value(tr, "D_WIDTH"));			// Current pan value from Reaper
				double prev = charToPan(P[i].prev);
				double move = charToPan(P[i].move);

				if (tr) {
					if (
						(prev < curPan && move > curPan) ||
						(prev > curPan && move < curPan) ||
						(prev == curPan)) {
						SetMediaTrackInfo_Value(tr, "D_WIDTH", charToPan(P[i].move));
					}
				}
			}
			P[i].prev = P[i].move;
		}
	}
}

// SL.Faders --------------------------------------
void SLCmd::UserF(vector<SLFader> &F) {}
void SLCmd::FxF(vector<SLFader> &F, Value &trPlugs) {
	for (int i = 0; i < 8; i++) {
		if (F[i].move != -1) {
			// First assignment
			if (F[i].prev == -1) F[i].prev = F[i].move;

			string page = slGlobal.trackFxPageSel;

			string ctrlType;

			string ctrlMode = to_string(slGlobal.rsCmdE);

			int fxid = slGlobal.trackFxSel - 1;
			string fxNumStr = to_string(fxid);

			(trPlugs[fxNumStr]["Page"][page]["ctrlF"]) ? ctrlType = "ctrlF" : ctrlType = "default";

			string fxP = to_string(i);

			if (trPlugs[fxNumStr]["Page"][page][ctrlType][ctrlMode][fxP]["fNum"]) {
				double valMin;
				double valMax;
				int val = int(round(TrackFX_GetParam(slGlobal.trackSel, fxid, trPlugs[fxNumStr]["Page"][page][ctrlType][ctrlMode][fxP].get("fNum", 0).asInt(), &valMin, &valMax) * 127));
				double stepRange = (valMax - valMin);
				val = int(val / stepRange);

				double newVal = (double(F[i].move) / 127) * stepRange;

				if (F[i].prev == val || (F[i].prev < val && F[i].move >= val) || (F[i].prev > val && F[i].move <= val)) {
					TrackFX_SetParam(slGlobal.trackSel, fxid, trPlugs[fxNumStr]["Page"][page][ctrlType][ctrlMode][fxP].get("fNum", 0).asInt(), newVal);
				}
			}
			F[i].prev = F[i].move;
		}
	}
}
void SLCmd::InstF(vector<SLFader> &F) {}
void SLCmd::MixerF(vector<SLFader> &F) {
	for (int i = 0; i < 8; i++) {
		if (F[i].move != -1) {
			int tid = slGlobal.trackBankStart + i;

			MediaTrack *tr = CSurf_TrackFromID(tid, false);
			double curVol = GetMediaTrackInfo_Value(tr, "D_VOL");
			double prev = charToVol(F[i].prev);
			double move = charToVol(F[i].move);

			if (F[i].prev == -1) F[i].prev = F[i].state;							// First assignment

			if (tr) {
				if ((prev == curVol) ||
					(prev < curVol && move > curVol) ||
					(prev > curVol && F[i].move < curVol))
					SetMediaTrackInfo_Value(tr, "D_VOL", charToVol(F[i].move));
				//CSurf_OnVolumeChangeEx(tr, charToVol(F[i].move), false, false);
			}
			F[i].prev = F[i].move;
		}
	}
}

// SL.Row A --------------------------------------
void SLCmd::UserA(vector<SLButton> &A) {}
void SLCmd::FxA(vector<SLButton> &A) {
	for (int i = 0; i < 8; i++) {
		if (SLGlobal::rsCmdMain == 0) {
			// Select FX
			if (A[i].click) (i < slGlobal.trackFxMax) ? slGlobal.trackFxSel = i + 1 : slGlobal.trackFxSel = slGlobal.trackFxMax;
			// Toggle FX GUI open/close
			if (A[i].clickHold) {
				TrackFX_SetOpen(slGlobal.trackSel, i, !(TrackFX_GetOpen(slGlobal.trackSel, i)));
				(i < slGlobal.trackFxMax) ? slGlobal.trackFxSel = i + 1 : slGlobal.trackFxSel = slGlobal.trackFxMax;
			}
			// Enable/disable FX
			if (A[i].dblClick) {
				TrackFX_SetEnabled(slGlobal.trackSel, i, !(TrackFX_GetEnabled(slGlobal.trackSel, i)));
			}
			// Exclusively open FX GUI, closing other FX windows
			if (A[i].dblClickHold) {
				TrackFX_SetOpen(slGlobal.trackSel, i, true);
				RunCommand("_S&M_WNCLS6");
				(i < slGlobal.trackFxMax) ? slGlobal.trackFxSel = i + 1 : slGlobal.trackFxSel = slGlobal.trackFxMax;
			}
		}
		if (SLGlobal::rsCmdMain == 7) {}
	}


}
void SLCmd::InstA(vector<SLButton> &A) {
	// Markers: Edit time signature marker near cursor
	if (A[4].on) {
		A[4].on = false;
		RunCommand(40618);
	}
	// Set edit cursor at time sel start
	if (A[5].on) {
		A[5].on = false;
		double time;
		GetSet_LoopTimeRange2(0, false, false, &time, NULL, false);
		if (time) SetEditCurPos2(0, time, true, false);
	}
	// Set edit cursor at time sel end
	if (A[6].on) {
		A[6].on = false;
		double time;
		GetSet_LoopTimeRange2(0, false, false, NULL, &time, false);
		if (time) SetEditCurPos2(0, time, true, false);
	}
	//Grid: Adjust by 2
	if (A[7].on) {
		A[7].on = false;

		double divisionInOut;
		int swingmodeInOut;
		double swingamtInOut;
		double newVal;
		GetSetProjectGrid(0, false, &divisionInOut, &swingmodeInOut, &swingamtInOut);

		newVal = __min(4.0, divisionInOut * 2);
		GetSetProjectGrid(0, true, &newVal, &swingmodeInOut, &swingamtInOut);
	}
}
void SLCmd::MixerA(vector<SLButton> &A) {
	for (int i = 0; i < 8; i++) {
		int tid = slGlobal.trackBankStart + i; // Set track number from pressed button and current bank
		MediaTrack *tr = CSurf_TrackFromID(tid, false);

		if (tr) {
			if (SLGlobal::rsCmdMain == 0) {
				// Exclusive select track
				if (A[i].click) {
					SetOnlyTrackSelected(tr);
					if (CSurf_TrackToID(tr, false) < CSurf_TrackToID(GetMixerScroll(), false)) {
						SetMixerScroll(CSurf_TrackFromID(tid - i, false));
					}
				}
				// Toggle select track (non exclusive)
				if (A[i].clickHold) SetMediaTrackInfo_Value(tr, "I_SELECTED", !GetMediaTrackInfo_Value(tr, "I_SELECTED"));
				if (A[i].dblClick) SetOnlyTrackSelected(tr);
				// Delete track
				if (A[i].dblClickHold) {
					DeleteTrack(tr);
					Undo_OnStateChangeEx("Delete track(s) (SL)", UNDO_STATE_TRACKCFG, -1);
				}
			}
			if (SLGlobal::rsCmdMain == 1) {}
			if (SLGlobal::rsCmdMain == 2) {
				// Show track in Track view
				if (A[i].on) {
					A[i].on = false;
					SetMediaTrackInfo_Value(tr, "B_SHOWINTCP", !GetMediaTrackInfo_Value(tr, "B_SHOWINTCP"));
				}
			}
			if (SLGlobal::rsCmdMain == 3) {}
			if (SLGlobal::rsCmdMain == 4) {}
			if (SLGlobal::rsCmdMain == 5) {}
			if (SLGlobal::rsCmdMain == 6) {
				// PHASE track
				if (A[i].on) {
					A[i].on = false;
					SetMediaTrackInfo_Value(tr, "B_PHASE", !(GetMediaTrackInfo_Value(tr, "B_PHASE")));
				}
			}
			if (SLGlobal::rsCmdMain == 7) {
				// MUTE track
				if (A[i].on) {
					A[i].on = false;
					SetMediaTrackInfo_Value(tr, "B_MUTE", !GetMediaTrackInfo_Value(tr, "B_MUTE"));
				}
			}
		}
	}
}

// SL.Row B --------------------------------------
void SLCmd::UserB(vector<SLButton> &B) {}
void SLCmd::FxB(vector<SLButton> &B, Value &trPlugs) {
	for (int i = 0; i < 8; i++) {

		string ctrlType = "ctrlB";
		int fxid = slGlobal.trackFxSel - 1;

		if (B[i].on) {
			B[i].on = false;
			FxButton(ctrlType, fxid, i, trPlugs);
		}
	}
}
void SLCmd::InstB(vector<SLButton> &B) {
	if (B[0].on) {
		B[0].on = false;
		SetMediaTrackInfo_Value(slGlobal.trackSel, "B_MUTE", !(GetMediaTrackInfo_Value(slGlobal.trackSel, "B_MUTE")));
	}
	if (B[1].on) {
		B[1].on = false;
		(GetMediaTrackInfo_Value(slGlobal.trackSel, "I_SOLO")) ? SetMediaTrackInfo_Value(slGlobal.trackSel, "I_SOLO", 0) : SetMediaTrackInfo_Value(slGlobal.trackSel, "I_SOLO", 2);
	}
	// Xenakios/SWS: Switch item source file to previous in folder
	if (B[2].on) {
		B[2].on = false;
		RunCommand("_XENAKIOS_SISFTPREVIF");
	}
	// Xenakios / SWS: Switch item source file to next in folder
	if (B[3].on) {
		B[3].on = false;
		RunCommand("_XENAKIOS_SISFTNEXTIF");
	}
	// Markers: Delete time signature marker near cursor
	if (B[4].on) {
		B[4].on = false;
		RunCommand(40617);
	}
	// Time selection: Set start point
	if (B[5].on) {
		B[5].on = false;
		RunCommand(40625);
	}
	// Time selection: Set end point
	if (B[6].on) {
		B[6].on = false;
		RunCommand(40626);
	}

	//Grid: Adjust by 0.5
	if (B[7].on) {
		B[7].on = false;

		double divisionInOut;
		int swingmodeInOut;
		double swingamtInOut;
		double newVal;
		GetSetProjectGrid(0, false, &divisionInOut, &swingmodeInOut, &swingamtInOut);

		newVal = __max(0.0078125, divisionInOut / 2);
		GetSetProjectGrid(0, true, &newVal, &swingmodeInOut, &swingamtInOut);
	}
}
void SLCmd::MixerB(vector<SLButton> &B) {
	for (int i = 0; i < 8; i++) {
		if (B[i].on) {
			B[i].on = false;
			int tid = slGlobal.trackBankStart + i;
			MediaTrack *tr = CSurf_TrackFromID(tid, false);

			// Toggle RECORD ARM for track in current bank
			if (SLGlobal::rsCmdMain == 0) SetMediaTrackInfo_Value(tr, "I_RECARM", !(GetMediaTrackInfo_Value(tr, "I_RECARM")));

			// Show track in Mixer
			else if (SLGlobal::rsCmdMain == 2) SetMediaTrackInfo_Value(tr, "B_SHOWINMIXER", !(GetMediaTrackInfo_Value(tr, "B_SHOWINMIXER")));

			// Toggle RECORD MONITOR for track in current bank
			else if (SLGlobal::rsCmdMain == 3) SetMediaTrackInfo_Value(tr, "I_RECMON", !(GetMediaTrackInfo_Value(tr, "I_RECMON")));

			// Toggle PHASE for track in current bank
			else if (SLGlobal::rsCmdMain == 5) SetMediaTrackInfo_Value(tr, "B_PHASE", !(GetMediaTrackInfo_Value(tr, "B_PHASE")));

			// Cycle AUTOMATION MODE for track in current bank (trim/off, read, touch, write, latch)
			else if (SLGlobal::rsCmdMain == 6) {
				int autoMode = (int)GetMediaTrackInfo_Value(tr, "I_AUTOMODE");
				autoMode++;
				if (autoMode > 5) autoMode = 0;
				SetMediaTrackInfo_Value(tr, "I_AUTOMODE", autoMode);
			}

			// SOLO IN PLACE track
			else if (SLGlobal::rsCmdMain == 7) {
				if (GetMediaTrackInfo_Value(tr, "I_SOLO")) SetMediaTrackInfo_Value(tr, "I_SOLO", 0);
				else SetMediaTrackInfo_Value(tr, "I_SOLO", 2);
			}
		}
	}
}

// SL.Row C --------------------------------------
void SLCmd::UserC(vector<SLButton> &C) {
	if (!SLGlobal::transportLock) {}
}
void SLCmd::FxC(vector<SLButton> &C, Value &trPlugs) {
	if (!SLGlobal::transportLock) {
		for (int i = 0; i < 8; i++) {

			string ctrlType = "ctrlC";
			int fxid = slGlobal.trackFxSel - 1;

			if (C[i].on) {
				C[i].on = false;
				FxButton(ctrlType, fxid, i, trPlugs);
			}
		}
	}
}
void SLCmd::InstC(vector<SLButton> &C) {
	if (!SLGlobal::transportLock) {
		if (slGlobal.rsCmdMain == 0) {
			// SWS: Goto/select prev marker/region
			if (C[6].on) {
				C[6].on = false;
				RunCommand("_SWS_SELPREVMORR");

			}
			// SWS: Goto/select next marker/region
			if (C[7].on) {
				C[7].on = false;
				RunCommand("_SWS_SELNEXTMORR");
			}
		}
	}
}
void SLCmd::MixerC(vector<SLButton> &C) {
	if (!SLGlobal::transportLock) {
		for (int i = 0; i < 8; i++) {
			if (C[i].on) {
				C[i].on = false;
				int tid = slGlobal.trackBankStart + i;
				MediaTrack *tr = CSurf_TrackFromID(tid, false);

				// Toggle MUTE for track in current track
				if (SLGlobal::rsCmdMain == 0) {
					SetMediaTrackInfo_Value(tr, "B_MUTE", !(GetMediaTrackInfo_Value(tr, "B_MUTE")));
				}
				// Markers: Go to marker 1-8
				if (SLGlobal::rsCmdMain == 6) GoToMarker(0, i + 1, true);
				// Regions: Go to region 1-8 after current region finishes playing(smooth seek)
				if (SLGlobal::rsCmdMain == 7) GoToRegion(0, i + 1, true);
			}
		}
	}
}

// SL.Row D --------------------------------------
void SLCmd::UserD(vector<SLButton> &D) {
	if (!SLGlobal::transportLock) {}
}
void SLCmd::FxD(vector<SLButton> &D, Value &trPlugs) {
	if (!SLGlobal::transportLock) {
		for (int i = 0; i < 8; i++) {

			string ctrlType = "ctrlD";
			int fxid = slGlobal.trackFxSel - 1;

			if (D[i].on) {
				D[i].on = false;
				FxButton(ctrlType, fxid, i, trPlugs);
			}
		}
	}
}
void SLCmd::InstD(vector<SLButton> &D) {
	if (!SLGlobal::transportLock) {
		if (slGlobal.rsCmdMain == 0) {
			// SWS : Select prev region
			if (D[6].on) {
				D[6].on = false;
				RunCommand("_SWS_SELPREVREG");

			}
			// SWS : Select next region
			if (D[7].on) {
				D[7].on = false;
				RunCommand("_SWS_SELNEXTREG");
			}
		}
	}
}
void SLCmd::MixerD(vector<SLButton> &D) {
	if (!SLGlobal::transportLock) {
		for (int i = 0; i < 8; i++) {
			if (D[i].on) {
				D[i].on = false;
				int tid = slGlobal.trackBankStart + i;
				MediaTrack *tr = CSurf_TrackFromID(tid, false);

				// Toggle SOLO IN PLACE for track in current track
				if (SLGlobal::rsCmdMain == 0) {
					if (GetMediaTrackInfo_Value(tr, "I_SOLO")) SetMediaTrackInfo_Value(tr, "I_SOLO", 0);
					else SetMediaTrackInfo_Value(tr, "I_SOLO", 2);
				}
				// Toggle SOLO (ignore routing) for track in current track
				if (SLGlobal::rsCmdMain == 4) {
					if (GetMediaTrackInfo_Value(tr, "I_SOLO")) SetMediaTrackInfo_Value(tr, "I_SOLO", 0);
					else SetMediaTrackInfo_Value(tr, "I_SOLO", 1);
				}
				// Markers: Go to marker 9-16
				if (SLGlobal::rsCmdMain == 6) GoToMarker(0, i + 9, true);
				// Regions: Go to region 9-16 after current region finishes playing(smooth seek)
				if (SLGlobal::rsCmdMain == 7) GoToRegion(0, i + 9, true);
			}
		}
	}
}

// SL.Drumpads ----------------------------------
void SLCmd::UserDr(MIDI_event_t * evt, vector<SLDrumpad> &Dr, SLSpeedDial &Sd) {}
void SLCmd::FxDr(MIDI_event_t * evt, vector<SLDrumpad> &Dr, SLSpeedDial &Sd) {
	int msg1 = evt->midi_message[0];
	int msg2 = evt->midi_message[1];
	int msg3 = evt->midi_message[2];

	for (int i = 0; i < 8; ++i) {
		if (msg2 == Dr[i].cc) {
			if (slGlobal.trackFxPageMax > i) slGlobal.trackFxPageSel = to_string(i + 1);
		}
	}
}
void SLCmd::InstDr(MIDI_event_t * evt, vector<SLDrumpad> &Dr, SLSpeedDial &Sd) {
	int msg1 = evt->midi_message[0];
	int msg2 = evt->midi_message[1];
	int msg3 = evt->midi_message[2];

	// Set project grid when Speed Dial is touched
	if (Sd.touchState) {
		if (msg2 == Dr[0].cc) RunCommand(41047); // 1/128
		if (msg2 == Dr[1].cc) RunCommand(40774); // 1/64
		if (msg2 == Dr[2].cc) RunCommand(40775); // 1/32
		if (msg2 == Dr[3].cc) RunCommand(40776); // 1/16
		if (msg2 == Dr[4].cc) RunCommand(40778); // 1/8
		if (msg2 == Dr[5].cc) RunCommand(40779); // 1/4
		if (msg2 == Dr[6].cc) RunCommand(40780); // 1/2
		if (msg2 == Dr[7].cc) RunCommand(40781); // 1
	}
}
void SLCmd::MixerDr(MIDI_event_t * evt, vector<SLDrumpad> &Dr, SLSpeedDial &Sd) {

	int msg1 = evt->midi_message[0];
	int msg2 = evt->midi_message[1];
	int msg3 = evt->midi_message[2];

	// Set project grid when Speed Dial is touched
	if (Sd.touchState) {
		if (msg2 == Dr[0].cc) RunCommand(41047); // 1/128
		if (msg2 == Dr[1].cc) RunCommand(40774); // 1/64
		if (msg2 == Dr[2].cc) RunCommand(40775); // 1/32
		if (msg2 == Dr[3].cc) RunCommand(40776); // 1/16
		if (msg2 == Dr[4].cc) RunCommand(40778); // 1/8
		if (msg2 == Dr[5].cc) RunCommand(40779); // 1/4
		if (msg2 == Dr[6].cc) RunCommand(40780); // 1/2
		if (msg2 == Dr[7].cc) RunCommand(40781); // 1
	}

	// Reset pan for tracks 1-8 in current bank
	else if (SLPot::touchGlobal) {
		for (int i = 0; i < 8; i++) {
			if (msg2 == Dr[i].cc) {
				int tid = slGlobal.trackBankStart + i;
				MediaTrack *tr = CSurf_TrackFromID(tid, false);
				if (tr)  SetMediaTrackInfo_Value(tr, "D_PAN", 0);
			}
		}
	}

	// Reset vol for tracks 1-8 in current bank
	else if (SLEncoder::touchGlobal) {
		for (int i = 0; i < 8; i++) {
			if (msg2 == Dr[i].cc) {
				int tid = slGlobal.trackBankStart + i;
				MediaTrack *tr = CSurf_TrackFromID(tid, false);
				if (tr)  SetMediaTrackInfo_Value(tr, "D_VOL", 1);
			}
		}
	}

	// Actions in combination with row select buttons
	else {
		for (int i = 0; i < 8; i++) {
			// Scroll mixer to the first track in current bank
			if (SLGlobal::rsCmdMain == 0 && msg2 == Dr[i].cc) {
				slGlobal.trackBankStart = 1 + (i * 8);											// Select bank 1-8 of tracks

				MediaTrack *tr = CSurf_TrackFromID(slGlobal.trackBankStart, false);
				SetMixerScroll(tr);											// Scroll mixer to the first track in current bank
			}
			else if (SLGlobal::rsCmdMain == 1 && msg2 == Dr[i].cc) slGlobal.trackBankStart = 1 + (i * 8);
			else if (SLGlobal::rsCmdMain == 2 && msg2 == Dr[i].cc) slGlobal.trackBankStart = 1 + (i * 8);
			else if (SLGlobal::rsCmdMain == 3 && msg2 == Dr[i].cc) slGlobal.trackBankStart = 1 + (i * 8);
			else if (SLGlobal::rsCmdMain == 4 && msg2 == Dr[i].cc) slGlobal.trackBankStart = 1 + (i * 8);
			else if (SLGlobal::rsCmdMain == 5 && msg2 == Dr[i].cc) slGlobal.trackBankStart = 1 + (i * 8);
			else if (SLGlobal::rsCmdMain == 6 && msg2 == Dr[i].cc) slGlobal.trackBankStart = 1 + (i * 8);
			else if (SLGlobal::rsCmdMain == 7 && msg2 == Dr[i].cc) slGlobal.trackBankStart = 1 + (i * 8);

			if (slGlobal.trackBankStart > slGlobal.trackMax) slGlobal.trackBankStart = slGlobal.trackMax; // Always select highest bank available if exceeds
		}
	}
}