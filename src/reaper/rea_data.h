#pragma once

#ifndef READATA_H
#define READATA_H

#include <string>

using namespace std;


// Maximum allowed tracks in the project
static const int maxTracks = 1024;


// Store data from Reaper in arrays

// Transport states
bool reaStop;
bool reaPause;
bool reaPlay;
bool reaRecord;
bool reaRepeat;

// Selected track parameters
int reaSelTrackAuto;
string reaSelTrackName;
double reaSelTrackVol;
string reaSelTrackVolStr;
double reaSelTrackPan;
string reaSelTrackPanStr;

int reaSelTrackSolo;
int reaSelTrackMute;
int reaSelTrackRecArm;
int reaSelTrackRecMon;

#endif // !READATA_H