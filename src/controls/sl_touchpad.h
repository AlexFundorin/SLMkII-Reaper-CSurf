#pragma once

#ifndef SLTOUCHPAD_H
#define SLTOUCHPAD_H

class SLTouchpad
{
public:
	SLTouchpad(int a, int b);
	~SLTouchpad();

	const int ccX;
	const int ccY;
	int stateX = 0;
	int stateY = 0;
};

#endif // !SLTOUCHPAD_H