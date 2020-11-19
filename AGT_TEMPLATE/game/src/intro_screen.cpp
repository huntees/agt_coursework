#include "pch.h"
#include "intro_screen.h"
#include "engine/entities/shapes/quad.h"

intro_screen::intro_screen(const std::string& path, float width, float height)
{
	m_texture = engine::texture_2d::create(path, true);
	m_quad = quad::create(glm::vec2(width, height));
	s_active = true;
}

intro_screen::~intro_screen()
{}

void intro_screen::on_update(const engine::timestep& time_step)
{
	if (!s_active)
		return;


}

void intro_screen::on_render(engine::ref<engine::shader> shader)
{
	if (!s_active)
		return;

	glm::mat4 transform(1.0f);
	transform = glm::translate(transform, glm::vec3(0.f, 0.f, 0.1f));

	m_texture->bind();

	engine::renderer::submit(shader, m_quad->mesh(), transform);

}

void intro_screen::deactivate()
{
	s_active = false;
}

engine::ref<intro_screen> intro_screen::create(const std::string& path, float width, float height)
{
	return std::make_shared<intro_screen>(path, width, height);
}
