#ifndef LEVELTOOLWINDOW_H
#define LEVELTOOLWINDOW_H

#include "TrackbarWindow.h"

namespace GUI
{
	class LevelToolWindow : public TrackbarWindow
	{
	public:
		static LevelToolWindow& getInstance(void)
		{
			static LevelToolWindow instance;
			return instance;
		}
	private:
		LevelToolWindow(void);
		LevelToolWindow(LevelToolWindow const&);
		void operator=(LevelToolWindow const&);

	public:
		void init(HINSTANCE hInstance, HWND hParentWnd,
				  const SubwindowDesc subwindowDesc);
	};
}

#endif