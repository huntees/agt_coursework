#pragma once
#include <engine.h>

class quad;

class intro_screen
{
public:
	intro_screen(const std::string& path, float width, float height);
	~intro_screen();

	void on_update(const engine::timestep& time_step);
	void on_render(engine::ref<engine::shader> shader);
	void deactivate();

	static engine::ref<intro_screen> create(const std::string& path, float width, float height);

private:
	bool s_active;

	engine::ref<engine::texture_2d> m_texture;

	engine::ref<quad> m_quad;
};

