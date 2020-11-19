#pragma once
#include <engine.h>
#include "glm/gtx/rotate_vector.hpp"

class player
{

public:

	player();
	~player();
	void initialise(engine::ref<engine::game_object> object);
	void on_update(const engine::timestep& time_step);

	engine::ref<engine::game_object> object() const { return m_object; }

	void turn(float angle);
	void update_first_person_camera(engine::perspective_camera& camera);
	void update_third_person_camera(engine::perspective_camera& camera);
	void jump();
	void sprint(const bool& activateSprint);

private:

	float m_speed{ 0.f };
	float m_timer;
	float animation_speed{ 1.f };

	float forward{ 10.f };

	float m_yaw = -90.0f;
	float m_pitch = 0.0f;
	glm::vec3 player_front { 0.f,0.f,1.f };
	glm::vec3 player_up { 0.f,1.f,0.f };
	glm::vec3 player_right { 1.f,0.f,0.f };
	glm::vec3 player::process_mouse();

	bool isFirstPerson = true;
	bool rotateAround = false;

	engine::ref< engine::game_object> m_object;
};
