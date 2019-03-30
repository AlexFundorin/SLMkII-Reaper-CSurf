#pragma once

#ifndef SLSPEEDDIAL_H
#define SLSPEEDDIAL_H

class SLSpeedDial {
public:

	// MIDI CC
	const int cc = 102;
	const int btnCc = 101;
	const int touchCc = 111;

	// Rotate
	bool move = false;	// if any
	int left;			// left turn
	int right;			// right turn

	// Touch
	bool touchState = false;	// Touched - 64. Not touched - 0

	// Button
	bool on = false;
	bool off = false;
	bool hold = false;
	bool click = false;
	bool clickHold = false;
	bool dblClick = false;
	bool dblClickHold = false;

	void SetRotate(int msg3) {
		state = msg3;
		move = true;
	}

	void SetButton(int msg3) {
		push = msg3;
	}

	void Parse() {
		// Rotate processing
		if (move) {
			(state > 64) ? left = state - 64 : right = state;
			move = false;
			state = 0;
		}
		else {
			left = 0;
			right = 0;
		}

		// Button processing
		if (push != pushState) {
			if (push) on = true;
			if (!push) off = true;
			pushState = push;
		}
		else {
			on = false;
			off = false;
			(push) ? hold = true : hold = false;
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
			//char str[512];
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

private:



	//Rotate varuibles
	int state = 0;					// CW - 1++, CCW - 65++
	int lastState;

	//Button varuibles
	cxxtimer::Timer timer;

	bool push = false;			// 1-0
	bool pushState = false;		// 1-0

	int stage = 0;

	int dblThreshold = 1000;

	int clickTime = 200;
	int dblClickTime = 300;
	int lastTime = 600;
};

#endif // !SLSPEEDDIAL_H