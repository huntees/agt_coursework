#pragma once
#include <engine.h>

class particle
{
public:
	particle();
	~particle();
	bool initialise(glm::vec3 p, float width, float height, glm::vec3 start_colour, glm::vec3 end_colour, float transition_speed, float life);
	glm::mat4 on_render(glm::vec3 lookAt, glm::vec3 upVector, engine::ref<engine::shader> shader);
	void on_update(const engine::timestep& timestep);
	bool is_alive() { return (m_life > 0); }

private:
	float m_life;
	glm::vec3 m_position;
	glm::vec3 m_velocity;

	float m_width;
	float m_height;

	float m_angle;
	float m_angular_velocity;

	glm::vec3 m_start_colour;
	glm::vec3 m_end_colour;
	float m_transition_speed;

};
