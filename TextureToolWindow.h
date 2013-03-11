#ifndef TEXTURETOOLWINDOW_H
#define TEXTURETOOLWINDOW_H

#include "StdAfx.h"
#include "Subwindow.h"

namespace GUI
{
	class TextureToolWindow : public Subwindow
	{
	public:
		static TextureToolWindow& getInstance(void)
		{
			static TextureToolWindow instance;
			return instance;
		}
	private:
		TextureToolWindow(void);
		TextureToolWindow(TextureToolWindow const&);
		void operator=(TextureToolWindow const&);

	public:
		void init(HINSTANCE hInstance, HWND hParentWnd, const SubwindowDesc subwindowDesc);
		LRESULT subWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

		void addTextureButton(const std::string& name, EventReceiver* eventReceiver,
							  const std::string& bitmapFilename);
		void addLoadFileButton(const std::string& name, EventReceiver* eventReceiver);
		void addTrackbar(const std::string& name, EventReceiver* eventReceiver,
						 const UINT minValue, const UINT maxValue, const UINT startValue);

		UINT getTrackbarValue(const std::string& itemName) const;
	protected:
		int getItemID(const UINT i) const;
		int getItemID(const std::string& itemName) const;
	private:
		std::vector<UINT> m_checkableButtonIDs;

		UINT m_trackbarCount;

		void resetButtonHighlights(void);
	};
}

#endif