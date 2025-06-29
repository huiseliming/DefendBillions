#pragma once
#include "flow_field_pathfinding.h"
#include "godot_cpp/classes/node2d.hpp"

namespace godot {
	class FlowFieldPathfindingMap : public Node2D
	{
		GDCLASS(FlowFieldPathfindingMap, Node2D);
		static void _bind_methods();
	public:

		void _draw() override;
		void _enter_tree() override;
		void _exit_tree() override;


		uint32_t m_Width{ 128 };
		uint32_t m_Height{ 128 };
		std::unique_ptr<FlowFieldPathfinding::TileMap> m_TileMap;

	};

}

