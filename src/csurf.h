
#ifndef _CSURF_H_
#define _CSURF_H_

#include "reaper_plugin_functions.h"
#include "resource.h"
#include <../lib/WDL/db2val.h>
#include <../lib/WDL/wdlstring.h>
#include <stdio.h>
#include <JuceLibraryCode/JuceHeader.h>

// Juce GUI window states
enum toggle_state { CannotToggle, ToggleOff, ToggleOn };


extern REAPER_PLUGIN_HINSTANCE g_hInst; // used for dialogs
extern HWND g_hwnd;

/*
** Calls back to REAPER (all validated on load)
*/
// Added by fundorin (SWS extra functions)

extern double(*BR_GetPrevGridDivision)(double position);
extern double(*BR_GetNextGridDivision)(double position);

// Added by fundorin (JUCE test functions)
void testUserInputs();


/*
** REAPER command message defines
*/

#define IDC_REPEAT                      1068
#define ID_FILE_SAVEAS                  40022
#define ID_FILE_NEWPROJECT              40023
#define ID_FILE_OPENPROJECT             40025
#define ID_FILE_SAVEPROJECT             40026
#define IDC_EDIT_UNDO                   40029
#define IDC_EDIT_REDO                   40030
#define ID_MARKER_PREV                  40172
#define ID_MARKER_NEXT                  40173
#define ID_INSERT_MARKERRGN             40174
#define ID_INSERT_MARKER                40157
#define ID_LOOP_SETSTART                40222
#define ID_LOOP_SETEND                  40223
#define ID_METRONOME                    40364
#define ID_GOTO_MARKER1                 40161
#define ID_SET_MARKER1                  40657

// Reaper track automation modes
enum AutoMode {
	AUTO_MODE_TRIM,
	AUTO_MODE_READ,
	AUTO_MODE_TOUCH,
	AUTO_MODE_WRITE,
	AUTO_MODE_LATCH,
};

midi_Output *CreateThreadedMIDIOutput(midi_Output *output); // returns null on null

#endif