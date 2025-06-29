#include "flow_field_pathfinding_map.h"
#include "godot_cpp/classes/theme_db.hpp"
#include "godot_cpp/classes/font.hpp"

using namespace godot;

void FlowFieldPathfindingMap::_bind_methods()
{
}

void FlowFieldPathfindingMap::_draw()
{
	if (m_TileMap) {
		float display_size = 128;
		for (size_t j = 0; j < m_TileMap->GetHeight(); j++)
		{
			for (size_t i = 0; i < m_TileMap->GetWidth(); i++)
			{
				auto& tile = m_TileMap->GetTile(i, j);
				if (tile.m_Moveable)
				{
					draw_rect(Rect2(i * display_size, j * display_size, display_size, display_size), Color::from_string("white", Color(0, 0, 0)));
					draw_string(ThemeDB::get_singleton()->get_fallback_font(), Vector2(i * display_size, j * display_size), String::num_int64(tile.m_Cost), HORIZONTAL_ALIGNMENT_LEFT, -1, 16, Color::from_string("red", Color()));
				}
				else 
				{
					draw_rect(Rect2(i * display_size, j * display_size, display_size, display_size), Color::from_string("black", Color(0, 0, 0)));
				}
			}
		}
	}
	draw_string(ThemeDB::get_singleton()->get_fallback_font(), Vector2(0, 0), "test_str", HORIZONTAL_ALIGNMENT_LEFT, -1);
}

void FlowFieldPathfindingMap::_enter_tree()
{
	m_TileMap = std::make_unique<FlowFieldPathfinding::TileMap>(m_Width, m_Height);
	m_TileMap->Process({1, 1});
}

void FlowFieldPathfindingMap::_exit_tree()
{
}
