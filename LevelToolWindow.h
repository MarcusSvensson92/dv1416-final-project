#ifndef LEVELTOOLWINDOW_H
#define LEVELTOOLWINDOW_H

#include "StdAfx.h"
#include "Subwindow.h"

namespace GUI
{
	struct LevelToolWindowDesc
	{
		std::string caption;
		UINT		x;
		UINT		y;
		UINT		width;
		UINT		height;
		UINT		margin;
	};

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
				  const LevelToolWindowDesc levelToolWindowDesc);
		void addTrackbar(const std::string& name, EventReceiver* eventReceiver,
						 const UINT minValue, const UINT maxValue, const UINT startValue);

		UINT getTrackbarValue(const std::string& itemName) const;
	protected:
		int getItemID(const UINT i) const;
	private:
		LevelToolWindowDesc m_levelToolWindowDesc;

		int getItemID(const std::string& itemName) const;
	};
}

#endif