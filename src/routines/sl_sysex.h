#pragma once

#ifndef SLSYSEX_H
#define SLSYSEX_H

#include "../controls/sl_button.h"
#include "../controls/sl_encoder.h"
#include "../routines/sl_global.h"
#include "../routines/sl_led.h"

#include <string>

using namespace std;


// Define SYSEX messages

class SLSysex
{
public:

	SLGlobal slGlobal;

	const unsigned char hello[27] = { 0xF0, 0x00, 0x20, 0x29, 0x03, 0x03, 0x12, 0x00, 0x02, 0x00, 0x02, 0x02, 0x01, 0x01, 0x01, 0x03, 0x04, 0x66, 0x75, 0x6E, 0x64, 0x6F, 0x72, 0x69, 0x6E, 0x00, 0xF7 }; // "fundorin" delete when test is over

	string fundorin = "fundorin";


	string online = "\xF0\x00\x20\x29\x03\x03\x12\x00\x02\x00\x01\x01\xF7"s;
	string offline = "\xF0\x00\x20\x29\x03\x03\x12\x00\x02\x00\x01\x00\xF7"s;

	string clearBothDisplays = "\xF0\x00\x20\x29\x03\x03\x12\x00\x02\x00\x02\x02\x01\xF7"s;
	string header = "\xF0\x00\x20\x29\x03\x03\x12\x00\x02\x00\x02"s;
	string lcdRequest = "\xF0\x00\x20\x29\x03\x05\x10\x05\x00\x00\x66\x04\x00\x00\x20\xF7"s;

	string simLeftPress = "\xF0\x00\x20\x29\x03\x05\x10\x05\x00\x00\x66\x01\x01\01\xF7"s; // simulate Btn1 press
	string simRightPress = "\xF0\x00\x20\x29\x03\x05\x10\x05\x00\x00\x66\x01\x01\11\xF7"s; // simulate Btn17 press


	//simLeftPress = "\xF0\x00\x20\x29\x03\x05\x12\x00\x00\x00\x66\x02\x01\x01\xF7"; // Switch LED/LCD to the left Side
	//simrightPress = "\xF0\x00\x20\x29\x03\x05\x12\x00\x00\x00\x66\x02\x09\x01\xF7"; // Switch LED/LCD to the right Side

	string textEnd = "\x00"s;
	string cursor = "\x01"s;
	string clearText = "\x02"s;
	string textStart = "\x04"s;
	string end = "\xF7"s;

	string clearBoth = "0x01"s;
	string clearTop = "0x02"s;
	string clearBottom = "0x03"s;
	string clearLeft = "0x04"s;
	string clearRight = "0x05"s;
	string clearLeftTop = "0x06"s;
	string clearLeftBottom = "0x07"s;
	string clearRightTop = "0x08"s;
	string clearRightBottom = "0x09"s;
	string clearToLen = "0x10"s; // 02 len 10 (lcd cmd, from cur pos to len, end cmd)

	void Send(string &str, midi_Output* out) {
		if (out) {
			MIDI_event_t msg;

			msg.frame_offset = 0;
			msg.size = int(str.size());
			memcpy(msg.midi_message, &str[0], msg.size);
			out->SendMsg(&msg, 0);
		}
	}

	void SendString(string &str, int pos, int line, midi_Output* out) {

		if (out) {
			MIDI_event_t foo;
			string sendStr;

			sendStr += header;	// sysex header
			sendStr += cursor;	// set cursor
			sendStr.push_back(pos);		// position
			sendStr.push_back(line);	// line
			sendStr += textStart;	// text start
			sendStr += str;				// text
			sendStr += textEnd;	// text end
			sendStr += end;		// sysex end

			foo.frame_offset = 0;
			foo.size = (int)sendStr.size();
			memcpy(foo.midi_message, &sendStr[0], foo.size);

			out->SendMsg(&foo, 0);
		}
	}

	void Parser(MIDI_event_t * evt, midi_Output* out, SLLed &slLed, vector<SLButton> &A, vector<SLButton> &B, vector<SLButton> &C, vector<SLButton> &D, vector<SLEncoder> &E, vector<SLButton> &Pg, vector<SLButton> &Am) {

		unsigned char msg[1024];
		memcpy(msg, evt->midi_message, sizeof(evt->midi_message)); // Full midi message. 512 bytes, as declared in reaper_plugin.h, line 202

		if (evt->midi_message[0] == 0xF0) {

			unsigned char online[] = { 0xF0, 0x00, 0x20, 0x29, 0x03, 0x03, 0x12, 0x00, 0x02, 0x00, 0x01, 0x01, 0xF7 };
			unsigned char offline[] = { 0xF0, 0x00, 0x20, 0x29, 0x03, 0x03, 0x12, 0x00, 0x02, 0x00, 0x01, 0x00, 0xF7 };

			// Compare arrays
			if (!memcmp(msg, online, sizeof(online))) {
				slGlobal.automapStatus = true;
				slLed.FeedbackReset(out, A, B, C, D, E, Pg, Am);
			}
			else if (!memcmp(msg, offline, sizeof(offline))) {
				slGlobal.automapStatus = false;
				Send(clearBothDisplays, out);			// Send clear both lcds
				out->Send(slGlobal.cc, Am[6].cc, 1, -1);	// Turn all LEDs off
			}
		}
	}
};

#endif // !SLSYSEX_H