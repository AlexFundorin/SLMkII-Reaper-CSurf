#pragma once

#ifndef SLFADER_H
#define SLFADER_H

class SLFader {
public:
	SLFader(int a) : cc(a) {}

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
	inline static const int touch = 110;
	bool touchState = false;
	static bool touchGlobal;
};

#endif // !SLFADER_H