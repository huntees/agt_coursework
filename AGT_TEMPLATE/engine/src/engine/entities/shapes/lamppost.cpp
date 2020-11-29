#include "pch.h"
#include "lamppost.h"
#include <engine.h>

engine::lamppost::lamppost() {

	std::vector<engine::mesh::vertex> lamppost_vertices

	{
		//top
		{ { 0.f, 0.3f, 0.f },			{ 0.f, 1.f, 0.f },			{ 0.f, 0.f } },
		{ { 0.3f, 0.3f, 0.0f },			{ 0.f, 1.f, 0.f },			{ 1.f, 0.f } },
		{ { 0.3f, 0.3f, -0.3f },		{ 0.f, 1.f, 0.f },			{ 1.f, 1.f } },
		{ { 0.f, 0.3f, -0.3f },			{ 0.f, 1.f, 0.f },			{ 0.f, 1.f } }, //3

		//front base face
		{ { 0.f, 0.3f, 0.f },			{ 0.f, 0.f, 1.f },			{ 0.f, 1.f } },
		{ { 0.f, 0.0f, 0.1f },			{ 0.f, 0.f, 1.f },			{ 0.f, 0.f } },
		{ { 0.3f, 0.0f, 0.1f },		    { 0.f, 0.f, 1.f },			{ 1.f, 0.f } },
		{ { 0.3f, 0.3f, 0.0f },			{ 0.f, 0.f, 1.f },			{ 1.f, 1.f } }, //7

		//front and right base connect
		{ { 0.3f, 0.3f, 0.0f },			glm::cross(glm::vec3(0.3f, 0.3f, 0.0f) - glm::vec3(0.3f, 0.0f, 0.1f),
													glm::vec3(0.3f, 0.3f, 0.0f) - glm::vec3(0.4f, 0.0f, 0.0f)),			{ 0.5f, 1.f } },
		{ { 0.3f, 0.0f, 0.1f },			glm::cross(glm::vec3(0.3f, 0.3f, 0.0f) - glm::vec3(0.3f, 0.0f, 0.1f),
													glm::vec3(0.3f, 0.3f, 0.0f) - glm::vec3(0.4f, 0.0f, 0.0f)),			{ 0.f, 0.f } },
		{ { 0.4f, 0.0f, 0.0f },		    glm::cross(glm::vec3(0.3f, 0.3f, 0.0f) - glm::vec3(0.3f, 0.0f, 0.1f),
													glm::vec3(0.3f, 0.3f, 0.0f) - glm::vec3(0.4f, 0.0f, 0.0f)),			{ 1.f, 0.f } }, //10

		//right base face
		{ { 0.3f, 0.3f, 0.0f },			{ 1.f, 0.f, 0.f },			{ 0.f, 1.f } },
		{ { 0.4f, 0.0f, 0.0f },			{ 1.f, 0.f, 0.f },			{ 0.f, 0.f } },
		{ { 0.4f, 0.0f, -0.3f },		{ 1.f, 0.f, 0.f },			{ 1.f, 0.f } },
		{ { 0.3f, 0.3f, -0.3f },		{ 0.f, 0.f, 1.f },			{ 1.f, 1.f } }, //14

		//right and back base connect
		{ { 0.3f, 0.3f, -0.3f },		glm::cross(glm::vec3(0.3f, 0.3f, -0.3f) - glm::vec3(0.4f, 0.0f, -0.3f),
													glm::vec3(0.3f, 0.3f, -0.3f) - glm::vec3(0.3f, 0.f, -0.4f)),			{ 0.5f, 1.f } },
		{ { 0.4f, 0.0f, -0.3f },		glm::cross(glm::vec3(0.3f, 0.3f, -0.3f) - glm::vec3(0.4f, 0.0f, -0.3f),
													glm::vec3(0.3f, 0.3f, -0.3f) - glm::vec3(0.3f, 0.f, -0.4f)),			{ 0.f, 0.f } },
		{ { 0.3f, 0.f, -0.4f },			glm::cross(glm::vec3(0.3f, 0.3f, -0.3f) - glm::vec3(0.4f, 0.0f, -0.3f),
													glm::vec3(0.3f, 0.3f, -0.3f) - glm::vec3(0.3f, 0.f, -0.4f)),			{ 1.f, 0.f } }, //17

		//back base face
		{ { 0.3f, 0.3f, -0.3f },		{ 0.f, 0.f, -1.f },			{ 0.f, 1.f } },
		{ { 0.3f, 0.f, -0.4f },			{ 0.f, 0.f, -1.f },			{ 0.f, 0.f } },
		{ { 0.f, 0.f, -0.4f },			{ 0.f, 0.f, -1.f },			{ 1.f, 0.f } },
		{ { 0.f, 0.3f, -0.3f },			{ 0.f, 0.f, -1.f },			{ 1.f, 1.f } }, //21

		//back and left base connect
		{ { 0.f, 0.3f, -0.3f },			glm::cross(glm::vec3(0.f, 0.3f, -0.3f) - glm::vec3(0.f, 0.f, -0.4f),
													glm::vec3(0.f, 0.3f, -0.3f) - glm::vec3(-0.1f, 0.f, -0.3f)),			{ 0.5f, 1.f } },
		{ { 0.f, 0.f, -0.4f },			glm::cross(glm::vec3(0.f, 0.3f, -0.3f) - glm::vec3(0.f, 0.f, -0.4f),
													glm::vec3(0.f, 0.3f, -0.3f) - glm::vec3(-0.1f, 0.f, -0.3f)),			{ 0.f, 0.f } },
		{ { -0.1f, 0.f, -0.3f },		glm::cross(glm::vec3(0.f, 0.3f, -0.3f) - glm::vec3(0.f, 0.f, -0.4f),
													glm::vec3(0.f, 0.3f, -0.3f) - glm::vec3(-0.1f, 0.f, -0.3f)),			{ 1.f, 0.f } }, //24

		//left base face
		{ { 0.f, 0.3f, -0.3f },			{ -1.f, 0.f, 0.f },			{ 0.f, 1.f } },
		{ { -0.1f, 0.0f, -0.3f },		{ -1.f, 0.f, 0.f },			{ 0.f, 0.f } },
		{ { -0.1f, 0.0f, 0.f },			{ -1.f, 0.f, 0.f },			{ 1.f, 0.f } },
		{ { 0.f, 0.3f, 0.f },			{ -1.f, 0.f, 0.f },			{ 1.f, 1.f } }, //28

		//left and front base connect
		{ { 0.f, 0.3f, 0.f },			glm::cross(glm::vec3(0.f, 0.3f, 0.f) - glm::vec3(-0.1f, 0.0f, 0.f),
													glm::vec3(0.f, 0.3f, 0.f) - glm::vec3(0.f, 0.0f, 0.1f)),			{ 0.5f, 1.f } },
		{ { -0.1f, 0.0f, 0.f },			glm::cross(glm::vec3(0.f, 0.3f, 0.f) - glm::vec3(-0.1f, 0.0f, 0.f),
													glm::vec3(0.f, 0.3f, 0.f) - glm::vec3(0.f, 0.0f, 0.1f)),				{ 0.f, 0.f } },
		{ { 0.f, 0.0f, 0.1f },			glm::cross(glm::vec3(0.f, 0.3f, 0.f) - glm::vec3(-0.1f, 0.0f, 0.f),
													glm::vec3(0.f, 0.3f, 0.f) - glm::vec3(0.f, 0.0f, 0.1f)),				{ 0.f, 1.f } }, //31

		//pole front
		{ { 0.08f, 5.f, -0.08f },		{ 0.f, 0.f, 1.f },			{ 0.f, 1.f } },
		{ { 0.08, 0.3f, -0.08f },		{ 0.f, 0.f, 1.f },			{ 0.f, 0.f } },
		{ { 0.22f, 0.3f, -0.08f },		{ 0.f, 0.f, 1.f },			{ 1.f, 0.f } },
		{ { 0.22f, 5.f, -0.08f },		{ 0.f, 0.f, 1.f },			{ 1.f, 1.f } },//35

		//pole right
		{ { 0.22f, 5.f, -0.08f },		{ 1.f, 0.f, 0.f },			{ 0.f, 1.f } },
		{ { 0.22f, 0.3f, -0.08f },		{ 1.f, 0.f, 0.f },			{ 0.f, 0.f } },
		{ { 0.22f, 0.3f, -0.22f },		{ 1.f, 0.f, 0.f },			{ 1.f, 0.f } },
		{ { 0.22f, 5.f, -0.22f },		{ 1.f, 0.f, 0.f },			{ 1.f, 1.f } },//39

		//pole back
		{ { 0.22f, 5.f, -0.22f },		{ 0.f, 0.f, -1.f },			{ 0.f, 1.f } },
		{ { 0.22f, 0.3f, -0.22f },		{ 0.f, 0.f, -1.f },			{ 0.f, 0.f } },
		{ { 0.08f, 0.3f, -0.22f },		{ 0.f, 0.f, -1.f },			{ 1.f, 0.f } },
		{ { 0.08f, 5.f, -0.22f },		{ 0.f, 0.f, -1.f },			{ 1.f, 1.f } },//43

		//pole left
		{ { 0.08f, 5.f, -0.22f },		{ -1.f, 0.f, 0.f },			{ 0.f, 1.f } },
		{ { 0.08f, 0.3f, -0.22f },		{ -1.f, 0.f, 0.f },			{ 0.f, 0.f } },
		{ { 0.08f, 0.3f, -0.08f },		{ -1.f, 0.f, 0.f },			{ 1.f, 0.f } },
		{ { 0.08f, 5.f, -0.08f },		{ -1.f, 0.f, 0.f },			{ 1.f, 1.f } },//47

		//lamp top
		{ { 0.02f, 5.1f, -0.4f },		{ 0.f, 1.f, 0.f },			{ 0.f, 1.f } },
		{ { 0.02f, 5.1f, 2.f },			{ 0.f, 1.f, 0.f },			{ 0.f, 0.f } },
		{ { 0.28f, 5.1f, 2.f },			{ 0.f, 1.f, 0.f },			{ 1.f, 0.f } },
		{ { 0.28f, 5.1f, -0.4f },		{ 0.f, 1.f, 0.f },			{ 1.f, 1.f } }, //51

		//lamp bottom
		{ { 0.02f, 5.f, 2.f },			{ 0.f, -1.f, 0.f },			{ 0.f, 1.f } },
		{ { 0.02f, 5.f, -0.4f },		{ 0.f, -1.f, 0.f },			{ 0.f, 0.f } },
		{ { 0.28f, 5.f, -0.4f },		{ 0.f, -1.f, 0.f },			{ 1.f, 0.f } },
		{ { 0.28f, 5.f, 2.f },			{ 0.f, -1.f, 0.f },			{ 1.f, 1.f } }, //51

		//lamp front
		{ { 0.02f, 5.1f, 2.f },			{ 0.f, 0.f, 1.f },			{ 0.f, 1.f } },
		{ { 0.02f, 5.f, 2.f },			{ 0.f, 0.f, 1.f },			{ 0.f, 0.f } },
		{ { 0.28f, 5.f, 2.f },			{ 0.f, 0.f, 1.f },			{ 1.f, 0.f } },
		{ { 0.28f, 5.1f, 2.f },			{ 0.f, 0.f, 1.f },			{ 1.f, 1.f } },//63

		//lamp right
		{ { 0.28f, 5.1f, 2.f },			{ 1.f, 0.f, 0.f },			{ 0.f, 1.f } },
		{ { 0.28f, 5.f, 2.f },			{ 1.f, 0.f, 0.f },			{ 0.f, 0.f } },
		{ { 0.28f, 5.f, -0.4f },		{ 1.f, 0.f, 0.f },			{ 1.f, 0.f } },
		{ { 0.28f, 5.1f, -0.4f },		{ 1.f, 0.f, 0.f },			{ 1.f, 1.f } },//67

		//back lamp
		{ { 0.28f, 5.1f, -0.4f },		{ -1.f, 0.f, 0.f },			{ 0.f, 1.f } },
		{ { 0.28f, 5.f, -0.4f },		{ -1.f, 0.f, 0.f },			{ 0.f, 0.f } },
		{ { 0.02f, 5.f, -0.4f },		{ -1.f, 0.f, 0.f },			{ 1.f, 0.f } },
		{ { 0.02f, 5.1f, -0.4f },		{ -1.f, 0.f, 0.f },			{ 1.f, 1.f } },//71

		//left lamp
		{ { 0.02f, 5.1f, -0.4f },		{ 0.f, 0.f, 1.f },			{ 0.f, 1.f } },
		{ { 0.02f, 5.f, -0.4f },		{ 0.f, 0.f, 1.f },			{ 0.f, 0.f } },
		{ { 0.02f, 5.f, 2.f },			{ 0.f, 0.f, 1.f },			{ 1.f, 0.f } },
		{ { 0.02f, 5.1f, 2.f },			{ 0.f, 0.f, 1.f },			{ 1.f, 1.f } },//75

	};

	const std::vector<uint32_t> lamppost_indices
	{
		 0,  1,  2,		 0,  2,  3,  //top
		 4,  5,  6,		 4,  6,  7,  //front base face
		 8,  9, 10,					 //front and right base connect
		11, 12, 13,		11, 13, 14,  //right base face
		15, 16, 17,					 //right and back base connect  
		18, 19, 20,		18, 20, 21,  //back base face
		22, 23, 24,					 //back and left base connect
		25, 26, 27,		25, 27, 28,  //left base face
		29, 30, 31,					 //left and front base connect
		32, 33, 34,		32, 34, 35,
		36, 37, 38,		36, 38, 39,
		40, 41, 42,		40, 42, 43,
		44, 45, 46,		44, 46, 47,
		48, 49, 50,		48, 50, 51,
		52, 53, 54,		52, 54, 55,
		56, 57, 58,		56, 58, 59,
		60, 61, 62,		60, 62, 63,
		64, 65, 66,		64, 66, 67,
		68, 69, 70,		68,	70, 71,
		72, 73, 74,		72, 74, 75,
	};

	m_mesh = engine::mesh::create(lamppost_vertices, lamppost_indices);
}

engine::lamppost::~lamppost() {}

engine::ref<engine::lamppost> engine::lamppost::create() {

	return std::make_shared<engine::lamppost>();
}
