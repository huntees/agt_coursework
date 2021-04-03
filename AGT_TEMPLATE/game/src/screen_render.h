#pragma once
#include <engine.h>

class quad;

class screen_render
{
public:
	screen_render(const std::string& path, float width, float height);
	~screen_render();

	void on_update(const engine::timestep& time_step);
	void on_render(engine::ref<engine::shader> shader, float x, float y, float z);
	void activate();
	void deactivate();

	static engine::ref<screen_render> create(const std::string& path, float width, float height);

private:
	bool s_active;

	engine::ref<engine::texture_2d> m_texture;

	engine::ref<quad> m_quad;
};

