#include <iostream>
#include <vector>
#include "../include/TanmiMapTool.hpp"

using namespace TanmiTool;

int main()
{
	TanmiMapMaskSetting setting;
	//setting.PrintMask();
	setting.ProlifewayCircle();
	//setting.PrintMask();
	auto map = TanmiMapTool::Instance().SetMaskSetting(setting)
		.SetMode(MapMode::prolifeway)
		.SetRandSeed(13)
		.SetSize(50,40)
		.CreateMap();

	setting.RemoveIsolatedLand(true);
	auto map2 = TanmiMapTool::Instance().SetMaskSetting(setting)
		.SetMode(MapMode::prolifeway)
		.SetRandSeed(13)
		.SetSize(50, 40)
		.CreateMap();
	map.PrintMap();
	std::cout << std::endl;
	map2.PrintMap();	
	std::cout << std::endl;
	TanmiMap::Compare(map, map2);

	return 0;
}