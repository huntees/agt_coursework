#pragma once
#include <engine.h>
#include "player.h"
#include "intro_screen.h"

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

	void check_bounce();

	engine::ref<engine::skybox>			m_skybox{};
	engine::ref<engine::game_object>	m_terrain{};
	engine::ref<engine::game_object>	m_cow{};
	engine::ref<engine::game_object>	m_tree{};
	engine::ref<engine::game_object>	m_ball{};
	engine::ref<engine::game_object>	m_mannequin{};
	engine::ref<engine::game_object>    m_tetrahedron{};
	engine::ref<engine::game_object>	m_jeep{};
	engine::ref<engine::game_object>	m_pizza{};
	engine::ref<engine::game_object>	m_jet{};
	engine::ref<engine::game_object>	m_office{};
	engine::ref<engine::game_object>	m_sanfran{};
	engine::ref<engine::game_object>	m_skyscrapers{};
	engine::ref<engine::game_object>    m_hexagon{};
	engine::ref<engine::game_object>    m_bench{};

	engine::ref<engine::material>		m_material{};
	engine::ref<engine::material>		m_tetrahedron_material{};

	engine::DirectionalLight            m_directionalLight;

	std::vector<engine::ref<engine::game_object>>     m_game_objects{};

	engine::ref<engine::bullet_manager> m_physics_manager{};
	engine::ref<engine::audio_manager>  m_audio_manager{};
	float								m_prev_sphere_y_vel = 0.f;
	engine::ref<engine::text_manager>	m_text_manager{};

	engine::orthographic_camera       m_2d_camera;
	engine::perspective_camera        m_3d_camera;

	engine::ref<intro_screen>	m_intro_screen{};

	enum CamState { FirstPerson, ThirdPerson, FreeView };
	CamState CamMode = FirstPerson;

	bool camSwitchDelayReady = true;
	float camSwitchTimer;

	bool playState = false;
};
