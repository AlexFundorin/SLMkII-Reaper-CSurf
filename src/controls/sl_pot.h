#pragma once

#ifndef SLPOT_H
#define SLPOT_H

class SLPot  {
public:
	SLPot(int a) : cc(a) {}

	void Parse() {
		if (state != lastState) {
			move = state;
			lastState = state;
		}
		else move = -1;
	}


	const int cc;
	int state = -1;
	int lastState = -1;
	int move = -1;
	int prev = -1;
	inline static const int touch = 109;
	bool touchState = false;
	static bool touchGlobal;
};

#endif // !SLPOT_H