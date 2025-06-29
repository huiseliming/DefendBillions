#pragma once
#include "core.h"
#include <set>

namespace FlowFieldPathfinding {

	enum FlowFieldDirection : uint8_t
	{
		FFD_None = 1ui8 << 0,
		FFD_LBit = 1ui8 << 0,
		FFD_RBit = 1ui8 << 1,
		FFD_UBit = 1ui8 << 2,
		FFD_DBit = 1ui8 << 3,

		FFD_LeftBit = FFD_LBit,
		FFD_RightBit = FFD_RBit,
		FFD_UpBit = FFD_UBit,
		FFD_DownBit = FFD_DBit,

		//FFD_0____ = 0,
		//FFD_1_L_D = 1,
		//FFD_2_M_D = 2,
		//FFD_3_R_D = 3,
		//FFD_4_L_M = 4,
		//FFD_5_M_M = 5,
		//FFD_6_R_M = 6,
		//FFD_7_L_U = 7,
		//FFD_8_M_U = 8,
		//FFD_9_R_U = 9,
	};
	struct Tile {
		uint32_t m_Cost : 20 { UINT32_MAX };
		uint32_t m_Reserved : 7  { 0 };
		uint32_t m_Moveable : 1  { 1 };
		uint32_t m_Direction : 4  { FFD_None };
	};

	struct Position {
		uint16_t m_X;
		uint16_t m_Y;
	};

	struct TileMap {
	public:
		TileMap(uint32_t p_Width, uint32_t p_Height)
			: m_Width(p_Width)
			, m_Height(p_Height)
			, m_Tiles(p_Width* p_Height)
		{
		}
		FORCEINLINE Tile& GetTile(uint32_t p_PosX, uint32_t p_PosY) { return m_Tiles[p_PosX + p_PosY * m_Width]; }
		FORCEINLINE Tile& GetTile(Position p_Pos) { return m_Tiles[p_Pos.m_X + p_Pos.m_Y * m_Width]; }
		FORCEINLINE uint32_t GetWidth() { return m_Width; }
		FORCEINLINE uint32_t GetHeight() { return m_Height; }
		FORCEINLINE void Preprocess() {
			for (size_t i = 0; i < m_Tiles.size(); i++)
			{
				m_Tiles[i].m_Cost = 0xFFFFF;
				m_Tiles[i].m_Direction = FFD_None;
			}
		}

		bool Process(Position p_TargetPos) {
			//
			Preprocess();
			// 
			std::vector<Position> tiles;
			std::vector<Position> tiles_next;
			auto& target_tile = GetTile(p_TargetPos);
			target_tile.m_Cost = 0;
			if (!target_tile.m_Moveable) return false;
			tiles.push_back(p_TargetPos);
			while (!tiles.empty())
			{
				for (auto tile_pos : tiles)
				{
					auto& tile = GetTile(tile_pos);
					constexpr FlowFieldDirection neighbor_tile_direction_array[] = {
						FFD_LBit,
						FFD_RBit,
						FFD_UBit,
						FFD_DBit,
					};
					Position neighbor_tile_pos_array[] = {
						Position(tile_pos.m_X - 1, tile_pos.m_Y),
						Position(tile_pos.m_X + 1, tile_pos.m_Y),
						Position(tile_pos.m_X, tile_pos.m_Y - 1),
						Position(tile_pos.m_X, tile_pos.m_Y + 1),
					};
					Tile* neighbor_tile_array[] = {
						(tile_pos.m_X > 0 ? &GetTile(tile_pos.m_X - 1, tile_pos.m_Y) : nullptr),
						(tile_pos.m_X < m_Width - 1 ? &GetTile(tile_pos.m_X + 1, tile_pos.m_Y) : nullptr),
						(tile_pos.m_Y > 0 ? &GetTile(tile_pos.m_X, tile_pos.m_Y - 1) : nullptr),
						(tile_pos.m_Y < m_Height - 1 ? &GetTile(tile_pos.m_X, tile_pos.m_Y + 1) : nullptr),
					};
					for (size_t i = 0; i < std::size(neighbor_tile_array); i++)
					{
						auto neighbor_tile = neighbor_tile_array[i];
						if (neighbor_tile && neighbor_tile->m_Moveable)
						{
							if (neighbor_tile->m_Cost > tile.m_Cost + 1) {
								neighbor_tile->m_Cost = tile.m_Cost + 1;
								neighbor_tile->m_Direction |= neighbor_tile_direction_array[i];
								tiles_next.push_back(neighbor_tile_pos_array[i]);
							}
							else if (neighbor_tile->m_Cost == tile.m_Cost + 1) {
								neighbor_tile->m_Direction |= neighbor_tile_direction_array[i];
							}
						}
					}
				}
				tiles.clear();
				tiles.swap(tiles_next);
			}
			return true;
		}

	protected:
		uint32_t m_Width;
		uint32_t m_Height;
		std::vector<Tile> m_Tiles;
		friend class FlowFieldPathfinding;
	};

	//class FlowFieldPathfinding : public TModule<FlowFieldPathfinding> {
	//	DECL_MODULE(FlowFieldPathfinding)
	//public:
	//};

	//IMPL_MODULE(FlowFieldPathfinding);
}






















 