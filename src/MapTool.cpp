#include <iostream>
#include <vector>
#include "../include/TanmiMapTool.hpp"

using namespace TanmiTool;

int main()
{
	TanmiMapMaskSetting setting;
	//setting.PrintMask();
	setting.RemoveIsolatedLand()
		.ProlifewayCircle();
	//setting.PrintMask();
	auto map = TanmiMapTool::Instance().SetMaskSetting(setting)
		.SetMode(MapMode::prolifeway)
		.SetRandSeed(114510)
		.SetSize(50,40)
		.CreateMap();
	map.PrintMap();
	return 0;
}