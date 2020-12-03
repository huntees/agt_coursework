#include "pch.h"
#include "screen_render.h"
#include "engine/entities/shapes/quad.h"

screen_render::screen_render(const std::string& path, float width, float height)
{
	m_texture = engine::texture_2d::create(path, true);
	m_quad = quad::create(glm::vec2(width, height));
	s_active = true;
}

screen_render::~screen_render()
{}

void screen_render::on_update(const engine::timestep& time_step)
{
	if (!s_active)
		return;


}

void screen_render::on_render(engine::ref<engine::shader> shader, float x, float y, float z)
{
	if (!s_active)
		return;

	glm::mat4 transform(1.0f);
	transform = glm::translate(transform, glm::vec3(x, y, z));

	m_texture->bind();

	engine::renderer::submit(shader, m_quad->mesh(), transform);

}

void screen_render::activate()
{
	s_active = true;
}

void screen_render::deactivate()
{
	s_active = false;
}

engine::ref<screen_render> screen_render::create(const std::string& path, float width, float height)
{
	return std::make_shared<screen_render>(path, width, height);
}
