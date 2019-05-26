#pragma once
#ifndef SLGUI_H
#define SLGUI_H

#include <nana/gui.hpp>
#include <nana/gui/widgets/button.hpp>
#include <nana/gui/widgets/label.hpp>
#include <nana/gui/widgets/slider.hpp>

class SLGui {
public:
	void SLGui::ShowGui() {

		using namespace nana;

		nana::form fm;
		label lb{ fm };
		lb.caption("Hello, world!");
		lb.bgcolor(colors::azure);
		//Define a layout object for the form.
		place layout(fm);
		//button btn(fm, rectangle(20, 20, 150, 30));
		//btn.caption("Quit");
		//btn.events().click(API::exit);  // API::exit is a function that is triggered on click

		slider sld1(fm, rectangle(50, 60, 30, 150));
		sld1.vertical(true);
		slider sld2(fm, rectangle(100, 60, 30, 150));
		sld2.vertical(true);
		slider sld3(fm, rectangle(150, 60, 30, 150));
		sld3.vertical(true);
		slider sld4(fm, rectangle(200, 60, 30, 150));
		sld4.vertical(true);
		slider sld5(fm, rectangle(250, 60, 30, 150));
		sld5.vertical(true);
		slider sld6(fm, rectangle(300, 60, 30, 150));
		sld6.vertical(true);
		slider sld7(fm, rectangle(350, 60, 30, 150));
		sld7.vertical(true);
		slider sld8(fm, rectangle(400, 60, 30, 150));
		sld8.vertical(true);

		////The div-text
		layout.div("vert<><<><here weight=80><>><>");
		layout["here"] << lb;
		layout.collocate();

		fm.show();
		nana::exec();

	}

};



#endif // !SLGUI_H