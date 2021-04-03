#include "pch.h"
#include "enemy.h"

enemy::enemy() {}

enemy::~enemy() {}

void enemy::initialise(engine::ref<engine::game_object> object, glm::vec3 position, glm::vec3 forward, bool canFly)
{
	m_object = object;
	m_object->set_forward(forward);
	m_object->set_position(position);
	can_fly = canFly;

	if (can_fly) {
		m_detection_radius = 8.f;
		m_trigger_radius = 6.f;

		m_speed = 1.1f;
		health_point = flying_hp;
	}
}

void enemy::on_update(const engine::timestep& time_step, const glm::vec3& player_position)
{
	float distance_to_player = glm::distance(m_object->position(), player_position);

	//allow target to fly
	if (can_fly) {
		//m_object->set_acceleration(glm::vec3(m_object->acceleration().x, 9.8f, m_object->acceleration().z));
		//m_object->set_velocity(glm::vec3(m_object->velocity().x, 0.f, m_object->velocity().z));
		m_object->set_velocity(glm::vec3(0.f));
		m_object->set_acceleration(glm::vec3(0.f));
	}

	// check which state is the enemy in, then execute the matching behaviour
	if (m_state == state::idle)
	{
		patrol(time_step);
		// check whether the condition has been met to switch to the on_guard state
		if (distance_to_player < m_detection_radius)
			m_state = state::on_guard;
	}
	else if (m_state == state::on_guard)
	{
		face_player(time_step, player_position);

		// check whether the condition has been met to switch back to idle state
		if (distance_to_player > m_detection_radius)
			m_state = state::idle;

		// check whether the condition has been met to switch to the chasing state
		else if (distance_to_player < m_trigger_radius)
			m_state = state::chasing;
	}
	else
	{
		chase_player(time_step, player_position);

		// check whether the condition has been met to switch back to idle state
		if (distance_to_player > m_detection_radius)
			m_state = state::idle;
	}
}

// move forwards until the timer runs out, then switch direction to move the other way
void enemy::patrol(const engine::timestep& time_step)
{
	m_switch_direction_timer -= (float)time_step;
	if (m_switch_direction_timer < 0.f)
	{
		m_object->set_forward(m_object->forward() * -1.f);
		m_switch_direction_timer = m_default_time;
	}


	m_object->set_position(m_object->position() + m_object->forward() * m_speed *(float)time_step);
	m_object->set_rotation_axis(glm::vec3(0.f, 1.f, 0.f));
	m_object->set_rotation_amount(atan2(m_object->forward().x, m_object->forward().z));
}

// stop and look at the player
void enemy::face_player(const engine::timestep& time_step, const glm::vec3& player_position)
{
	m_object->set_forward(player_position - m_object->position());
	m_object->set_rotation_axis(glm::vec3(0.f, 1.f, 0.f));
	m_object->set_rotation_amount(atan2(m_object->forward().x, m_object->forward().z));
}

// move forwards in the direction of the player
void enemy::chase_player(const engine::timestep& time_step, const glm::vec3& player_position)
{
	m_object->set_forward(player_position - m_object->position());
	m_object->set_position(m_object->position() + m_object->forward() * m_speed *(float)time_step);
	m_object->set_rotation_axis(glm::vec3(0.f, 1.f, 0.f));
	m_object->set_rotation_amount(atan2(m_object->forward().x, m_object->forward().z));
}
