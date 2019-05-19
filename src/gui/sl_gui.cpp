#pragma once
#ifndef SLGUI_H
#define SLGUI_H

#include "../csurf.h"

#include <JuceLibraryCode/AppConfig.h>
#include <JuceLibraryCode/JuceHeader.h>

using namespace std;
using namespace juce;


//class SLWindow final : public ResizableWindow
//{
//public:
//	SLWindow(String title, int w, int h, bool resizable, Colour bgcolor)
//		: ResizableWindow(title, bgcolor, false)
//	{
//		setContentNonOwned(&contentView, true);
//		setTopLeftPosition(100, 100);
//		setResizable(resizable, false);
//		setOpaque(true);
//		setSize(w, h);
//	}
//
//	int getDesktopWindowStyleFlags() const override
//	{
//		if (isResizable() == true)
//			return ComponentPeer::windowHasCloseButton | ComponentPeer::windowHasTitleBar | ComponentPeer::windowIsResizable | ComponentPeer::windowHasMinimiseButton;
//		return ComponentPeer::windowHasCloseButton | ComponentPeer::windowHasTitleBar | ComponentPeer::windowHasMinimiseButton;
//	}
//
//private:
//	SLViewGUI contentView;
//};

#endif // !SLGUI_H