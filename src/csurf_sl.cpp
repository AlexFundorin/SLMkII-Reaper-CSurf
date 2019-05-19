
/*
Useful functions:

code to display text:
char str[512];
sprintf(str, "%i\n", amt);
ShowConsoleMsg(str);

code to detect variable type:
typeid(variable).name()


code to output json obeject into file:
ofstream outFile(format("{}\\test.json", plugmapsUserPath), ios_base::out | ios_base::trunc);
outFile << projPlugs << endl;
outFile.close();


*/

/*
 Might use these commands in project:

void DeleteTrack(MediaTrack* tr) deletes a track

I_FXEN : int * : 0 = fx bypassed, nonzero = fx active
I_WNDH : int * : current TCP window height(Read - only)
I_CUSTOMCOLOR : int * : custom color, OS dependent color | 0x100000 (i.e.ColorToNative(r, g, b) | 0x100000).If you do not | 0x100000, then it will not be used(though will store the color anyway).
C_BEATATTACHMODE : char * : char * to one char of beat attached mode, -1 = def, 0 = time, 1 = allbeats, 2 = beatsposonly
I_FOLDERDEPTH : int * : folder depth change(0 = normal, 1 = track is a folder parent, -1 = track is the last in the innermost folder, -2 = track is the last in the innermost and next - innermost folders, etc
I_FOLDERCOMPACT : int * : folder compacting(only valid on folders), 0 = normal, 1 = small, 2 = tiny children

Actions:
40113 View: Toggle track zoom to maximum height
40110 View: Toggle track zoom to minimum height
40723 View: Expand selected track height, minimize others

40271 View: Show FX browser window
40378 View: Show big clock window
40268 View: Show navigator window
40326 View: Show region/marker manager window

40036 View: Toggle auto-view-scroll during playback
40262 View: Toggle auto-view-scroll while recording

40113 View: Toggle track zoom to maximum height
40110 View: Toggle track zoom to minimum height

41622 View: Toggle zoom to selected items


40031 View: Zoom time selection

41591 Track: Toggle tracks visible (hide) in TCP
41592 Track: Toggle tracks visible (hide) in mixer

41919 Track: Toggle preserve PDC delayed monitoring in recorded items


_WOL_SETHZOOMC_CENTERVIEW SWS / wol: Options - Set "Horizontal zoom center" to "Center of view"
_WOL_SETHZOOMC_EDITPLAYCUR SWS / wol : Options - Set "Horizontal zoom center" to "Edit cursor or play cursor (default)"
_WOL_SETHZOOMC_EDITCUR SWS / wol : Options - Set "Horizontal zoom center" to "Edit cursor"
_WOL_SETHZOOMC_MOUSECUR SWS / wol : Options - Set "Horizontal zoom center" to "Mouse cursor"

_WOL_SETVZOOMC_LASTSELTRACK SWS / wol : Options - Set "Vertical zoom center" to "Last selected track"
_WOL_SETVZOOMC_TOPVISTRACK SWS / wol : Options - Set "Vertical zoom center" to "Top visible track"
_WOL_SETVZOOMC_TRACKCVIEW SWS / wol : Options - Set "Vertical zoom center" to "Track at center of view"
_WOL_SETVZOOMC_TRACKMOUSECUR SWS / wol : Options - Set "Vertical zoom center" to "Track under mouse cursor"

//if (evt->midi_message[1] == Tr[0].cc && evt->midi_message[2] == 1) Main_OnCommand(40151, 0);		// View: Go to cursor
//if (evt->midi_message[1] == Tr[1].cc && evt->midi_message[2] == 1) Main_OnCommand(40150, 0);		// View : Go to play position



*/

#include "csurf_sl.h"

using namespace std;
using namespace fmt;
using namespace Json;

static regex operator""_r(const char* r, size_t ct) { return regex(r, ct); }

// Startup actions (performed once)
void CSurf_SL::slStartup() {
	slSysex.Send(slSysex.online, midi_out);						// Send Automap online to controller
	slSysex.Send(slSysex.clearBothDisplays, midi_out);				// Clear LCD

	string first = "                  Novation SL Mk II sctipt for Reaper                   ";
	string second = "                  Novation SL Mk II sctipt for Reaper                   ";
	string third = "                           (made by fundorin)                           ";
	string forth = "                           (made by fundorin)                           ";

	slSysex.SendString(first, 0, 1, midi_out);
	slSysex.SendString(second, 0, 2, midi_out);
	slSysex.SendString(third, 0, 3, midi_out);
	slSysex.SendString(forth, 0, 4, midi_out);


	midi_out->Send(slGlobal.cc, slAm[6].cc, 1, -1);		// Turn all LEDs off

	midi_out->Send(slGlobal.cc, tLockReq, 1, -1);		// Request Transport-Lock state

	surfaceMode = 3;								// Select MIXER mode
	surfaceModeLast = surfaceMode;

	slGlobal.trackBankSel = 0;								// Reset track's bank selection

	rsCurMain = rsDefMain;
	rsCurE = rsDefE;
	rsCurP = rsDefP;
	rsCurF = rsDefF;
	rsCurSd = rsDefSd;

	midi_reinit();									// Reset all MIDI devices in Reaper

	Main_OnCommand(41743, 0);						// Refresh all surfaces

	slDisplay.UpdateTimer.start();							// Start LCD refresh timer
	slLed.ledVuUpdateTimer.start();						// Start LED liveVU refresh timer

	GetPlayState();

	plugmapsPath = format("{}SLPlugmaps\\!Generated\\", userPath); // set plugmaps path
	if (!fexists(plugmapsPath)) fs::create_directories(plugmapsPath);
	plugmapsUserPath = format("{}SLPlugmaps\\", userPath); // set plugmaps path

	g_plugin_chain_window = new SLWindow("SL", 900, 300, false, juce::Colour());

	runStart = false;				// Disable constant run of this function
}
// Define number of banks in project
void CSurf_SL::banksDefine() {
	slGlobal.trackMax = CountTracks(0);
	if (slGlobal.trackMax > 0) {
		//slTrackBankMax = (slTrackMax - 1) / 8; // Highest bank number

		if (slGlobal.trackBankStart < 1) slGlobal.trackBankStart = 1;
		if (slGlobal.trackBankStart > slGlobal.trackMax) slGlobal.trackBankStart = slGlobal.trackMax;

		slGlobal.track = CSurf_TrackFromID(1, false);

		// Define first selected track or first track in bank
		if (GetSelectedTrack(0, 0)) slGlobal.trackSel = GetSelectedTrack(0, 0);
		else if (GetLastTouchedTrack()) slGlobal.trackSel = GetLastTouchedTrack();
		else slGlobal.trackSel = CSurf_TrackFromID(slGlobal.trackBankStart, false);

		slGlobal.trackFxMax = TrackFX_GetCount(slGlobal.trackSel);
		slTrackFxBank = (slGlobal.trackFxMax - 1) / 8;

		if (slGlobal.trackSelPrev != slGlobal.trackSel) {
			slGlobal.trackSelPrev = slGlobal.trackSel;
			slTrackFxBank = 0;
			slGlobal.trackFxSel = 1;
		}
		if (slGlobal.trackFxSel != slGlobal.trackFxSelPrev) {
			slGlobal.trackFxSelPrev = slGlobal.trackFxSel;
			slGlobal.trackFxPageSel = "1";
		}

		string fxNum = to_string(slGlobal.trackFxSel - 1);
		slGlobal.trackFxPageMax = trPlugs[fxNum]["Page"].size();

		if (slGlobal.trackFxSel != slGlobal.trackFxSelPrev) {
			slGlobal.trackSelPrev = slGlobal.trackSel;
			slGlobal.trackFxPageSel = "1";
		}
	}
	else {
		slGlobal.trackBankStart = 1;
		slGlobal.trackSel = 0;
	}
}

//void CSurf_SL::banksDefine() {
//	slGlobal.trackMax = CountTracks(0);
//	if (slGlobal.trackMax > 0) {
//		slGlobal.trackBankMax = (slGlobal.trackMax - 1) / 8; // Highest bank number
//
//		slGlobal.track = CSurf_TrackFromID(1, false);
//
//		// Define first selected track or first track in project
//		(GetSelectedTrack(0, 0)) ? slGlobal.trackSel = GetSelectedTrack(0, 0) : slGlobal.trackSel = CSurf_TrackFromID(1, false);
//
//
//		slGlobal.trackFxMax = TrackFX_GetCount(slGlobal.trackSel);
//		slTrackFxBank = (slGlobal.trackFxMax - 1) / 8;
//
//		if (slGlobal.trackSelPrev != slGlobal.trackSel) {
//			slGlobal.trackSelPrev = slGlobal.trackSel;
//			slTrackFxBank = 0;
//			slGlobal.trackFxSel = 1;
//		}
//		if (slGlobal.trackFxSel != slGlobal.trackFxSelPrev) {
//			slGlobal.trackFxSelPrev = slGlobal.trackFxSel;
//			slGlobal.trackFxPageSel = "1";
//		}
//
//		string fxNum = to_string(slGlobal.trackFxSel - 1);
//		slGlobal.trackFxPageMax = trPlugs[fxNum]["Page"].size();
//
//		if (slGlobal.trackFxSel != slGlobal.trackFxSelPrev) {
//			slGlobal.trackSelPrev = slGlobal.trackSel;
//			slGlobal.trackFxPageSel = "1";
//		}
//	}
//	else {
//		slGlobal.trackBankMax = 0;
//		slGlobal.trackSel = 0;
//	}
//}


void CSurf_SL::resizeTracks() {
	auto calcSize = CountTracks(0) + 1;
	if (reaTrack.size() != calcSize) {
		reaTrack.reserve(calcSize + 64);
		reaTrack.resize(calcSize);
	}
}

// ---------- PARSERS -------------------------------
//void CSurf_SL::SysexParser(MIDI_event_t * evt) {
//
//	unsigned char msg[1024];
//	memcpy(msg, evt->midi_message, sizeof(evt->midi_message)); // Full midi message. 512 bytes, as declared in reaper_plugin.h, line 202
//
//	if (evt->midi_message[0] == 0xF0) {
//
//		unsigned char online[] = { 0xF0, 0x00, 0x20, 0x29, 0x03, 0x03, 0x12, 0x00, 0x02, 0x00, 0x01, 0x01, 0xF7 };
//		unsigned char offline[] = { 0xF0, 0x00, 0x20, 0x29, 0x03, 0x03, 0x12, 0x00, 0x02, 0x00, 0x01, 0x00, 0xF7 };
//
//		// Compare arrays
//		if (!memcmp(msg, online, sizeof(online))) {
//			automapStatus = true;
//			LedFeedbackReset();
//		}
//		else if (!memcmp(msg, offline, sizeof(offline))) {
//			automapStatus = false;
//			sysex.Send(sysex.clearBothDisplays, midi_out);			// Send clear both lcds
//			midi_out->Send(slGlobal.cc, Am[6].cc, 1, -1);	// Turn all LEDs off
//		}
//	}
//}
void CSurf_SL::MidiParser(MIDI_event_t * evt) {
	int msg1 = evt->midi_message[0];
	int msg2 = evt->midi_message[1];
	int msg3 = evt->midi_message[2];

	if (msg1 == slGlobal.cc) {

		for (int i = 0; i < 8; i++) {
			if (msg2 == slE[i].cc) {						// Encoders
				slE[i].state = msg3;
				slE[i].move = true;
			}
			if (msg2 == slP[i].cc) slP[i].state = msg3;		// Pots
			if (msg2 == slF[i].cc) slF[i].state = msg3;		// Faders

			if (msg2 == slA[i].cc) slA[i].SetState(msg3);
			if (msg2 == slB[i].cc) slB[i].SetState(msg3);
			if (msg2 == slC[i].cc) slC[i].SetState(msg3);
			if (msg2 == slD[i].cc) slD[i].SetState(msg3);

			if (msg2 == slRs[i].cc) slRs[i].SetState(msg3);

			if (msg2 == slTr[i].cc && msg3 <= 1) slTr[i].SetState(msg3);
			if (msg2 == slAm[i].cc && msg3 >= 64) slAm[i].SetState(msg3 - 64);
		}

		// Page select
		for (int i = 0; i < 4; i++) if (msg2 == slPg[i].cc) slPg[i].SetState(msg3);

		// Touch states		
		if (msg2 == SLEncoder::touch) (msg3 >= 64) ? SLEncoder::touchGlobal = slE[msg3 - 64].touchState = true : SLEncoder::touchGlobal = slE[msg3].touchState = false;
		if (msg2 == SLPot::touch) (msg3 >= 64) ? SLPot::touchGlobal = slP[msg3 - 64].touchState = true : SLPot::touchGlobal = slP[msg3].touchState = false;
		if (msg2 == SLFader::touch) (msg3 >= 64) ? SLFader::touchGlobal = slF[msg3 - 64].touchState = true : SLFader::touchGlobal = slF[msg3].touchState = false;

		if (msg2 == slSd.touchCc) msg3 == 64 ? slSd.touchState = true : slSd.touchState = false;			// Speed Dial touched
		if (msg2 == slSd.cc) slSd.SetRotate(msg3);														// Speed Dial rotation
		if (msg2 == slSd.btnCc) slSd.SetButton(msg3);													// Speed Dial Button
	}

	// Drumpads (Note ON)
	if (msg1 == slGlobal.cc) {
		for (int i = 0; i < 8; i++) if (msg2 == slDr[i].cc) slDr[i].state = msg3;
	}
}
void CSurf_SL::CmdParser() {
	for (int i = 0; i < 8; i++) {
		slE[i].Parse();						// Encoders
		slP[i].Parse();						// Pots
		slF[i].Parse();						// Faders

		slA[i].Parse();
		slB[i].Parse();
		slC[i].Parse();
		slD[i].Parse();

		slRs[i].Parse();

		slTr[i].Parse();
		slAm[i].Parse();
	}

	// Page select
	for (int i = 0; i < 4; i++) slPg[i].Parse();
	// Speed Dial
	slSd.Parse();
}
void CSurf_SL::FxParser() {
	int fxMax = TrackFX_GetCount(slGlobal.trackSel);
	if (fxMax > 0) {
		for (int i = 0; i < fxMax; ++i) {
			int fxNum = i;
			char fxNameChar[512];											// Prepare buffer for FX name
			TrackFX_GetFXName(slGlobal.trackSel, fxNum, fxNameChar, 512);			// Get FX name for selected track

			string fxName = format("{}", fxNameChar);						// Convert name from char to string
			string fxDev;													// Prepare string for dev name
			string fxType;													// Prepare string for dev name

			string fxNameFull = fxName;										// Copy orinal FX name

			slDisplay.FxNameFormat(fxType, fxName, fxDev);					// Extract name and dev from FX name string
			string fxNameReduced = fxName;									// Copy orinal FX name
			slDisplay.ReduceString(&fxNameReduced, 8);
			//string fxDevFull = fxDev;										// Copy orinal FX developer name

			//Test to write full plugmap names
			fxName = format("{}", fxNameChar);
			fxName = regex_replace(fxName, regex("\\:"), " -");

			fxName = regex_replace(fxName, regex("[^\\w\\s-\\(\\)\\+\\.]+"), "");	// Remove restricted chars from name
			fxDev = regex_replace(fxDev, regex("[^\\w\\s-\\.]+"), "");		// Remove restricted chars from dev
			fxType = regex_replace(fxDev, regex("[^\\w\\s-\\.]+"), "");		// Remove restricted chars from type

			fxDevPath = format("{}{}", plugmapsPath, fxDev);
			fxFilename = format("{0}\\{1}.json", fxDevPath, fxName);
			fxDevUserPath = format("{}{}", plugmapsUserPath, fxDev);
			fxUserFilename = format("{0}\\{1}.json", fxDevUserPath, fxName);

			if (!fexists(fxUserFilename) && !fexists(fxFilename)) {
				//Create dev folder in !gen
				if (!fexists(fxDevPath)) fs::create_directory(fxDevPath);

				// Create main object
				Json::Value rootNode(objectValue);

				// Create information object
				Json::Value info(objectValue);

				info["Name"] = fxName;
				info["NameFull"] = fxNameFull;
				info["NameShort"] = fxNameReduced;
				info["Dev"] = fxDev;
				info["CreateAuthor"] = "Automatic";
				info["CreateVersion"] = 1;
				info["PlugmapComment1"] = "comment one";
				info["PlugmapComment2"] = "comment two";
				info["PlugmapComment3"] = "comment three";

				time_t rawtime;
				struct tm * timeinfo;
				char buffer[80];
				time(&rawtime);
				timeinfo = localtime(&rawtime);
				strftime(buffer, 80, "%Y-%m-%d", timeinfo);
				info["CreateDate"] = format("{}", buffer);

				// Dump info section object into main object
				rootNode["Info"] = info;

				Json::Value pages(objectValue);

				for (int k = 0; k < (TrackFX_GetNumParams(slGlobal.trackSel, fxNum) / 8) + 1; ++k) {

					Json::Value fxparams(objectValue);

					for (int j = 0; j < 8; ++j) {
						if (TrackFX_GetNumParams(slGlobal.trackSel, fxNum) > j + (8 * k)) {
							char fxParmNameChar[512];
							TrackFX_GetParamName(slGlobal.trackSel, fxNum, j + (8 * k), fxParmNameChar, 512);	// Get FX parm value for selected track
																										//fxparams[format("{}", j)]["parmNameFull"] = fxParmNameChar;
							string fxParmNameStr = format("{}", fxParmNameChar);
							fxName = regex_replace(fxParmNameStr, regex("[^\\w\\s-\\(\\)\\+\\.]+"), "");
							fxparams[to_string(j)]["pNameFull"] = fxParmNameStr;

							slDisplay.ReduceString(&fxParmNameStr, 8);
							fxparams[to_string(j)]["pNameShort"] = fxParmNameStr;

							fxparams[to_string(j)]["fNum"] = j + (8 * k);
							fxparams[to_string(j)]["cMode"] = 0;
							fxparams[to_string(j)]["cFeedback"] = 0;

							fxparams[to_string(j)]["_comment"] = "comment";

							double step = 0.0;
							double fine = 0.1;
							double coarse = 10.0;
							bool toggle = false;
							TrackFX_GetParameterStepSizes(slGlobal.trackSel, fxNum, j + (8 * k), &step, &fine, &coarse, &toggle);

							fxparams[to_string(j)]["pToggle"] = toggle;
							fxparams[to_string(j)]["pStepNum"] = int(step);
							if (toggle) {
								fxparams[to_string(j)]["pStepNorm"] = "1";
								fxparams[to_string(j)]["pStepFine"] = "1";
								fxparams[to_string(j)]["pStepCoarse"] = "1";
							}
							else if (!step) {
								fxparams[to_string(j)]["pStepNorm"] = format("{}", 1);
								fxparams[to_string(j)]["pStepFine"] = format("{:.1}", 0.1);
								fxparams[to_string(j)]["pStepCoarse"] = format("{}", 10);
							}
							else {
								fxparams[to_string(j)]["pStepNorm"] = format("{:.3}", step);
								fxparams[to_string(j)]["pStepFine"] = format("{:.3}", fine);
								fxparams[to_string(j)]["pStepCoarse"] = format("{:.3}", coarse);
							}

							Json::Value stepVal(arrayValue);
							fxparams[to_string(j)]["pStepVal"] = stepVal;

							double minval;
							double midval;
							double maxval;
							TrackFX_GetParamEx(slGlobal.trackSel, fxNum, j * k, &minval, &maxval, &midval);
							fxparams[to_string(j)]["pValMin"] = minval;
							fxparams[to_string(j)]["pValMid"] = midval;
							fxparams[to_string(j)]["pValMax"] = maxval;
							fxparams[to_string(j)]["pReverse"] = false;
							fxparams[to_string(j)]["pValName"] = "";
							fxparams[to_string(j)]["pLoop"] = false;
						}
					}
					pages[to_string(k + 1)]["_PageName"] = format("{}. Default", k + 1);
					pages[to_string(k + 1)]["default"]["0"] = fxparams;

				}
				rootNode["Page"] = pages;

				ofstream outFile(fxFilename, ios_base::out | ios_base::trunc);
				outFile << rootNode << endl;
				outFile.close();
			}
			else {
				// Add all available FX from first selected track or first track in the project into buffer
				if (!projPlugs[fxName]) {
					if (!fexists(fxUserFilename)) {
						ifstream istr(fxFilename);
						istr >> projPlugs[fxName];
					}
					else {
						ifstream istr(fxUserFilename);
						istr >> projPlugs[fxName];
					}
					//ofstream outFile(format("{}\\test.json", plugmapsUserPath), ios_base::out | ios_base::trunc);
					//outFile << projPlugs << endl;
					//outFile.close();
				}
				// Only rewrite FX slot if FX in slot changed
				if (trPlugs[to_string(i)]["Info"]["Name"] != fxName) {
					trPlugs[to_string(i)] = projPlugs[fxName];
				}
			}
		}
	}
}

// ---------- ROW SELECT ----------------------------
void CSurf_SL::RowSelectLed() {

	// Do not enable. Ever. Messes up with REC LED and maybe some other stuff
	//for (int i = 0; i < 8; i++) {
	//	if (Rs[i].led != Rs[i].hold) {
	//		Rs[i].led = Rs[i].hold;
	//		midi_out->Send(slGlobal.cc, Rs[i].cc, Rs[i].led, 0);
	//	}
	//}
}
void CSurf_SL::RowSelectCmd() {
	for (int i = 0; i < 8; i++) {
		// Toggle temp RS mode for controls
		if (slRs[i].hold) SLGlobal::rsCmdMain = i;
		if (slRs[i].hold) slGlobal.rsCmdE = i;
		if (slRs[i].hold) slGlobal.rsCmdP = i;
		if (slRs[i].hold) slGlobal.rsCmdF = i;
		if (slRs[i].hold) slGlobal.rsCmdSd = i;

		// Toggle permanent RS mode for controls
		if (slRs[i].dblClick) {
			// Add many good actions here!
		}
		// Restore default RS mode for controls
		if (slRs[i].dblClickHold) {
			if (SLEncoder::touchGlobal) (rsCurE != i) ? rsCurE = i : rsCurE = rsDefE;
			else if (SLPot::touchGlobal) (rsCurP != i) ? rsCurP = i : rsCurP = rsDefP;
			else if (SLFader::touchGlobal) (rsCurF != i) ? rsCurF = i : rsCurF = rsDefF;
			else if (slSd.touchState) (rsCurSd != i) ? rsCurSd = i : rsCurSd = rsDefSd;
			//else (rsCurMain != i) ? SLGlobal::rsCmdMain = i : rsCurMain = rsDefMain;
		}
	}

	if (!slRs[0].hold && !slRs[1].hold && !slRs[2].hold && !slRs[3].hold && !slRs[4].hold && !slRs[5].hold && !slRs[6].hold && !slRs[7].hold) {
		SLGlobal::rsCmdMain = rsCurMain;
		slGlobal.rsCmdE = rsCurE;
		slGlobal.rsCmdP = rsCurP;
		slGlobal.rsCmdF = rsCurF;
		slGlobal.rsCmdSd = rsCurSd;
	}
}

// ---------- LEARN AND VIEW ------------------------
void CSurf_SL::LearnViewLed() {
	if (slAm[0].led = !slAm[0].hold) {
		slAm[0].led = slAm[0].hold;
		midi_out->Send(slGlobal.cc, slAm[0].cc, slAm[0].led, 0);
	}
	if (slAm[1].led = !slAm[1].hold) {
		slAm[1].led = slAm[1].hold;
		midi_out->Send(slGlobal.cc, slAm[1].cc, slAm[1].led, 0);
	}
}


void CSurf_SL::LearnViewCmd() {
	//if (slAm[0].on) {
	//	if (!g_plugin_chain_window->isOnDesktop()) {
	//		g_plugin_chain_window->addToDesktop(g_plugin_chain_window->getDesktopWindowStyleFlags(), GetMainHwnd());
	//		g_plugin_chain_window->setSize(600, 300);
	//		g_plugin_chain_window->setVisible(true);
	//	}
	//	else {
	//		//g_plugin_chain_window->setVisible(false);
	//		g_plugin_chain_window->removeFromDesktop();
	//	}
	//}
	if (slAm[1].click) {

	}
}

// ---------- DRUMPADS ------------------------------
void CSurf_SL::DrumpadsCmd(MIDI_event_t * evt)
{
	if (evt->midi_message[0] == slGlobal.note) {

		if (surfaceMode == 0) slCmd.UserDr(evt, slDr, slSd);
		if (surfaceMode == 1) slCmd.FxDr(evt, slDr, slSd);
		if (surfaceMode == 2) slCmd.InstDr(evt, slDr, slSd);
		if (surfaceMode == 3) slCmd.MixerDr(evt, slDr, slSd);
	}
}

// All LCD feedback happens here
void CSurf_SL::ModeSelectorLcd() {

	// LCD feedback for USER mode
	if (surfaceMode == 0) LcdUserMode();
	// LCD feedback for FX mode
	else if (surfaceMode == 1) LcdFxMode();
	// LCD feedback for INST mode
	else if (surfaceMode == 2) LcdInstMode();
	// LCD feedback for MIXER mode
	else if (surfaceMode == 3) LcdMixerMode();
}
// All LED feedback happens here
void CSurf_SL::ModeSelectorLed() {

	// LED feedback for USER mode
	if (surfaceMode == 0) {
		slLed.UserPg1(midi_out, slPg);
		slLed.UserPg2(midi_out, slPg);

		slLed.UserR(midi_out, slE, slP, slF);

		slLed.UserA(midi_out, slA);
		slLed.UserB(midi_out, slB);
		slLed.UserC(midi_out, slC);
		slLed.UserD(midi_out, slD);
	}

	// LED feedback for FX mode
	if (surfaceMode == 1) {
		slLed.FxPg1(midi_out, slPg);
		slLed.FxPg2(midi_out, slPg);

		slLed.FxR(midi_out, slE, slP, slF, trPlugs);

		slLed.FxA(midi_out, slA);
		slLed.FxB(midi_out, slB, trPlugs);
		slLed.FxC(midi_out, slC, trPlugs);
		slLed.FxD(midi_out, slD, trPlugs);
	}

	// LED feedback for INST mode			
	if (surfaceMode == 2) {
		slLed.InstPg1(midi_out, slPg);
		slLed.InstPg2(midi_out, slPg);

		slLed.InstR(midi_out, slE);

		slLed.InstA(midi_out, slA);
		slLed.InstB(midi_out, slB);
		slLed.InstC(midi_out, slC);
		slLed.InstD(midi_out, slD);
	}

	// LED feedback for MIXER mode
	if (surfaceMode == 3) {
		slLed.MixerPg1(midi_out, slPg);
		slLed.MixerPg2(midi_out, slPg);

		slLed.MixerR(midi_out, slE, slP, slF, reaPlay, reaPause);

		slLed.MixerA(midi_out, slA);
		slLed.MixerB(midi_out, slB);
		slLed.MixerC(midi_out, slC);
		slLed.MixerD(midi_out, slD);
	}

	// Light surface mode LEDs
	for (int i = 2; i <= 5; i++)
	{
		if (surfaceMode == i - 2)
		{
			if (!slAm[i].led) {
				slAm[i].led = !slAm[i].led;
				midi_out->Send(slGlobal.cc, slAm[i].cc, slAm[i].led, 0);
			}
		}
		else if (slAm[i].led) {
			slAm[i].led = !slAm[i].led;
			midi_out->Send(slGlobal.cc, slAm[i].cc, slAm[i].led, 0);
		}
	}
}
// All commmands are processed here
void CSurf_SL::ModeSelectorCmd() {
	for (int i = 2; i < 6; i++) {

		if (slAm[i].clickHold && surfaceMode != i - 2) {

			slDisplay.FeedbackReset(midi_out, slSysex, top_msg_old, btm_msg_old);

			surfaceModeLast = surfaceMode;
			surfaceMode = i - 2;						// Get 0-3 values for mode selection
		}
		else if (slAm[i].off && surfaceMode != surfaceModeLast) {

			slDisplay.FeedbackReset(midi_out, slSysex, top_msg_old, btm_msg_old);

			surfaceMode = surfaceModeLast;
		}
		else if (slAm[i].click && surfaceMode != i - 2) {

			slDisplay.FeedbackReset(midi_out, slSysex, top_msg_old, btm_msg_old);

			surfaceMode = i - 2;						// Get 0-3 values for mode selection
			surfaceModeLast = surfaceMode;
		}
		else if (slAm[i].dblClick && surfaceMode == i - 2) {
			if (surfaceMode == 0) secondModeUser = false;
			if (surfaceMode == 1) secondModeFx = false;
			if (surfaceMode == 2) secondModeInst = false;
			if (surfaceMode == 3) secondModeMixer = false;
		}
	}

	// Choose mode for pots/faders/encoders/buttons
	// Surface USER mode
	if (surfaceMode == 0) {
		slCmd.UserPg1(slPg);
		slCmd.UserPg2(slPg);

		slCmd.UserE(slE);
		slCmd.UserP(slP);
		slCmd.UserF(slF);

		slCmd.UserA(slA);
		slCmd.UserB(slB);
		slCmd.UserC(slC);
		slCmd.UserD(slD);

		slCmd.UserSD(slSd);
	}
	// Surface FX mode
	else if (surfaceMode == 1) {
		slCmd.FxPg1(slPg);
		slCmd.FxPg2(slPg);
		slCmd.FxE(slE, trPlugs);
		slCmd.FxP(slP, trPlugs);
		slCmd.FxF(slF, trPlugs);
		slCmd.FxA(slA);
		slCmd.FxB(slB, trPlugs);
		slCmd.FxC(slC, trPlugs);
		slCmd.FxD(slD, trPlugs);

		slCmd.FxSD(slSd);
	}
	// Surface INST mode
	else if (surfaceMode == 2) {
		slCmd.InstPg1(slPg);
		slCmd.InstPg2(slPg);

		slCmd.InstE(slE);
		slCmd.InstP(slP);
		slCmd.InstF(slF);

		slCmd.InstA(slA);
		slCmd.InstB(slB);
		slCmd.InstC(slC);
		slCmd.InstD(slD);

		slCmd.InstSD(slSd);
	}
	// Surface MIXER mode
	else if (surfaceMode == 3) {
		slCmd.MixerPg1(slPg);
		slCmd.MixerPg2(slPg);

		slCmd.MixerE(slE);
		slCmd.MixerP(slP);
		slCmd.MixerF(slF);

		slCmd.MixerA(slA);
		slCmd.MixerB(slB);
		slCmd.MixerC(slC);
		slCmd.MixerD(slD);

		slCmd.MixerSD(slSd);
	}
}

void CSurf_SL::LcdUserMode() {
	string top_msg;
	string btm_msg;

	// Checks if there was enough time between LCD updates
	if (slDisplay.UpdateTimer.count() > lcdUpdateTime) {

		if (slGlobal.trackSel) {
			top_msg = format("{:<72}", reaTrack[CSurf_TrackToID(slGlobal.trackSel, false)].name);				// Display first selected
		}
		else {
			top_msg = "No tracks in the Project!";
		}

		top_msg = format("{:^72}", top_msg);  // Fill the line
		slDisplay.Update(midi_out, slSysex, top_msg, btm_msg, top_msg_old, btm_msg_old);
	}
}
void CSurf_SL::LcdFxMode() {
	string top_msg = "";
	string btm_msg = "";

	// Checks if there was enough time between LCD updates
	if (slDisplay.UpdateTimer.count() > lcdUpdateTime) {

		string fxCounter;
		string devName = "";

		int fxid = slGlobal.trackFxSel - 1;
		string fxNumStr = to_string(fxid);

		string page = slGlobal.trackFxPageSel;

		// For project with tracks
		if (slGlobal.trackSel) {
			if (slGlobal.trackFxSel) {
				// Encoders row
				if (SLEncoder::touchGlobal) {
					if (slGlobal.rsCmdE == 1) {
						if (TrackFX_GetCount(slGlobal.trackSel) > 0) {
							for (int i = 0; i < 8; i++) {
								if (TrackFX_GetNumParams(slGlobal.trackSel, i) > i) {
									// Top message
									string fxNameStr = trPlugs[to_string(i)]["Info"].get("NameShort", " ").asString();
									(i == fxid) ? top_msg += format("~{:<8}", fxNameStr) : top_msg += format(" {:<8}", fxNameStr);
									// Bottom message
									char buf[512];
									TrackFX_GetFormattedParamValue(slGlobal.trackSel, i, TrackFX_GetNumParams(slGlobal.trackSel, i) - 1, buf, 512);
									string value;
									(slE[i].touchState) ? value = format("~Wet {:>3}%", buf) : value = format(" Wet {:>3}%", buf);

									btm_msg += format("{:>9}", value);
								}
								else {
									top_msg += format("{:<9}", " ");
									btm_msg += format("{:<9}", " ");
								}
							}
						}
					}

					else {
						for (int i = 0; i < 8; i++) {
							string fxP = to_string(i);
							string ctrlType = "default";
							string ctrlMode = to_string(slGlobal.rsCmdE);

							if (trPlugs[fxNumStr]["Page"][page][ctrlType][ctrlMode][fxP]) {
								// Top message
								string shortName = trPlugs[fxNumStr]["Page"][page][ctrlType][ctrlMode][fxP].get("pNameShort", " ").asString();
								(slE[i].touchState) ? top_msg += format("~{:<8}", shortName) : top_msg += format(" {:<8}", shortName);

								// Bottom message
								// Get FX parm value for selected fx
								char fxParmValueChar[512];
								// Try getting formatted value
								bool form = TrackFX_GetFormattedParamValue(slGlobal.trackSel, fxid, trPlugs[fxNumStr]["Page"][page][ctrlType][ctrlMode][fxP].get("fNum", 0).asInt(), fxParmValueChar, 512);
								// Convert formatted value from char to string
								string fxParmValueStr;
								if (form) fxParmValueStr = format("{}", fxParmValueChar);
								// If failed, try getting normalized value
								else fxParmValueStr = format("{}", TrackFX_GetParamNormalized(slGlobal.trackSel, fxid, trPlugs[fxNumStr]["Page"][page][ctrlType][ctrlMode][fxP].get("fNum", 0).asInt()));
								// If failed, output value in 0-127 range
								if (fxParmValueStr.size() == 0) fxParmValueStr = format("{}", int(TrackFX_GetParamNormalized(slGlobal.trackSel, fxid, trPlugs[fxNumStr]["Page"][page][ctrlType][ctrlMode][fxP].get("fNum", 0).asInt()) * 127));
								slDisplay.ReduceString(&fxParmValueStr, 8);
								btm_msg += format(" {:^8}", fxParmValueStr);
							}
							else {
								top_msg += format("{:<9}", " ");
								btm_msg += format("{:<9}", " ");
							}
						}
					}
				}
				// Pots row
				else if (SLPot::touchGlobal) {
					for (int i = 0; i < 8; i++) {
						string fxP = to_string(i);
						string ctrlType;
						(trPlugs[fxNumStr]["Page"][page]["ctrlP"]) ? ctrlType = "ctrlP" : ctrlType = "default";
						string ctrlMode = to_string(slGlobal.rsCmdP);

						if (trPlugs[fxNumStr]["Page"][page][ctrlType][ctrlMode][fxP]["fNum"]) {

							string shortName = trPlugs[fxNumStr]["Page"][page][ctrlType][ctrlMode][fxP].get("pNameShort", " ").asString();
							(slP[i].touchState) ? top_msg += format("~{:<8}", shortName) : top_msg += format(" {:<8}", shortName);

							// Get FX parm value for selected fx
							char fxParmValueChar[512];
							// Try getting formatted value
							bool form = TrackFX_GetFormattedParamValue(slGlobal.trackSel, fxid, trPlugs[fxNumStr]["Page"][page][ctrlType][ctrlMode][fxP].get("fNum", 0).asInt(), fxParmValueChar, 512);
							// Convert value from char to string
							string fxParmValueStr;
							if (form) fxParmValueStr = format("{}", fxParmValueChar);
							// If failed, try getting normalized value
							else fxParmValueStr = format("{}", TrackFX_GetParamNormalized(slGlobal.trackSel, fxid, trPlugs[fxNumStr]["Page"][page][ctrlType][ctrlMode][fxP].get("fNum", 0).asInt()));
							// If failed, output value in 0-127 range
							if (fxParmValueStr.size() == 0) fxParmValueStr = format("{}", int(TrackFX_GetParamNormalized(slGlobal.trackSel, fxid, trPlugs[fxNumStr]["Page"][page][ctrlType][ctrlMode][fxP].get("fNum", 0).asInt()) * 127));
							slDisplay.ReduceString(&fxParmValueStr, 8);
							btm_msg += format(" {:^8}", fxParmValueStr);
						}
						else {
							top_msg += format("{:<9}", " ");
							btm_msg += format("{:<9}", " ");
						}
					}
				}
				// Faders row
				else if (SLFader::touchGlobal) {
					for (int i = 0; i < 8; i++) {
						string fxP = to_string(i);

						string ctrlType;
						(trPlugs[fxNumStr]["Page"][page]["ctrlF"]) ? ctrlType = "ctrlF" : ctrlType = "default";
						string ctrlNum = to_string(slGlobal.rsCmdF);

						if (trPlugs[fxNumStr]["Page"][page][ctrlType][ctrlNum][fxP]["fNum"]) {

							string shortName = trPlugs[fxNumStr]["Page"][page][ctrlType][ctrlNum][fxP].get("pNameShort", " ").asString();
							(slF[i].touchState) ? top_msg += format("~{:<8}", shortName) : top_msg += format(" {:<8}", shortName);

							// Get FX parm value for selected fx
							char fxParmValueChar[512];
							// Try getting formatted value
							bool form = TrackFX_GetFormattedParamValue(slGlobal.trackSel, fxid, trPlugs[fxNumStr]["Page"][page][ctrlType][ctrlNum][fxP].get("fNum", 0).asInt(), fxParmValueChar, 512);
							// Convert value from char to string
							string fxParmValueStr;
							if (form) fxParmValueStr = format("{}", fxParmValueChar);
							// If failed, try getting normalized value
							else fxParmValueStr = format("{}", TrackFX_GetParamNormalized(slGlobal.trackSel, fxid, trPlugs[fxNumStr]["Page"][page][ctrlType][ctrlNum][fxP].get("fNum", 0).asInt()));
							// If failed, output value in 0-127 range
							if (fxParmValueStr.size() == 0) fxParmValueStr = format("{}", int(TrackFX_GetParamNormalized(slGlobal.trackSel, fxid, trPlugs[fxNumStr]["Page"][page][ctrlType][ctrlNum][fxP].get("fNum", 0).asInt()) * 127));
							slDisplay.ReduceString(&fxParmValueStr, 8);
							btm_msg += format(" {:^8}", fxParmValueStr);
						}
						else {
							top_msg += format("{:<9}", " ");
							btm_msg += format("{:<9}", " ");
						}
					}

				}
				// Buttons B row
				else if (SLGlobal::rsCmdMain == 2) {
					for (int i = 0; i < 8; i++) {
						string fxP = to_string(i);
						string ctrlType = "ctrlB";
						string ctrlMode = to_string(0);

						if (trPlugs[fxNumStr]["Page"][page][ctrlType][ctrlMode][fxP]["fNum"]) {

							string shortName = trPlugs[fxNumStr]["Page"][page][ctrlType][ctrlMode][fxP].get("pNameShort", " ").asString();
							top_msg += format(" {:^8}", shortName);

							// Get FX parm value for selected fx
							char fxParmValueChar[512];
							// Try getting formatted value
							bool form = TrackFX_GetFormattedParamValue(slGlobal.trackSel, fxid, trPlugs[fxNumStr]["Page"][page][ctrlType][ctrlMode][fxP].get("fNum", 0).asInt(), fxParmValueChar, 512);
							// Convert value from char to string
							string fxParmValueStr;
							if (form) fxParmValueStr = format("{}", fxParmValueChar);
							// If failed, try getting normalized value
							else fxParmValueStr = format("{}", TrackFX_GetParamNormalized(slGlobal.trackSel, fxid, trPlugs[fxNumStr]["Page"][page][ctrlType][ctrlMode][fxP].get("fNum", 0).asInt()));
							// If failed, output value in 0-127 range
							if (fxParmValueStr.size() == 0) fxParmValueStr = format("{}", int(TrackFX_GetParamNormalized(slGlobal.trackSel, fxid, trPlugs[fxNumStr]["Page"][page][ctrlType][ctrlMode][fxP].get("fNum", 0).asInt()) * 127));
							slDisplay.ReduceString(&fxParmValueStr, 8);
							btm_msg += format(" {:^8}", fxParmValueStr);
						}
						else {
							top_msg += format("{:<9}", " ");
							btm_msg += format("{:<9}", " ");
						}
					}
				}
				// Buttons C row
				else if (SLGlobal::rsCmdMain == 6 && !SLGlobal::transportLock) {
					for (int i = 0; i < 8; i++) {
						string fxP = to_string(i);
						string ctrlType = "ctrlC";
						string ctrlMode = to_string(0);

						if (trPlugs[fxNumStr]["Page"][page][ctrlType][ctrlMode][fxP]["fNum"]) {

							string shortName = trPlugs[fxNumStr]["Page"][page][ctrlType][ctrlMode][fxP].get("pNameShort", " ").asString();
							top_msg += format(" {:^8}", shortName);

							// Get FX parm value for selected fx
							char fxParmValueChar[512];
							// Try getting formatted value
							bool form = TrackFX_GetFormattedParamValue(slGlobal.trackSel, fxid, trPlugs[fxNumStr]["Page"][page][ctrlType][ctrlMode][fxP].get("fNum", 0).asInt(), fxParmValueChar, 512);
							// Convert value from char to string
							string fxParmValueStr;
							if (form) fxParmValueStr = format("{}", fxParmValueChar);
							// If failed, try getting normalized value
							else fxParmValueStr = format("{}", TrackFX_GetParamNormalized(slGlobal.trackSel, fxid, trPlugs[fxNumStr]["Page"][page][ctrlType][ctrlMode][fxP].get("fNum", 0).asInt()));
							// If failed, output value in 0-127 range
							if (fxParmValueStr.size() == 0) fxParmValueStr = format("{}", int(TrackFX_GetParamNormalized(slGlobal.trackSel, fxid, trPlugs[fxNumStr]["Page"][page][ctrlType][ctrlMode][fxP].get("fNum", 0).asInt()) * 127));
							slDisplay.ReduceString(&fxParmValueStr, 8);
							btm_msg += format(" {:^8}", fxParmValueStr);
						}
						else {
							top_msg += format("{:<9}", " ");
							btm_msg += format("{:<9}", " ");
						}
					}
				}
				// Buttons D row
				else if (SLGlobal::rsCmdMain == 7 && !SLGlobal::transportLock) {
					for (int i = 0; i < 8; i++) {
						string fxP = to_string(i);
						string ctrlType = "ctrlD";
						string ctrlMode = to_string(0);

						if (trPlugs[fxNumStr]["Page"][page][ctrlType][ctrlMode][fxP]["fNum"]) {

							string shortName = trPlugs[fxNumStr]["Page"][page][ctrlType][ctrlMode][fxP].get("pNameShort", " ").asString();
							top_msg += format(" {:^8}", shortName);

							// Get FX parm value for selected fx
							char fxParmValueChar[512];
							// Try getting formatted value
							bool form = TrackFX_GetFormattedParamValue(slGlobal.trackSel, fxid, trPlugs[fxNumStr]["Page"][page][ctrlType][ctrlMode][fxP].get("fNum", 0).asInt(), fxParmValueChar, 512);
							// Convert value from char to string
							string fxParmValueStr;
							if (form) fxParmValueStr = format("{}", fxParmValueChar);
							// If failed, try getting normalized value
							else fxParmValueStr = format("{}", TrackFX_GetParamNormalized(slGlobal.trackSel, fxid, trPlugs[fxNumStr]["Page"][page][ctrlType][ctrlMode][fxP].get("fNum", 0).asInt()));
							// If failed, output value in 0-127 range
							if (fxParmValueStr.size() == 0) fxParmValueStr = format("{}", int(TrackFX_GetParamNormalized(slGlobal.trackSel, fxid, trPlugs[fxNumStr]["Page"][page][ctrlType][ctrlMode][fxP].get("fNum", 0).asInt()) * 127));
							slDisplay.ReduceString(&fxParmValueStr, 8);
							btm_msg += format(" {:^8}", fxParmValueStr);
						}
						else {
							top_msg += format("{:<9}", " ");
							btm_msg += format("{:<9}", " ");
						}
					}
				}
				// Default display
				else {
					if (TrackFX_GetCount(slGlobal.trackSel) > 0) {
						// Top message
						string trNum = format("{}", CSurf_TrackToID(slGlobal.trackSel, false));
						string trNameShort = format("{:^8}:", reaTrack[CSurf_TrackToID(slGlobal.trackSel, false)].nameshort);

						string fxName = trPlugs[fxNumStr]["Info"].get("Name", " ").asString();
						fxCounter = format(" FX {}/{} {}", fxid + 1, TrackFX_GetCount(slGlobal.trackSel), fxName);
						string pageName = trPlugs[fxNumStr]["Page"][slGlobal.trackFxPageSel].get("_PageName", " ").asString();

						string devName = trPlugs[fxNumStr]["Info"].get("Dev", " ").asString();
						int fxNameSz = (int)72 - (12 + trNum.size() + pageName.size());
						top_msg = format("{0}. {1}{2:<{3}.{3}} {4}", trNum, trNameShort, fxCounter, fxNameSz, pageName);

						// Bottom Mmessage
						for (int i = 0; i < 8; i++) {
							if (TrackFX_GetCount(slGlobal.trackSel) > i) {

								string fxNameShort = trPlugs[to_string(i)]["Info"].get("NameShort", " ").asString();
								(i == fxid) ? btm_msg += format("~{:<8}", fxNameShort) : btm_msg += format(" {:<8}", fxNameShort);
							}
							else btm_msg += format("{:<9}", " ");
						}

						top_msg = format("{:<72.72}", top_msg);											// Fill and constrain top line
						btm_msg = format("{:<72.72}", btm_msg);											// Fill and constrain btm line
					}
				}
			}
			else {
				string topLine = format("{}. {}", CSurf_TrackToID(slGlobal.trackSel, false), reaTrack[CSurf_TrackToID(slGlobal.trackSel, false)].name);
				top_msg = format("{:<72.72}", topLine);	// Fill and constrain top line
				btm_msg = format("{:<72.72}", "No FX on track");							// Fill and constrain btm line
			}
		}
		// For empty project
		else {
			top_msg = format("{:^72}", "Empty project!");										// Fill and constrain top line
			btm_msg = format("{:^72}", "Please, add a track");									// Fill and constrain btm line
		}
		slDisplay.Update(midi_out, slSysex, top_msg, btm_msg, top_msg_old, btm_msg_old);															// Update LCD
	}
}
void CSurf_SL::LcdInstMode() {

	string top_msg;
	string btm_msg;

	// Checks if there was enough time between LCD updates
	if (slDisplay.UpdateTimer.count() > lcdUpdateTime) {

		string top_msg_left;
		string top_msg_right;

		string btm_msg_left;
		string btm_msg_right;

		if (SLPot::touchGlobal) {
		}
		else if (SLFader::touchGlobal) {
		}
		else {
			if (SLEncoder::touchGlobal) {

				// LEFT.Top message
				top_msg_left = format(" {:^8.8} {:^8.8} {:^8.8} {:^8.8}", "LeftTrim", "RghtEdge", "Content", "Duplicat");

				// LEFT.Bottom message
				if (slGlobal.rsCmdE == 6) {

				}
				else if (slGlobal.rsCmdE == 7) {
					btm_msg_left = format(" {:^8.8} {:^8.8} {:^8.8} {:^8.8}", "Grid", "Grid", "Grid", "Measure");
				}
				else {
					btm_msg_left = format(" {:^8.8} {:^8.8} {:^8.8} {:^8.8}", "Grid", "Grid", "Grid", "Length");
				}
			}
			else {
				// LEFT.Top message
				string trackCount;
				if (CountSelectedMediaItems(0) == 1) trackCount = "Media Item";
				else if (CountSelectedMediaItems(0) > 1) trackCount = format("Items ({})", CountSelectedMediaItems(0));
				else {
					if (CountSelectedTracks(0) == 1) trackCount = "Track";
					else if (CountSelectedTracks(0) > 1) trackCount = format("Tracks ({})", CountSelectedTracks(0));
					else trackCount = "No objects";
				}
				top_msg_left = format("{}", trackCount);

				// LEFT.Bottom message
				string trackName;
				if (slGlobal.trackSel) {
					char buf[512] = "No track";
					GetTrackName(slGlobal.trackSel, buf, 512);
					trackName = buf;
				}
				else trackName = slDisplay.Nul();
				btm_msg_left = format("{}", trackName);
			}

			// RIGHT.Top message
			string curBpm = format("{:^8.8}", (round((TimeMap2_GetDividedBpmAtTime(0, GetCursorPosition())) * 100)) / 100);
			top_msg_right = format("{:^8.8} {:^8.8} {:^8.8} {:^8.8}", curBpm, "SelStart", "SelEnd", "Grid");

			// RIGHT.Bottom message
			// Get time signature
			double tpos = FindTempoTimeSigMarker(0, GetCursorPosition());
			int measures;
			int cml;
			int cdenom;
			double fullbeats;
			TimeMap2_timeToBeats(0, tpos, &measures, &cml, &fullbeats, &cdenom);
			string timeSigStr = format("{}/{}", cml, cdenom);

			// Get time selection start and end positions
			double startLoop;
			double endLoop;
			GetSet_LoopTimeRange2(0, false, false, &startLoop, &endLoop, false);
			char startLoopChar[512];
			char endLoopChar[512];
			format_timestr_pos(startLoop, startLoopChar, 512, 2);
			format_timestr_pos(endLoop, endLoopChar, 512, 2);
			string startLoopStr = format("{:^8.8}", startLoopChar);
			string endLoopStr = format("{:^8.8}", endLoopChar);

			// Get grid
			double divisionInOut;
			int swingmodeInOut;
			double swingamtInOut;
			string gridStr;
			GetSetProjectGrid(0, false, &divisionInOut, &swingmodeInOut, &swingamtInOut);

			double denom = 1 / divisionInOut;
			if (denom >= 2) gridStr = format("1/{}", denom);
			else if (denom == 0) gridStr = format("{:^8}", 1);
			else gridStr = format("{:^8}", divisionInOut);
			btm_msg_right = format("{:^8.8} {:^8.8} {:^8.8} {:^8.8}", timeSigStr, startLoopStr, endLoopStr, gridStr);
		}

		top_msg = format("{:^36.36}{:^36.36}", top_msg_left, top_msg_right);  // Fill the line
		btm_msg = format("{:^36.36}{:^36.36}", btm_msg_left, btm_msg_right);  // Fill the line

		slDisplay.Update(midi_out, slSysex, top_msg, btm_msg, top_msg_old, btm_msg_old);

		//if (SLGlobal::rsCmdMain == 0) {
		//	if (GetTrackNumSends(slGlobal.trackSel, 0) == 0) {
		//		top_msg = format("{:<72}", reaTrack[CSurf_TrackToID(slGlobal.trackSel, false)].name);
		//		btm_msg = format("{:<72}", "No sends on this track");
		//	}
		//	else {
		//		for (int i = 0; i < 8; i++) {
		//			(i < GetTrackNumSends(slGlobal.trackSel, 0)) ? top_msg += slDisplay.SendName(slGlobal.trackSel, i, 8) : top_msg += format("{:^9.8}", " ");

		//			if (i < GetTrackNumSends(slGlobal.trackSel, 0)) {
		//				if (SLPot::touchGlobal) btm_msg += slDisplay.SendRcvPan(slGlobal.trackSel, i, 0);
		//				if (SLEncoder::touchGlobal) btm_msg += slDisplay.SendRcvVol(slGlobal.trackSel, i, 0);
		//				else btm_msg += slDisplay.SendRcvVol(slGlobal.trackSel, i, 0);
		//			}
		//			else btm_msg += format("{:^9.8}", " ");
		//		}
		//	}
		//}
		//if (SLGlobal::rsCmdMain == 7) {

		//	if (GetTrackNumSends(slGlobal.trackSel, -1) == 0) {
		//		top_msg = format("{:<72}", reaTrack[CSurf_TrackToID(slGlobal.trackSel, false)].name);
		//		btm_msg = format("{:<72}", "No receives on this track");
		//	}
		//	else {
		//		for (int i = 0; i < 8; i++) {
		//			(i < GetTrackNumSends(slGlobal.trackSel, -1)) ? top_msg += slDisplay.RecvName(slGlobal.trackSel, i, 8) : top_msg += format("{:^9.8}", " ");

		//			if (i < GetTrackNumSends(slGlobal.trackSel, -1)) {
		//				if (SLPot::touchGlobal) btm_msg += slDisplay.SendRcvPan(slGlobal.trackSel, i, -1);
		//				else btm_msg += slDisplay.SendRcvVol(slGlobal.trackSel, i, -1);
		//			}
		//			else btm_msg += format("{:^9.8}", " ");
		//		}
		//	}
		//}
	}
}
void CSurf_SL::LcdMixerMode() {

	string top_msg;
	string btm_msg;

	// Checks if there was enough time between LCD updates
	if (slDisplay.UpdateTimer.count() > lcdUpdateTime) {
		// Construct LCD strings for different modes
		for (int i = 0; i < 8; i++) {
			int tid = slGlobal.trackBankStart + i;
			MediaTrack *tr = CSurf_TrackFromID(tid, false);
			// If track exist
			if (tr) {
				//top_msg += format("{:^8} ", reaTrack[tid].nameshort);				// Display track names

				// If pot row touched
				if (SLPot::touchGlobal) {

					top_msg += format("{:^8} ", reaTrack[tid].nameshort);				// Display track names

					if (slGlobal.rsCmdP == 0) {
						(slP[i].touchState) ? btm_msg += format("[{:^7}]", slDisplay.TrackPan(tr)) : btm_msg += format("{:^9}", slDisplay.TrackPan(tr));
					}
					if (slGlobal.rsCmdP == 7) {
						(slP[i].touchState) ? btm_msg += format("[{:^7}]", slDisplay.TrackPanWidth(tr)) : btm_msg += format("{:^9}", slDisplay.TrackPanWidth(tr));
					}
				}
				// If encoder row touched
				else if (SLEncoder::touchGlobal) {
					if (slGlobal.rsCmdE == 0) {

						top_msg += format("{:^8} ", reaTrack[tid].nameshort);				// Display track names

						(slE[i].touchState) ? btm_msg += format("~{:^8}", slDisplay.TrackVol(tr)) : btm_msg += format("{:^9}", slDisplay.TrackVol(tr));
					}
					else {
						for (int j = 0; j < 8; ++j) {
							if (slGlobal.rsCmdE == j + 1) {
								if (GetTrackNumSends(tr, 0) > j) {

									char sendTrackName[512];
									GetTrackSendName(tr, j, sendTrackName, 512);

									top_msg += format(" {:^8}", sendTrackName);				// Display track names

									double vol;
									double pan;
									GetTrackSendUIVolPan(tr, j, &vol, &pan);

									(slE[i].touchState) ? btm_msg += format("~S{} {:^4.2}", j + 1, vol) : btm_msg += format(" S{} {:^4.2}", j + 1, vol);
								}
								else {
									top_msg += format("{:^9}", " ");
									btm_msg += format("{:^9}", " ");
								}
							}
						}
					}
				}
				// If fader row touched
				else if (SLFader::touchGlobal) {

					top_msg += format("{:^8} ", reaTrack[tid].nameshort);

					if (slGlobal.rsCmdF == 0) {
						(slF[i].touchState) ? btm_msg += format("~{:^8}", slDisplay.TrackVol(tr)) : btm_msg += format("{:^9}", slDisplay.TrackVol(tr));
					}
				}
				// Nothing is touched
				else {
					top_msg += format("{:^8} ", reaTrack[tid].nameshort);
					btm_msg += format("{:^9}", slDisplay.BtmStrip(tr));	// Volume level in dB
				}
			}
			// If track doesn't exist
			else {
				top_msg += slDisplay.Nul();							// Empty 9 chars string
				btm_msg += slDisplay.Nul();							// Empty 9 chars string
			}
		}
		slDisplay.Update(midi_out, slSysex, top_msg, btm_msg, top_msg_old, btm_msg_old);
	}
}

// Class constructor
CSurf_SL::CSurf_SL(int offset, int size, int indev, int outdev, int * errStats)
{
	m_offset = offset;
	m_size = size;
	m_midi_in_dev = indev;
	m_midi_out_dev = outdev;

	//create midi hardware access
	midi_in = m_midi_in_dev >= 0 ? CreateMIDIInput(m_midi_in_dev) : NULL;
	midi_out = m_midi_out_dev >= 0 ? CreateThreadedMIDIOutput(CreateMIDIOutput(m_midi_out_dev, false, NULL)) : NULL;

	if (errStats)
	{
		if (m_midi_in_dev >= 0 && !midi_in) *errStats |= 1;
		if (m_midi_out_dev >= 0 && !midi_out) *errStats |= 2;
	}

	if (midi_in)
		midi_in->start();

	slGlobal.automapStatus = false;
}

// Class destructor
CSurf_SL::~CSurf_SL() // override
{
	if (midi_out)
	{
		midi_out->Send(0xBF, slAm[6].cc, 1, 0);	// Send all LEDs off
		Sleep(5);
		slSysex.Send(slSysex.clearBothDisplays, midi_out);		// Send clear both lcds
		Sleep(5);
		//SysexSend(sysex.offline);				// Send Automap in offline
		//Sleep(5);
	}
	delete midi_out;
	delete midi_in;
}

// Actions upon incoming MIDI messages from SL
void CSurf_SL::OnMIDIEvent(MIDI_event_t * evt) {
	// Sysex messages. TLock status and Automap mode.
	slSysex.Parser(evt, midi_out, slLed, slA, slB, slC, slD, slE, slPg, slAm);
	// Stores midi control states
	MidiParser(evt);												

	// Automap mode
	if (slGlobal.automapStatus) {
		//Transport-Lock
		if (evt->midi_message[0] == slGlobal.cc && evt->midi_message[1] == slAm[7].cc) SLGlobal::transportLock = evt->midi_message[2];
		// Drumpad functions (independent from surface mode)
		DrumpadsCmd(evt);
	}
}
// Main loop. Automatically called 30x/sec or so
void CSurf_SL::Run() {
	if (midi_in) {
		// Yet unknown magic
		midi_in->SwapBufs(timeGetTime());
		int l = 0;
		MIDI_eventlist *list = midi_in->GetReadBuf();
		MIDI_event_t *evts;
		while ((evts = list->EnumItems(&l))) OnMIDIEvent(evts);
		if (runStart) slStartup();	// Runs once, on plugin load


		// Internal plugin logic
		banksDefine();			// Keeps track bank/ track FX bank / FX Parameter bank logic
		resizeTracks();

		// Automap mode
		if (slGlobal.automapStatus) {

			CmdParser();
			FxParser();

			// CMD section
			slCmd.Transport(slTr, slC, mLed);		// All TRANSPORT functions (independent from surface mode)
			RowSelectCmd();		// Shift and double press shift functions for transport section
			LearnViewCmd();

			

			// LED section
			slLed.Transport(midi_out, slC, slD, slTr, reaPlay, reaPause, reaStop, reaRecord, reaRepeat, mLed);
			RowSelectLed();
			LearnViewLed();

			ModeSelectorCmd();	// Surface Mode Selector (USER/FX/INST/MIXER)	
			ModeSelectorLed();	// LED mode selector for different modes
			ModeSelectorLcd();	// LCD mode selector for different modes


			/* Don't delete (for testing purposes)
			char str[512];
			sprintf(str, "%i\n", amt);
			ShowConsoleMsg(str);
			*/
			//char str[512];
			//sprintf(str, "%i\n", SLGlobal::rsCmdMain);
			//ShowConsoleMsg(str);
		}
	}
}


// turn track id index into decimal id integer
#define FIXID(id) int id=CSurf_TrackToID(trackid,false); int oid=id; id -= m_offset; {if (id < 0) return;}

// these will be called by the host when states change etc
void CSurf_SL::SetSurfaceVolume(MediaTrack * trackid, double volume) {
	FIXID(id)
			resizeTracks();
			reaTrack[id].volume = volume;
}
void CSurf_SL::SetSurfacePan(MediaTrack * trackid, double pan) {
	FIXID(id)
			resizeTracks();
			reaTrack[id].pan = pan;
}
void CSurf_SL::SetSurfaceMute(MediaTrack * trackid, bool mute) {
	FIXID(id)
			resizeTracks();
			reaTrack[id].mute = mute;
}
void CSurf_SL::SetSurfaceSelected(MediaTrack * trackid, bool selected) {
	FIXID(id)
			resizeTracks();
			reaTrack[id].selected = selected;
}
void CSurf_SL::SetSurfaceSolo(MediaTrack * trackid, bool solo) {
	FIXID(id)
			resizeTracks();
			reaTrack[id].solo = solo;
}
void CSurf_SL::SetSurfaceRecArm(MediaTrack * trackid, bool recarm) {
	FIXID(id)
			resizeTracks();
			reaTrack[id].recarm = recarm;
}

void CSurf_SL::SetPlayState(bool play, bool pause, bool rec) {
	if (midi_out) {
		reaStop = (pause || (!pause && !play));
		reaPlay = (play || (!play && pause));
		reaPause = pause;
		reaRecord = rec;
	}
}
void CSurf_SL::SetRepeatState(bool rep) {
	if (midi_out) {
		reaRepeat = rep;
	}
}

void CSurf_SL::SetTrackTitle(MediaTrack * trackid, const char * title) {
	FIXID(id)
			resizeTracks();
			reaTrack[id].name = regex_replace(title, regex("[^\\w\\s-\\(\\)]+"), "");

			reaTrack[id].nameshort = title;
			slDisplay.ReduceString(&reaTrack[id].nameshort, 8);
			reaTrack[id].nameshort = format("{:^8.8}", reaTrack[id].nameshort);
}

void OnTrackSelection(MediaTrack *trackid) {

}

void CSurf_SL::SetAutoMode(int mode) {
	reaSelTrackAuto = mode;
}



// Unknown magic (have some clues)
static void parseParms(const char *str, int parms[4]) {
	parms[0] = 0;
	parms[1] = 9;
	parms[2] = parms[3] = -1;

	const char *p = str;
	if (p) {
		int x = 0;
		while (x < 4) {
			while (*p == ' ') p++;
			if ((*p < '0' || *p > '9') && *p != '-') break;
			parms[x++] = atoi(p);
			while (*p && *p != ' ') p++;
		}
	}
}
// Unknown magic
static IReaperControlSurface *createFunc(const char *type_string, const char *configString, int *errStats) {
	int parms[4];
	parseParms(configString, parms);

	return new CSurf_SL(parms[0], parms[1], parms[2], parms[3], errStats);
}
// Unknown magic
static WDL_DLGRET dlgProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch (uMsg) {
	case WM_INITDIALOG: {
		int parms[4];
		parseParms((const char *)lParam, parms);

		int n = GetNumMIDIInputs();
		int x = (int)SendDlgItemMessage(hwndDlg, IDC_COMBO2, CB_ADDSTRING, 0, (LPARAM)"None");
		SendDlgItemMessage(hwndDlg, IDC_COMBO2, CB_SETITEMDATA, x, -1);
		x = (int)SendDlgItemMessage(hwndDlg, IDC_COMBO3, CB_ADDSTRING, 0, (LPARAM)"None");
		SendDlgItemMessage(hwndDlg, IDC_COMBO3, CB_SETITEMDATA, x, -1);
		for (x = 0; x < n; x++) {
			char buf[512];
			if (GetMIDIInputName(x, buf, sizeof(buf))) {
				int a = (int)SendDlgItemMessage(hwndDlg, IDC_COMBO2, CB_ADDSTRING, 0, (LPARAM)buf);
				SendDlgItemMessage(hwndDlg, IDC_COMBO2, CB_SETITEMDATA, a, x);
				if (x == parms[2]) SendDlgItemMessage(hwndDlg, IDC_COMBO2, CB_SETCURSEL, a, 0);
			}
		}
		n = GetNumMIDIOutputs();
		for (x = 0; x < n; x++) {
			char buf[512];
			if (GetMIDIOutputName(x, buf, sizeof(buf))) {
				int a = (int)SendDlgItemMessage(hwndDlg, IDC_COMBO3, CB_ADDSTRING, 0, (LPARAM)buf);
				SendDlgItemMessage(hwndDlg, IDC_COMBO3, CB_SETITEMDATA, a, x);
				if (x == parms[3]) SendDlgItemMessage(hwndDlg, IDC_COMBO3, CB_SETCURSEL, a, 0);
			}
		}
		SetDlgItemInt(hwndDlg, IDC_EDIT1, parms[0], TRUE);
		SetDlgItemInt(hwndDlg, IDC_EDIT2, parms[1], FALSE);
	}
						break;
	case WM_USER + 1024:
		if (wParam > 1 && lParam) {
			char tmp[512];

			int indev = -1, outdev = -1, offs = 0, size = 9;
			int r = int(SendDlgItemMessage(hwndDlg, IDC_COMBO2, CB_GETCURSEL, 0, 0));
			if (r != CB_ERR) indev = int(SendDlgItemMessage(hwndDlg, IDC_COMBO2, CB_GETITEMDATA, r, 0));
			r = int(SendDlgItemMessage(hwndDlg, IDC_COMBO3, CB_GETCURSEL, 0, 0));
			if (r != CB_ERR)  outdev = (int)SendDlgItemMessage(hwndDlg, IDC_COMBO3, CB_GETITEMDATA, r, 0);

			BOOL t;
			r = GetDlgItemInt(hwndDlg, IDC_EDIT1, &t, TRUE);
			if (t) offs = r;
			r = GetDlgItemInt(hwndDlg, IDC_EDIT2, &t, FALSE);
			if (t) {
				if (r < 1)r = 1;
				else if (r > 256)r = 256;
				size = r;
			}
			sprintf_s(tmp, "%d %d %d %d", offs, size, indev, outdev);
			lstrcpyn((char *)lParam, tmp, int(wParam));
		}
		break;
	}
	return 0;
}
// Unknown magic
static HWND configFunc(const char *type_string, HWND parent, const char *initConfigString) {
	return CreateDialogParam(g_hInst, MAKEINTRESOURCE(IDD_SURFACEEDIT_MCU), parent, dlgProc, (LPARAM)initConfigString);
}
// Register this .cpp file in csurf_main.cpp
reaper_csurf_reg_t csurf_sl_reg = { "NovationSL", "Novation SL MkII (fundorin)", createFunc, configFunc, };
