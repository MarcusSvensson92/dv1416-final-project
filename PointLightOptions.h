#ifndef POINTLIGHTOPTIONS_H
#define POINTLIGHTOPTIONS_H

#include "TrackbarWindow.h"

namespace GUI
{
	class PointLightOptions : public TrackbarWindow
	{
	public:
		static PointLightOptions& getInstance(void)
		{
			static PointLightOptions instance;
			return instance;
		}
	private:
		PointLightOptions(void);
		PointLightOptions(PointLightOptions const&);
		void operator=(PointLightOptions const&);

	public:
		void init(HINSTANCE hInstance, HWND hParentWnd,
				  const SubwindowDesc subwindowDesc);
	};
}

#endif