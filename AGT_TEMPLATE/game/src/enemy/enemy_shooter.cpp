#include "pch.h"
#include "enemy_shooter.h"
#include <iostream>
enemy_shooter::enemy_shooter() {}

enemy_shooter::~enemy_shooter() {}

void enemy_shooter::initialise(engine::ref<engine::game_object> object, glm::vec3 position, glm::vec3 forward, bool canFly, bool istank)
{
	m_object = object;
	m_object->set_forward(forward);
	m_object->set_position(position);
	can_fly = canFly;
	is_tank = istank;

	if (is_tank) {
		shoot_speed = tank_shoot_speed;
	}
}

void enemy_shooter::on_update(const engine::timestep& time_step, const glm::vec3& player_position, projectile& missile)
{
	float distance_to_player = glm::distance(m_object->position(), player_position);

	if (can_fly) {
		//m_object->set_acceleration(glm::vec3(m_object->acceleration().x, 9.8f, m_object->acceleration().z));
		//m_object->set_velocity(glm::vec3(m_object->velocity().x, 0.f, m_object->velocity().z));
		m_object->set_velocity(glm::vec3(0.f));
		m_object->set_acceleration(glm::vec3(0.f));
	}

	// check which state is the enemy_shooter in, then execute the matching behaviour
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
		if (distance_to_player > m_detection_radius) {
			m_state = state::idle;
		}

		//shoot player if they get into the radius
		else if (distance_to_player < m_shoot_radius) {
			m_state = state::shooting;
		}

	}
	else if (m_state == state::shooting)
	{
		face_player(time_step, player_position);
		shoot_player(time_step, missile, 1.f);

		// keep up with the player to keep them in range of shooting
		if (distance_to_player > m_shoot_radius) {
			m_state = state::chasing;
		}

		// retreat to a safer distance if the player get too close while shooting slower *only if it is a regular unit*
		else if (distance_to_player < m_retreat_radius && !is_tank) {
			m_state = state::retreat;
		}

		// chases the player alot faster to melee them
		else if (distance_to_player < m_melee_radius) {
			m_state = state::melee;
		}
	}
	else if (m_state == state::melee)
	{
		chase_player(time_step, player_position, 1.5f);

		// if player is out of range for melee, start shooting
		if (distance_to_player > m_melee_radius) {
			m_state = state::shooting;
		}
	}
	else if (m_state == state::retreat) {

		retreat(time_step, player_position);
		shoot_player(time_step, missile, 3.f);

		//once in a safe distance, begin shooting faster while standing still
		if (distance_to_player > m_retreat_radius + 3.f) {
			m_state = state::shooting;
		}
		
	}
	else
	{
		chase_player(time_step, player_position, 1.f);

		// check whether the condition has been met to switch back to idle state
		if (distance_to_player < m_shoot_radius) {
			m_state = state::shooting;
		}

		else if (distance_to_player > m_detection_radius + 5.f) {
			m_state = state::idle;
		}
	}

	//shoot
	//retreat
}

// move forwards until the timer runs out, then switch direction to move the other way
void enemy_shooter::patrol(const engine::timestep& time_step)
{
	m_switch_direction_timer -= (float)time_step;
	if (m_switch_direction_timer < 0.f)
	if (m_switch_direction_timer < 0.f)
	{
		m_object->set_forward(m_object->forward() * -1.f);
		m_switch_direction_timer = m_default_time;
	}


	m_object->set_position(m_object->position() + m_object->forward() * m_speed * (float)time_step);
	m_object->set_rotation_axis(glm::vec3(0.f, 1.f, 0.f));
	m_object->set_rotation_amount(atan2(m_object->forward().x, m_object->forward().z));
}

// stop and look at the player
void enemy_shooter::face_player(const engine::timestep& time_step, const glm::vec3& player_position)
{
	//normalizing so when enemy shoots, the front vector isnt somewhere far away from the model
	m_object->set_forward(glm::normalize(player_position - m_object->position()));
	m_object->set_rotation_axis(glm::vec3(0.f, 1.f, 0.f));
	m_object->set_rotation_amount(atan2(m_object->forward().x, m_object->forward().z));

}

// move forwards in the direction of the player
void enemy_shooter::chase_player(const engine::timestep& time_step, const glm::vec3& player_position, float speed_mul)
{
	//normalizing so when enemy shoots, the front vector isnt somewhere far away from the model
	m_object->set_forward(glm::normalize(player_position - m_object->position()));
	m_object->set_position(m_object->position() + m_object->forward() * (m_speed * speed_mul) * (float)time_step);
	m_object->set_rotation_axis(glm::vec3(0.f, 1.f, 0.f));
	m_object->set_rotation_amount(atan2(m_object->forward().x, m_object->forward().z));
}

void enemy_shooter::shoot_player(const engine::timestep& time_step, projectile& missile, float mod)
{
	if (!enemy_shot) {
		missile.enemy_fire(*this, 180.0f);
		missile.set_active(true);

		enemy_shot = true;
		shoot_interval = shoot_speed * mod;
	}

	if (shoot_interval > 0.0f)
	{
		
		shoot_interval -= (float)time_step;

		if (shoot_interval < 0.0f)
		{
			enemy_shot = false;
		}
	}
}

void enemy_shooter::retreat(const engine::timestep& time_step, const glm::vec3& player_position)
{
	//normalizing so when enemy shoots, the front vector isnt somewhere far away from the model
	m_object->set_forward(glm::normalize(player_position - m_object->position()));
	m_object->set_position(m_object->position() + m_object->forward() * (-m_speed * 2.f)* (float)time_step);
	m_object->set_rotation_axis(glm::vec3(0.f, 1.f, 0.f));
	m_object->set_rotation_amount(atan2(m_object->forward().x, m_object->forward().z));
}
