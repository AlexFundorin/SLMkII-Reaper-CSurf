#include "sl_display.h"

static regex operator""_r(const char* r, size_t ct) { return regex(r, ct); }

// Perform series of text reductions at string
void SLDisplay::ReduceString(string *str, int sz) {

	auto re = vector<regex>{ "[^\\w\\s]+"_r, "_"_r, "[\\s]"_r, "[yuioae]"_r, "[zjqxkvbpgwfmcldhrsnt]"_r, "[YUIOAE]"_r, "[ZJQXKVBPGWFMCLDHRSNT]"_r, "[0987654321]"_r };

	for (int i = 0; i < 7; i++) {
		for (int pos = (int)(*str).size(); (*str).size() > sz && pos > -1; --pos) {
			if (regex_match((*str).substr(pos, 1), re[i])) (*str).erase(pos, 1);
		}
	}
	for (int pos = 0; (*str).size() > sz && pos < (*str).size(); ++pos) {
		if (regex_match((*str).substr(pos, 1), re[7])) (*str).erase(pos, 1);
	}
}

void SLDisplay::FeedbackReset(midi_Output* out, SLSysex &slSysex, string &top_msg_old, string &btm_msg_old) { // Clear display
	slSysex.Send(slSysex.clearBothDisplays, out);				// Clear LCD
	top_msg_old = "";
	btm_msg_old = "";
}

void SLDisplay::Update(midi_Output* out, SLSysex &slSysex, string &top_msg, string &btm_msg, string &top_msg_old, string &btm_msg_old) {
	// Send LCD strings only if they've changed
	if (top_msg != top_msg_old || btm_msg != btm_msg_old) {
		if (top_msg != top_msg_old) {
			top_msg_old = top_msg;								// Save new top msg
			string top_msg_both = top_msg + top_msg;			// Glue two top msgs together
			slSysex.SendString(top_msg_both, 0, 1, out);				// Update top lines
		}
		if (btm_msg != btm_msg_old) {
			btm_msg_old = btm_msg;								// Save new btm msg
			string btm_msg_both = btm_msg + btm_msg;			// Glue two btm msgs together
			slSysex.SendString(btm_msg_both, 0, 3, out);				// Update bottom lines
		}
		UpdateTimer.reset();									// Reset and stop timer
		UpdateTimer.start();									// Start timer
	}
}

void SLDisplay::FxNameFormat(string &type, string &name, string &dev) {
	// Getting rid of FX type, user tags, (x86), number of outs
	regex re(
		"^([\\w]+:[\\s]+)?\\s*?"											// type (optional for "Video processor" plugin only)
		"([\\w\\s-\\/\\.\\,\\'\\(*\\)*\\+]+?)"								// [1] name
		"\\s*?(?:\\|+.*?)?"													// tags (optional)
		"\\s*?(?:\\(x86\\))?"												// platform (optional)
		"\\s*?(\\(+?[^\\(^\\)]+?\\)+?)?"									// [2] dev (optional for "Video processor" plugin only)
		"\\s*?(?:\\([^\\(^\\)]+ch\\)|\\([^\\(^\\)]+out\\)|\\([mono]+\\))?"	// out (optional)
		"[.*]?$"
	);
	cmatch result;
	regex_search(name.c_str(), result, re);
	type = result.str(1);
	dev = result.str(3);
	name = result.str(2);
}

string SLDisplay::TrackPan(MediaTrack *tr) {
	int pan = int(GetMediaTrackInfo_Value(tr, "D_PAN") * 100);
	string str;

	if (pan < 0) str = format("{:d}%L", pan * -1);
	else if (pan > 0) str = format("{:d}%R", pan);
	else str = "center";

	//str = format("{0:^9}", str);
	return str;
}

string SLDisplay::TrackPanWidth(MediaTrack *tr) {
	int pan = int(GetMediaTrackInfo_Value(tr, "D_WIDTH") * 100);
	string str = format("Wd: {:d}%", pan);

	//str = format("{0:^9}", str);
	return str;
}

string SLDisplay::TrackVol(MediaTrack *tr) {
	string str;

	str = format("{0:.2f}", VAL2DB(GetMediaTrackInfo_Value(tr, "D_VOL")));

	smatch result;
	regex_search(str, result, "([-]?)([0-9]*)([\\.]?)([0-9]*)"_r);

	string unit;
	if (GetMediaTrackInfo_Value(tr, "I_SOLO") != 0 && GetMediaTrackInfo_Value(tr, "B_MUTE") != 0) unit = format("{:.2}", "MS");
	else if (GetMediaTrackInfo_Value(tr, "I_SOLO") != 0) unit = format("{:.2}", "Sl");
	else if (GetMediaTrackInfo_Value(tr, "B_MUTE") != 0) unit = format("{:.2}", "Mt");
	else unit = format("{:.2}", "dB");

	if (regex_match(str, "-150.00"_r)) { // replace lowest possible value - 150 dB with "-inf"
		str = format("-inf {}", unit);
	}
	else {
		int pres = int(3 - result.str(2).size());
		str = format("{0}{1}{2:.{4}}{3:.{4}} {5}", result.str(1), result.str(2), result.str(3), result.str(4), pres, unit);
	}

	//str = format("{:^9}", str); // use 9 chars space

	return str;
}

string SLDisplay::SendName(MediaTrack *tr, int num, int sz) {
	string str;

	char sendName[512];
	GetTrackSendName(tr, num, sendName, 512);		// C-string name

	string sendShort = format("{}", sendName);	// std string name
	ReduceString(&sendShort, sz);				// short name
	str = format("{0:<{1}}", sendShort, sz);
	return str;
}

string SLDisplay::RecvName(MediaTrack *tr, int num, int sz) {
	string str;

	char recvName[512];
	GetTrackReceiveName(tr, num, recvName, 512);		// C-string name

	string sendShort = format("{}", recvName);	// std string name
	ReduceString(&sendShort, sz);				// short name
	str = format("{0:<{1}}", sendShort, sz);

	return str;
}

string SLDisplay::SendRcvPan(MediaTrack *tr, int num, int send) {
	string str;
	int pan = int(GetTrackSendInfo_Value(tr, 0, send, "D_PAN") * 100);

	if (pan < 0) str = format("{:d}%L", pan * -1);
	else if (pan > 0) str = format("{:d}%R", pan);
	else str = "center";

	str = format("{0:^9}", str);

	return str;
}
string SLDisplay::SendRcvVol(MediaTrack *tr, int num, int send) {
	string str;

	str = format("{0:.2f}", VAL2DB(GetTrackSendInfo_Value(tr, 0, send, "D_VOL")));

	smatch result;
	regex_search(str, result, "([-]?)([0-9]*)([\\.]?)([0-9]*)"_r);

	string unit;
	(GetTrackSendInfo_Value(tr, send, num, "B_MUTE")) ? unit = format("{:.2}", "Mt") : unit = format("{:.2}", "dB");

	if (regex_match(str, "-150.00"_r)) { // replace lowest possible value - 150 dB with "-inf"
		str = format("-inf {}", unit);
	}
	else {
		int pres = int(3 - result.str(2).size());
		str = format("{0}{1}{2:.{4}}{3:.{4}} {5}", result.str(1), result.str(2), result.str(3), result.str(4), pres, unit);
	}

	str = format("{:^9}", str); // use 9 chars space
	return str;
}

string SLDisplay::BtmStrip(MediaTrack *tr) {
	string str;

	// Selected (0, 1)
	//(GetMediaTrackInfo_Value(tr, "I_SELECTED") > 0) ? str += ">" : str += " ";

	// 1. Folder, last in folder (0, 1, <0)
	if (GetMediaTrackInfo_Value(tr, "I_FOLDERDEPTH") > 0) str += "F";
	if (GetMediaTrackInfo_Value(tr, "I_FOLDERDEPTH") < 0) str += "l";
	if (GetMediaTrackInfo_Value(tr, "I_FOLDERDEPTH") == 0) str += "-";

	// 2. Mute (0, 1)
	(GetMediaTrackInfo_Value(tr, "B_MUTE") > 0) ? str += "M" : str += "-";

	// 3. Solo (0, >0)
	(GetMediaTrackInfo_Value(tr, "I_SOLO") > 0) ? str += "S" : str += "-";

	// 4. Record Monitor (0, 1, 2) OFF, ON, AUTO
	if (GetMediaTrackInfo_Value(tr, "I_RECMON") == 0) str += "O";
	if (GetMediaTrackInfo_Value(tr, "I_RECMON") == 1) str += "-";
	if (GetMediaTrackInfo_Value(tr, "I_RECMON") == 2) str += "A";

	// 5. FX chain active (0, >0) Bypass FX
	(GetMediaTrackInfo_Value(tr, "I_FXEN") > 0) ? str += "-" : str += "B";

	// 6. Phase inverted (0, 1)
	(GetMediaTrackInfo_Value(tr, "B_PHASE") > 0) ? str += "P" : str += "-";

	// 7. Automation mode (0=trim/off, 1=Read, 2=Touch, 3=Write, 4=Latch, 5-latch Preview) -RTWLP
	
	if (GetMediaTrackInfo_Value(tr, "I_AUTOMODE") == 1) str += "R";
	else if (GetMediaTrackInfo_Value(tr, "I_AUTOMODE") == 2) str += "T";
	else if (GetMediaTrackInfo_Value(tr, "I_AUTOMODE") == 3) str += "W";
	else if (GetMediaTrackInfo_Value(tr, "I_AUTOMODE") == 4) str += "L";
	else if (GetMediaTrackInfo_Value(tr, "I_AUTOMODE") == 5) str += "P";
	else str += "-";

	// 8. Record Arm (0, 1)
	(GetMediaTrackInfo_Value(tr, "I_RECARM") > 0) ? str += "R" : str += "-";

	str += " ";

	return str;
}
string SLDisplay::Nul() {
	string str;
	str = format("{:^9}", " ");
	return str;
}