#pragma once
#include <engine.h>
#include "../weapons/projectile.h"

class projectile;
class enemy_shooter
{
	enum class state
	{
		idle,
		on_guard,
		shooting,
		melee,
		retreat,
		chasing
	};

public:
	enemy_shooter();
	~enemy_shooter();

	// set the parameters for the enemy_shooter
	void initialise(engine::ref<engine::game_object> object, glm::vec3 position, glm::vec3 forward, bool can_fly, bool istank);

	// update the enemy_shooter
	void on_update(const engine::timestep& time_step, const glm::vec3& player_position, projectile& p);

	// methods controlling the enemy_shooter’s behaviour in a certain state
	void patrol(const engine::timestep& time_step);

	void face_player(const engine::timestep& time_step, const glm::vec3& player_position);

	void chase_player(const engine::timestep& time_step, const glm::vec3& player_position, float speed_mul);

	void shoot_player(const engine::timestep& time_step, projectile& p, float shoot_speed);

	void retreat(const engine::timestep& time_step, const glm::vec3& player_position);

	// game object bound to the enemy_shooter
	engine::ref<engine::game_object> object() const { return m_object; }


private:

	const float tank_shoot_speed = 2.f;
	const float regular_shoot_speed = 1.3f;

	bool is_tank = false;

	// enemy_shooter's speed
	float m_speed{ 1.f };

	// timer controlling the direction switch and the reset value for this timer
	float m_default_time{ 6.f };
	float m_switch_direction_timer = m_default_time;

	// threshold distances
	float m_detection_radius{ 13.f };
	float m_shoot_radius{ 10.f };
	float m_melee_radius{ 3.f };
	float m_retreat_radius{ 5.f };
	

	// game object bound to the enemy_shooter
	engine::ref< engine::game_object> m_object;

	//current state of the enemy_shooter's state machine
	state m_state = state::idle;

	bool can_fly = false;

	float shoot_speed = regular_shoot_speed;
	float shoot_interval = -1.f;
	bool enemy_shot = false;
};