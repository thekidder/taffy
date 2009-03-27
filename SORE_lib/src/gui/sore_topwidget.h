//$Id$

#ifndef SORE_TOPWIDGET_H
#define SORE_TOPWIDGET_H

#include "../sore_input.h"
#include "../sore_screeninfo.h"
#include "sore_widget.h"

namespace SORE_GUI
{
	class TopWidget : public Widget
	{
	public:
		TopWidget(unsigned int width, unsigned int height);
		SORE_Graphics::ProjectionInfo GetProjection(SORE_Graphics::ScreenInfo s);
		bool OnResize(SORE_Kernel::Event* e);
	private:
		SORE_Graphics::render_list GetThisRenderList();
		bool ProcessEvents(SORE_Kernel::Event* e);
		void UpdateResolution(unsigned int w, unsigned int h);
	};
}

#endif
