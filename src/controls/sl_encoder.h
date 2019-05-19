#pragma once
	
#ifndef SLENCODER_H
#define SLENCODER_H

class SLEncoder {
public:
	SLEncoder(int a, int b) : cc(a), ring(b) {}

	const int cc;
	const int ring;
	inline static const int touch = 108;

	int state = -1;
	int lastState = -1;

	int left = 0;
	int right = 0;
	bool move = false;

	int prev = -1;
	int mode = 0;
	int led = 0;
	bool touchState = false;

	static bool touchGlobal;

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
	}
};

#endif // !SLENCODER_H