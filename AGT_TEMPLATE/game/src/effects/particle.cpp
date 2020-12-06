#include "pch.h"
#include "particle.h"

particle::particle()
{}

particle::~particle()
{}

void particle::on_update(const engine::timestep& timestep)
{
	// Position moving based on velocity (could implement more interesting physical equations here depending on the desired effect
	m_position += m_velocity*(float)timestep;

	// Rotation of image
	m_angle += m_angular_velocity* (float)timestep;

	// Life decreasing on each iteration
	m_life -= 0.1f * (float)timestep;

	// Size of the quad increasing on each iteration
	m_width += 0.1f  * (float)timestep;
	m_height += 0.1f * (float)timestep;

}


bool particle::initialise(glm::vec3 p, float width, float height, glm::vec3 start_colour, glm::vec3 end_colour, float transition_speed, float life)
{
	// Set the initial position and velocity
	m_position = p;

	// Give the particle some random velocity in the xz plane
	float r1 = (rand() / (float) RAND_MAX) * 2 - 1;
	float r2 = (rand() / (float) RAND_MAX) * 2 - 1;

	//make particles fall down as if affected by gravity
	m_velocity = glm::vec3(r1*0.1f, -3.f, r2*0.1f);
	glm::normalize(m_velocity);
	m_velocity *= 0.1f; 

	// Set the sprite colour sequence, width, height
	m_width = width;
	m_height = height;
	// Set the sprite image intial rotation angle and rotational velocity
	m_angle = 0.f;
	m_angular_velocity = 1.0f;

	// Set the particle life
	m_life = life;

	m_start_colour = start_colour;
	m_end_colour = end_colour;
	m_transition_speed = transition_speed;


	return true;
}


glm::mat4 particle::on_render(glm::vec3 look_at, glm::vec3 up_vector, engine::ref<engine::shader> shader)
{

	if (m_life <= 0.0f)
		return glm::mat4(1.f);

	// Billboarding -- form a quad at p in the plane orthogonal to the lookAt vector
	
	glm::vec3 orthogonal_vector = glm::cross(glm::normalize(look_at), up_vector);

	float angle = atan2(look_at.x, look_at.z);

	glm::mat4 transform(1.0f);
	transform = glm::translate(transform, m_position);
	transform = glm::rotate(transform, angle, glm::vec3(0.f, 1.f, 0.f));
	transform = glm::scale(transform, glm::vec3(m_width, m_height, 1.f));

	// Allow faster (m_transitionSpeed > 1) or slower transitions (0 < m_transitionSpeed < 1) between the two colours
	float f = pow(m_life, m_transition_speed);

	// Interpolate between the start and end colour
	glm::vec3 colour = (1 - f) * m_end_colour + f * m_start_colour;

	std::dynamic_pointer_cast<engine::gl_shader>(shader)->set_uniform("in_colour", colour);

	return transform;
}
