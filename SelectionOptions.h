#ifndef SELECTIONOPTIONS_H
#define SELECTIONOPTIONS_H

#include "TrackbarWindow.h"

namespace GUI
{
	class SelectionOptions : public TrackbarWindow
	{
	public:
		static SelectionOptions& getInstance(void)
		{
			static SelectionOptions instance;
			return instance;
		}
	private:
		SelectionOptions(void);
		SelectionOptions(SelectionOptions const&);
		void operator=(SelectionOptions const&);

	public:
		void init(HINSTANCE hInstance, HWND hParentWnd,
				  const SubwindowDesc subwindowDesc);
	};
}

#endif