#ifndef EVENTRECEIVER_H
#define EVENTRECEIVER_H

#include "StdAfx.h"

namespace GUI
{
	class EventReceiver
	{
	public:
		virtual void onEvent(const std::string& sender, const std::string& eventName) = 0;
	};

	typedef std::pair<const std::string, EventReceiver*> EventElement;
}

#endif