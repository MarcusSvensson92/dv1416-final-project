#ifndef LEVELTOOLWINDOW_H
#define LEVELTOOLWINDOW_H

#include "StdAfx.h"
#include "Subwindow.h"

namespace GUI
{
	class LevelToolWindow : public Subwindow
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

		void addTrackbar(const std::string& name, EventReceiver* eventReceiver,
						 const UINT minValue, const UINT maxValue, const UINT startValue);

		UINT getTrackbarValue(const std::string& itemName) const;
	protected:
		int getItemID(const UINT i) const;
		int getItemID(const std::string& itemName) const;
	};
}

#endif