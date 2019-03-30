# SLMkII-Reaper-CSurf
Control Reaper DAW with Novation SL MkII controller on Windows.
This project is provided "as is".

1. Add DontHideAutomapPorts.reg (from 'extra' folder) contents to your registry and reboot your computer.
2. Disable or uninstall Automap sofware.
3. Copy reaper_csurf_sl_x64.dll to your Reaper's UserPlugins folder.
4. Open Reaper. /Preferences/ Control/OSC/web / add.
5. Select 'Novation SL MkII (fundorin)'.
6. Set MIDI input to 'MIDIIN3 (SL MkII)'.
7. Set MIDI output to 'MIDIOUT3 (SL MkII)'.

You can now control Reaper.
a) Mixer layout. Control tracks (volume/pan/rec arm/mute/solo/etc).
b) Inst. Not completed.
c) FX layout. Control VST plugins settings.
d) User layout. Not completed. Speed Dial works as mouse left click and scroll wheel.

Use left and right row select buttons in combination with the main controls to get extra functions.

Let call the controls here:
Pg - buttons to the left and right of the LCD.
E - encoders.
P - pots.
F - faders.
ABCD - buttons. A means 1st row of the buttons (above encoders). B means 2nd row (under encoders). C are below faders. D row is below C. 
Tr - transport buttons. Use C and D with active 'Transport' button (above Speed Dial encoder) to control Reaper's transport. (Play button is Tr4, for example).
When transport is active:
C1 - undo (hold to save project). C2 - redo. C3 - stop (don't save recorded). C4 - don't remember. C5-C7 - change layouts. C8 - toggle metronome.
D1 - scroll to project start. D2 - scroll to project end. D3 -stop (save recorded). D4 - play. D5 - loop/repeat. D6 - record. D7-D8 are for metronome visualization.
Dr - drumpads.
Rs - row select buttons. R1-5 are at the left side of the controller. R6-8 on the right.
Sd - Speed Dial.

For example, in Mixer layout, Speed Dial controls edit cursor. Pushing and rotating Sd will make time selection.
But, if you'd hold Rs8 button (bottom right row), rotating and pushing Sd will change play rate between 0.5 and 1.
Hold Rs7 button. Press Sd to toggle Solo in Front. Rotate Sd to control Solo in Front level.
You can also change project grid by touching the Speed Dial and hitting one of the drum pads (1/128 to 1 grid), simultaneously.

Keep in mind that there are lots of "hidden" features, like Tr8 button toggles metronome, but if you'd hold Rs7 (closest to Tr8) and start tapping Tr8, you can tap the project tempo.

In fx mode, when any track is selected, you can switch between it's plugins with A1-8 by single click. Double click A1-8 will toggle offline state for that fx. Holding A1-8 will show/hide plugin GUI. Double click and hold A1-8 will show this plugin's GUI exclusively, hiding all other GUIs.

Binaries are available in 'release' tab. I'm using Debug x64 version as my daily driver.
