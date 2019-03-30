#pragma once

#ifndef SLBUTTON_H
#define SLBUTTON_H

#include "../csurf.h"

#include <timer/cxxtimer.hpp>

using namespace std;

class SLButton {
public:

	SLButton(int a);
	~SLButton();

	const int cc;
	bool led = false;
	

	bool on = false;
	bool off = false;
	bool hold = false;
	bool click = false;
	bool clickHold = false;
	bool dblClick = false;
	bool dblClickHold = false;

private: 
	cxxtimer::Timer timer;

	bool state = false;
	bool lastState = false;

	int dblThreshold = 1000;

	int clickTime = 200;
	int dblClickTime = 300;
	int lastTime = 600;

	int stage = 0;

public:

	void SetState(int msg3) {
		state = msg3;
	}

	// Process button states
	void Parse() {

		// Set on/off/hold variables
		if (state == lastState) {
			if (on) on = false;
			if (off) off = false;
			(state) ? hold = true : hold = false;
		}
		else {
			if (state) on = true;
			if (!state) off = true;
			lastState = state;
		}

		// Set click, clickHold, dblClick, dblClickHold variables 
		switch (stage) {
		case 0:
			if (on) {
				timer.reset();
				timer.start();
				stage = 1;
			}
			break;
		case 1:
			if (timer.count() < clickTime) {
				if (off) {
					stage = 2;
				}
			}
			else {
				clickHold = true;
				stage = 5;
			}
			break;
		case 2:
			if (timer.count() < dblClickTime) {
				if (on) {
					stage = 3;
				}
			}
			else {
				click = true;
				stage = 5;
			}
			break;
		case 3:
			if (timer.count() < lastTime) {
				if (off) {
					dblClick = true;
					stage = 5;
				}
			}
			else {
				if (!off) {
					dblClickHold = true;
					stage = 5;
				}
			}
			break;
		case 5:
			//sprintf(str, "click:%i\tclickHold:%i\tdblClick:%i\tdblClickHold%i\ttimer:%i\n", click, clickHold, dblClick, dblClickHold,(int)timer.count());
			//ShowConsoleMsg(str);

			click = false;
			clickHold = false;
			dblClick = false;
			dblClickHold = false;

			timer.reset();

			stage = 0;
			break;
		}
	}
};

#endif // !SLBUTTON_H