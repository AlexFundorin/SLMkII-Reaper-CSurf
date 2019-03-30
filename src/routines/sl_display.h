#pragma once

#ifndef SLDISPLAY_H
#define SLDISPLAY_H

#include <regex>

// Third-party libraries
#include <fmt/format.h>
#include <json/json/json.h>
#include <timer/cxxtimer.hpp>

#include "../routines/sl_global.h"
#include "../routines/sl_convert.h"
#include "../routines/sl_sysex.h"

using namespace std;
using namespace fmt;
using namespace Json;

class SLDisplay
{
public:

	// Create LCD update timer
	cxxtimer::Timer UpdateTimer;
	cxxtimer::Timer ButtonTimer;

	void ReduceString(string *str, int sz);

	void FeedbackReset(midi_Output * out, SLSysex & slSysex, string &top_msg_old, string &btm_msg_old);

	void Update(midi_Output * out, SLSysex & slSysex, string & top_msg, string & btm_msg, string & top_msg_old, string & btm_msg_old);

	void FxNameFormat(string & type, string & name, string & dev);

	string TrackPan(MediaTrack * tr);

	string TrackPanWidth(MediaTrack * tr);

	string TrackVol(MediaTrack * tr);

	string SendName(MediaTrack * tr, int num, int sz);

	string RecvName(MediaTrack * tr, int num, int sz);

	string SendRcvPan(MediaTrack * tr, int num, int send);

	string SendRcvVol(MediaTrack * tr, int num, int send);

	string BtmStrip(MediaTrack * tr);

	string Nul();

};

#endif // !SLDISPLAY_H

