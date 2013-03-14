#ifndef TERRAINOPTIONS_H
#define TERRAINOPTIONS_H

#include "StdAfx.h"
#include "Subwindow.h"

namespace GUI
{
	class TerrainOptions : public Subwindow
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

		void addTrackbar(const std::string& name, EventReceiver* eventReceiver,
						 const UINT minValue, const UINT maxValue, const UINT startValue);

		UINT getTrackbarValue(const std::string& itemName) const;
	protected:
		int getItemID(const UINT i) const;
		int getItemID(const std::string& itemName) const;
	};
}
#endif