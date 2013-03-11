#ifndef TOOL_H
#define TOOL_H

#include "StdAfx.h"
#include "EventReceiver.h"

class Tool : public GUI::EventReceiver
{
public:
	Tool(void);
	virtual ~Tool(void);

	virtual void update(const float dt) = 0;
};

#endif