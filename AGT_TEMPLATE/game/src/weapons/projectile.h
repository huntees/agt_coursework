#pragma once
#include "../enemy/enemy_shooter.h"
#include <engine.h>
#include "engine/entities/bounding_box_bullet.h"

class enemy_shooter;
class projectile
{
public:
	projectile();
	~projectile();

	void initialise(engine::ref<engine::game_object> object);
	void fire(const engine::perspective_camera& camera, float force, glm::vec3 playerPos);
	void enemy_fire(enemy_shooter& enemy, float force);
	void on_update(const engine::timestep& time_step);
	void on_render(const engine::ref<engine::shader>& shader);

	glm::vec3 last_position() const { return m_last_position; }

	engine::ref<engine::game_object> object() const { return m_object; }

	void set_box(float width, float height, float depth, glm::vec3 position) { m_projectile_box.set_box(width, height, depth, position); }
	bounding_box& getBox() { return m_projectile_box; };

private:
	engine::ref<engine::game_object> m_object;

	glm::vec3 m_instantaneous_acceleration{ 0.f };
	glm::vec3 m_instantaneous_angular_acceleration{ 0.f };

	float m_contact_time = 0.0f;

	float m_rotational_inertia = 0.f;

	float m_phi, m_theta;

	glm::vec3 m_last_position{ 0.f };

	// Methods to handle collisions with the ground
	bool collision_detection(float y_plane);
	void collision_response(float y_plane);

	bounding_box m_projectile_box;
};
