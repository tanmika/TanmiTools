/*****************************************************************//**
 * @file   TanmiMapTool.hpp
 * @brief  一种二维地图生成工具
 *
 * @author tanmika
 * @date   October 2023
 *********************************************************************/
#pragma once
#ifndef _TANMI_MAP_TOOL_
#define _TANMI_MAP_TOOL_

#include<iostream>
#include<vector>
#include <string>
#include <fstream>
#include <bitset>

using std::vector;
/*管理地图工具的单例类*/
namespace TanmiTool {
	enum class MapMode
	{
		normal,
		prolifeway,
	};
	/**
	 * @brief 地图详细设置
	 */
	class TanmiMapTool;
	class TanmiMapMaskSetting
	{
		friend class TanmiMapTool;
	public:
		TanmiMapMaskSetting()
		{
			mask = 0;
		}
		~TanmiMapMaskSetting() = default;
		TanmiMapMaskSetting& RemoveIsolatedLand(bool on = true)
		{
			if (on)
				mask |= 1;
			else
				mask &= 0xFFFFFFFE;
			return *this;
		}
		TanmiMapMaskSetting& ProlifewayCircle(bool on = true)
		{
			if (on)
				mask |= 2;
			else
				mask &= 0xFFFFFFFD;
			return *this;
		}
		void PrintMask()
		{
			std::cout << std::bitset<8>(mask) << std::endl;
		}
	private:
		unsigned int mask;
	};
	struct TanmiMap
	{
		int width;	///< 地图宽度
		int height;	///< 地图高度
		int typeNum; ///< 地图中标记类型数量
		vector<vector<int>> map;	///< 地图数据
		TanmiMapMaskSetting setting;
		TanmiMap(int w, int h, int t) :width(w), height(h), typeNum(t), setting()
		{
			map.resize(w);
			for (auto& col : map)
			{
				col.resize(h);
				for (auto e : col)
				{
					e = 0;
				}
			}
		}
		void PrintMap()
		{
			for (auto& row : map)
			{
				for (auto& e : row)
				{
					std::cout << e << ' ';
				}
				std::cout << std::endl;
			}
		}
		void Write2File(const std::string& _path)
		{
			if (_path.empty() || _path.substr(_path.find_last_of('.')) != ".tanmimap")
			{
				std::cerr << "Invalid file extension. The file extension must be .tanmimap." << std::endl;
				return;
			}
			std::ofstream file(_path);
			if (file.is_open())
			{
				// Write TanmiMap data to the file
				file << width << " " << height << " " << typeNum << "\n";

				for (const auto& row : map)
				{
					for (int value : row)
					{
						file << value << " ";
					}
					file << "\n";
				}
				file.close();
				std::cout << "TanmiMap data has been written to the file." << std::endl;
			}
			else
			{
				std::cerr << "open file failed!" << std::endl;
				return;
			}
		}
	};
	
	class TanmiMapTool
	{
	public:
		static TanmiMapTool& Instance()
		{
			static TanmiMapTool* instance = new TanmiMapTool();
			return *instance;
		}
		TanmiMapTool& SetMode(MapMode _mode)
		{
			mode = _mode; return *this;
		}	///< 设置生成模式
		TanmiMapTool& SetSize(int _width, int _height)
		{
			width = std::max(1, _height);
			height = std::max(1, _width); return *this;
		}	///< 设置地图大小
		TanmiMapTool& SetRandSeed(unsigned int _seed)
		{
			srand(_seed);
			seed = _seed; return *this;
		}///< 设置随机种子
		TanmiMapTool& SetTypeNum(int num)
		{
			typeNum = std::max(2, num); return *this;
		}
		///< 设置地图中标记类型数量(至少为2)
		TanmiMapTool& SetMaskSetting(TanmiMapMaskSetting& _mask)
		{
			mask = &_mask; return *this;
		}///< 设置地图详细设置
		TanmiMap CreateMap();	///< 生成地图
	private:
		struct PointInfo
		{
			int id;
			int src_x;
			int src_y;
			int x;
			int y;
			PointInfo(int _x, int _y, int id) : src_x(_x), src_y(_y), x(_x), y(_y), id(id)
			{}
		};
		void CreateRamdonMap(TanmiMap& map, vector<int>& numList, float density);///<生成完全随机的地图
		void CreateProlifewayMap(TanmiMap& map, float desity, int prolifewayRange, int id);///<生成扩散式地图
		void bfs(TanmiMap& map, int step, int stepLimit, PointInfo& point, int id);///< 广度优先生成扩散
		TanmiMapTool() = default;
		~TanmiMapTool() = default;

		MapMode	mode = MapMode::normal;
		TanmiMapMaskSetting* mask = nullptr;
		int width = 10;
		int height = 10;
		unsigned int seed = 0;
		int typeNum = 2;
	};
	TanmiMap TanmiMapTool::CreateMap()
	{
		TanmiMap map(width, height, typeNum);
		map.setting = mask ? *mask : map.setting;
		vector<int> num;

		switch (mode)
		{
		case TanmiTool::MapMode::normal:
			for (size_t i = 0; i < typeNum; i++)
			{
				num.push_back(i);
			}
			CreateRamdonMap(map, num, 1);
			break;
		case TanmiTool::MapMode::prolifeway:
			for (size_t i = 1; i < typeNum; i++)
			{
				CreateProlifewayMap(map, 0.01f, (width + height) / 6, i);
			}
			break;
		default:
			break;
		}
		return map;
	}
	inline void TanmiMapTool::CreateRamdonMap(TanmiMap& map, vector<int>& numList, float density)
	{
		for (auto& row : map.map)
		{
			for (auto& e : row)
			{
				float r = (float)(rand() % 1000) / 1000;
				if (r <= density)
				{
					e = numList[rand() % numList.size()];
				}
			}
		}
	}
	inline void TanmiMapTool::CreateProlifewayMap(TanmiMap& map, float density, int prolifewayRange, int id)
	{
		for (int i = 0; i < map.width; i++)
		{
			for (int j = 0; j < map.height; j++)
			{
				float r = (float)(rand() % 1000) / 1000;
				if (r <= density)
				{
					int range = rand() % (prolifewayRange - 1) + 1;
					PointInfo point(i, j, id);
					bfs(map, 0, range, point, id);
				}
			}
		}
	}
	inline void TanmiMapTool::bfs(TanmiMap& map, int step, int stepLimit, PointInfo& point, int id)
	{
		if (point.x < 0 || point.x >= map.width || point.y < 0 || point.y >= map.height)
			return;
		if (map.map[point.x][point.y] == id)
			return;
		map.map[point.x][point.y] = id;
		step++;
		if (!(map.setting.mask && 2) && step >= stepLimit)
			return;
		else if ((map.setting.mask && 2) &&
			((point.src_x - point.x) * (point.src_x - point.x) +
				(point.src_y - point.y) * (point.src_y - point.y) >
				stepLimit * stepLimit * 0.64))
			return;
		point.x += 1;
		bfs(map, step, stepLimit, point, id);
		point.x -= 2;
		bfs(map, step, stepLimit, point, id);
		point.x += 1;
		point.y += 1;
		bfs(map, step, stepLimit, point, id);
		point.y -= 2;
		bfs(map, step, stepLimit, point, id);
		point.y += 1;
	}
}

#endif // _TANMI_MAP_TOOL_