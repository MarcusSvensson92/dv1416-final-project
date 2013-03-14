#ifndef TRACKBARWINDOW_H
#define TRACKBARWINDOW_H

#include "Subwindow.h"

namespace GUI
{
	class TrackbarWindow : public Subwindow
	{
	public:
		TrackbarWindow(void);
		virtual ~TrackbarWindow(void);

		void addTrackbar(const std::string& name, EventReceiver* eventReceiver,
						 const UINT minValue, const UINT maxValue, const UINT startValue);
		UINT getTrackbarValue(const std::string& itemName) const;
	protected:
		POINT m_trackbarSize;
		UINT  m_trackbarTextSize;
		UINT  m_trackbarMargin;
	};
}

#endif