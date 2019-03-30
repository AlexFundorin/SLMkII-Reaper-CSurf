#pragma once

#ifndef SLCONVERT_H
#define SLCONVERT_H
#include "..\csurf.h"

// Min max pairs comparison
template <class T> T __maxVal(const T& left, const T& right)
{
	return left > right ? left : right;
}
template <class T> T __minVal(const T& left, const T& right)
{
	return left < right ? left : right;
}

// Convert decimal integer (0-127) into volume value
static double charToVol(unsigned char val)
{
	double pos = ((double)val*1000.0) / 127.0;
	pos = SLIDER2DB(pos);
	return DB2VAL(pos);

}

// Convert  volume value into decimal integer (0-127)
static  unsigned char volToChar(double vol)
{
	double d = (DB2SLIDER(VAL2DB(vol))*127.0 / 1000.0);
	if (d < 0.0)d = 0.0;
	else if (d > 127.0)d = 127.0;

	return (unsigned char)(d + 0.5);
}

//Convert decimal integer (0-127) into panorama value
//static double charToPan(int val)
//{
//
//	//y = (a * (x - h)) ^ 3 + k
//
//	double pos = (double(val)*1000.0 + 0.5) / 127.0;
//
//	pos = (pos - 500.0) / 500.0;
//	pos = round(pos * 1000) / 1000.0;
//	//if (fabs(pos) < 0.08) pos = 0.0;
//
//	return pos;
//}

static double charToPan(int val)
{
	double magnet = 1.0 / 256063;
	double midFirst = double(127) / 2;
	double midSecond = 0;
	double pos;

	pos = magnet * pow(double(val) - midFirst, 3) + midSecond;
	pos = cbrt(pos);
	pos = round(pos * 100) / 100.0;

	if (fabs(pos) < 0.03) pos = 0.0;

	return pos;
}


//static double charToPan(int val)
//{
//	double magnet = double(127) / 3000000;
//	double midFirst = double(127) / 2;
//	double midSecond = 0;
//	double pos;
//
//	 pos = magnet * pow(double(val) - midFirst, 3) + midSecond;
//	 pos = cbrt(pos);
//	 pos = (pos - (-2.213101)) / (2.213101 - (-2.213101)) * (1 - (-1)) + (-1);
//	 pos = round(pos * 1000) / 1000.0;
//
//	//if (fabs(pos) < 0.08) pos = 0.0;
//
//	return pos;
//}

static unsigned char panToChar(double pan)
{
	pan = (pan + 1.0)*63.5;

	if (pan < 0.0)pan = 0.0;
	else if (pan > 127.0)pan = 127.0;

	return (int)round(pan);
}


//auto re = vector<regex>{ regex("[^\\w\\s]+"), regex("_"), regex("[\\s]"), regex("[yuioae]"), regex("[zjqxkvbpgwfmcldhrsnt]"), regex("[YUIOAE]"), regex("[ZJQXKVBPGWFMCLDHRSNT]"), regex("[0987654321]") };
//regex operator""_r(const char* r, size_t ct) { return regex(r, ct); }
//auto re = vector<regex>{ "[^\\w\\s]+"_r, "_"_r, "[\\s]"_r, "[yuioae]"_r, "[zjqxkvbpgwfmcldhrsnt]"_r, "[YUIOAE]"_r, "[ZJQXKVBPGWFMCLDHRSNT]"_r, "[0987654321]"_r };
//
//void eraseIfMatch(string *str, regex re, bool rightToLeft) {
//	if (rightToLeft) {
//		for (int pos = (*str).size(); (*str).size()>8 && pos > -1; --pos) {
//			if (regex_match((*str).substr(pos, 1), re)) (*str).erase(pos, 1);
//		}
//	}
//	else {
//		for (int pos = 0; (*str).size()>8 && pos < (*str).size(); ++pos) {
//			if (regex_match((*str).substr(pos, 1), re)) (*str).erase(pos, 1);
//		}
//	}
//}

#endif