#pragma once
#include <engine.h>
#include "player.h"
#include "screen_render.h"
#include "engine/entities/bounding_box_bullet.h"
#include "effects/jetpack_trail.h"
#include "weapons/projectile.h"
#include "effects/explosion.h"
#include "effects/cross_fade.h"
#include "enemy/enemy.h"
#include "enemy/enemy_shooter.h"

class example_layer : public engine::layer
{
public:
	example_layer();
	~example_layer();

	void on_update(const engine::timestep& time_step) override;
	void on_render() override;
	void on_event(engine::event& event) override;

private:
	player m_player{};

	enemy_shooter m_enemy_droid{};
	enemy_shooter m_enemy_mech{};
	enemy m_enemy_drone{};
	enemy m_enemy_bb8{};

	void check_bounce();

	engine::ref<engine::skybox>			m_skybox{};
	engine::ref<engine::game_object>	m_terrain{};
	engine::ref<engine::game_object>	m_road{};
	engine::ref<engine::game_object>	m_intersection{};
	engine::ref<engine::game_object>	m_weapon_holder{};
	engine::ref<engine::game_object>	m_cow{};
	engine::ref<engine::game_object>	m_tree{};
	engine::ref<engine::game_object>	m_ball{};
	engine::ref<engine::game_object>	m_mannequin{};
	engine::ref<engine::game_object>	m_droid{};
	engine::ref<engine::game_object>	m_drone{};
	engine::ref<engine::game_object>	m_bb8{};
	engine::ref<engine::game_object>	m_mech{};
	engine::ref<engine::game_object>	m_heart{};
	engine::ref<engine::game_object>	m_shield{};
	engine::ref<engine::game_object>    m_tetrahedron{};
	engine::ref<engine::game_object>	m_jeep{};
	engine::ref<engine::game_object>	m_pizza{};
	engine::ref<engine::game_object>	m_jet{};
	engine::ref<engine::game_object>	m_office{};
	engine::ref<engine::game_object>	m_sanfran{};
	engine::ref<engine::game_object>	m_skyscrapers{};
	engine::ref<engine::game_object>	m_policeCar{};
	engine::ref<engine::game_object>    m_hexagon{};
	engine::ref<engine::game_object>    m_bench{};
	engine::ref<engine::game_object>    m_lamppost{};
	engine::ref<engine::game_object>    m_missile{};
	engine::ref<engine::game_object>    m_enemy_missile{};
	engine::ref<engine::game_object>    m_enemy_missile2{};
	engine::ref<engine::game_object>    m_bouncynade{};

	projectile missile;
	const int missile_damage = 10;
	projectile bouncynade;
	const int bouncynade_damage = 30;

	float heartRotation = 0.f;
	float heart_timer = 0.f;
	float heart_respawn_time = 5.f;

	float shieldRotation = 40.f;
	float shield_timer = 0.f;
	float shield_respawn_time = 10.f;

	enum class WeaponState { Slot1, Slot2, Slot3 };
	WeaponState WeaponSlot = WeaponState::Slot1;

	projectile enemy_missile;
	projectile enemy_missile2;

	bounding_box m_droid_box;
	bounding_box m_mech_box;
	bounding_box m_drone_box;
	bounding_box m_bb8_box;
	bounding_box m_heart_box;
	bounding_box m_shield_box;

	const float immune_time = 2.f;
	bool player_immunity = false;
	float player_immunity_timer = -1.f;

	bounding_box m_cow_box;

	engine::ref<engine::material>		m_material{};
	engine::ref<engine::material>		m_tetrahedron_material{};
	engine::ref<engine::material>		m_red_spotLight_material{};
	engine::ref<engine::material>		m_white_pointLight_material{};

	engine::DirectionalLight            m_directionalLight;

	engine::SpotLight m_red_spotLight;
	uint32_t num_spot_lights = 1;

	engine::ref<engine::game_object>	m_red_spotLight_ball{};
	float spotLightRotation = 0.f;

	engine::PointLight m_white_pointLight;
	uint32_t num_point_lights = 1;

	engine::ref<engine::game_object>    m_pointLight_object{};

	std::vector<engine::ref<engine::game_object>>     m_game_objects{};

	engine::ref<engine::bullet_manager> m_physics_manager{};
	engine::ref<engine::audio_manager>  m_audio_manager{};
	float								m_prev_sphere_y_vel = 0.f;
	engine::ref<engine::text_manager>	m_text_manager{};

	engine::orthographic_camera       m_2d_camera;
	engine::perspective_camera        m_3d_camera;

	engine::ref<screen_render>	m_intro_screen{};
	engine::ref<screen_render>	m_HUD{};
	engine::ref<screen_render>	m_HUD_range_finder{};
	engine::ref<screen_render>	m_HUD_jetpack{};
	engine::ref<screen_render>	m_HUD_missile{};
	engine::ref<screen_render>	m_HUD_bouncynade{};
	float range_finder_height = 0.f;

	enum class CamState { FirstPerson, ThirdPerson, FreeView };
	CamState CamMode = CamState::FirstPerson;

	bool jetpackTrailReady = true;
	float jetpackTrailTimer;
	bool jetpackHoverOn = false;

	bool playState = false;

	void damage_player(int damage);

	//===============================================================Effects============================================================================
	jetpack_trail m_jetpack_trail;

	engine::ref<cross_fade>							m_cross_fade{};

	engine::ref<explosion>							m_explosion{};
	//bool missile_active = false;
	//bool enemy_missile_active = false;
	//bool enemy_missile2_active = false;
	bool bouncynade_armed = true;
	float bouncynade_armtime = -1.f;
};
