#pragma once

#ifndef SLDRUMPAD_H
#define SLDRUMPAD_H

class SLDrumpad {
public:
	SLDrumpad(int a);
	~SLDrumpad();

	const int cc;
	int state = 0;
};

#endif // !SLDRUMPAD_H