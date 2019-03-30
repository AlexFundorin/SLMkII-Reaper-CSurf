#pragma once

#ifndef REATRACK_H
#define REATRACK_H

#include <string>

using namespace std;

class ReaTrack {
public:
	ReaTrack();
	~ReaTrack();

	double volume;
	double pan;
	bool mute;
	bool selected;
	bool solo;
	bool recarm;

	string name;
	string nameshort;

	int automode;
	
};

#endif // !REATRACK_H