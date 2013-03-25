#ifndef DIRECTIONALLIGHTOPTIONS_H
#define DIRECTIONALLIGHTOPTIONS_H

#include "TrackbarWindow.h"

namespace GUI
{
	class DirectionalLightOptions : public TrackbarWindow
	{
	public:
		static DirectionalLightOptions& getInstance(void)
		{
			static DirectionalLightOptions instance;
			return instance;
		}
	private:
		DirectionalLightOptions(void);
		DirectionalLightOptions(DirectionalLightOptions const&);
		void operator=(DirectionalLightOptions const&);

	public:
		void init(HINSTANCE hInstance, HWND hParentWnd,
				  const SubwindowDesc subwindowDesc);
	};
}

#endif