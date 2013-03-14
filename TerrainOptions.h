#ifndef TERRAINOPTIONS_H
#define TERRAINOPTIONS_H

#include "TrackbarWindow.h"

namespace GUI
{
	class TerrainOptions : public TrackbarWindow
	{
	public:
		static TerrainOptions& getInstance(void)
		{
			static TerrainOptions instance;
			return instance;
		}
	private:
		TerrainOptions(void);
		TerrainOptions(TerrainOptions const&);
		void operator=(TerrainOptions const&);

	public:
		void init(HINSTANCE hInstance, HWND hParentWnd,
				  const SubwindowDesc subwindowDesc);
	};
}

#endif