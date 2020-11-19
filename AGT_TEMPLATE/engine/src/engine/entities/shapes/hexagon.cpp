#include "pch.h"
#include "hexagon.h"
#include <engine.h>

engine::hexagon::hexagon() {

	std::vector<engine::mesh::vertex> hexagon_vertices;

	for (int i = 0; i < 6; i++) {

		float theta = (i / 6.0f) * 2 * engine::PI;

		engine::mesh::vertex vertex(
			glm::vec3(cos(theta), sin(theta), 0.0f),
			glm::vec3(0.0f, 0.0f, 1.0f),
			glm::vec2(cos(theta) / 2.0f + 0.25f, sin(theta) / 2.0f + 0.25f));

		hexagon_vertices.push_back(vertex);
	}

	const std::vector<uint32_t> hexagon_indices
	{
		0,1,2,
		2,3,0,
		3,4,5,
		5,0,3
	};

	m_mesh = engine::mesh::create(hexagon_vertices, hexagon_indices);
}

engine::hexagon::~hexagon() {}

engine::ref<engine::hexagon> engine::hexagon::create() {

	return std::make_shared<engine::hexagon>();
}
