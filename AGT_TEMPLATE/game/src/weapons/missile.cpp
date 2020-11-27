#include "pch.h"
#include "missile.h"

missile::missile()
{}

missile::~missile()
{}

void missile::initialise(engine::ref<engine::game_object> object)
{
	m_object = object;
}

void missile::fire(const engine::perspective_camera& camera, float force, glm::vec3 playerPos)
{
	m_object->set_velocity(glm::vec3(0.f));
	m_object->set_acceleration(glm::vec3(0.f, 0.f, 0.f));
	m_object->set_torque(glm::vec3(0.f));
	m_object->set_angular_velocity(glm::vec3(0.f));
	m_contact_time = 0.0f;

	// Set the ball to the current camera position
	m_object->set_position(playerPos + glm::vec3(0.f, 0.45f, 0.f) + (camera.front_vector() * 0.5f));
	//m_object->set_velocity(25.0f * camera.front_vector());
	m_object->set_acceleration(3.0f * force * camera.front_vector());

	// Determine rotation angles of camera (from Lab 4)
	m_theta = engine::PI / 2.f - acos(camera.front_vector().y);
	m_phi = atan2(camera.front_vector().x, camera.front_vector().z);

	m_object->set_rotation_axis(glm::vec3(0.f, 1.f, 0.f));
	m_object->set_rotation_amount(m_phi);
}

void missile::on_update(const engine::timestep& time_step)
{
	// Update physical quanitities
	m_last_position = m_object->position();

	//m_object->set_position(m_object->position()+ m_object->velocity() * (float)time_step);

	//glm::vec3 angle = m_object->rotation_axis() * m_object->rotation_amount();
	//angle += m_object->angular_velocity() * (float)time_step;
	//m_object->set_rotation_amount(glm::length(angle));
	//if (glm::length(angle)>0.f)
	//{
	//	m_object->set_rotation_axis(glm::normalize(angle));
	//}

	//// Turn off instantaneous forces if contact time is surpassed
	//if (glm::length(m_instantaneous_acceleration) > 0 && m_contact_time > 0.05) {
	//	m_instantaneous_acceleration = glm::vec3(0.f);
	//	m_instantaneous_angular_acceleration = glm::vec3(0.f);
	//	m_contact_time = 0.f;
	//}
	//m_contact_time += time_step;

	//// Check for collision detection
	//float y_plane = 0.5f;
	//if (collision_detection(y_plane)) {
	//	collision_response(y_plane);
	//}

	m_missile_box.on_update(object()->position() - glm::vec3(0.f, object()->offset().y,
		0.f) * object()->scale(), object()->rotation_amount(), object()->rotation_axis());
}

void missile::on_render(const engine::ref<engine::shader>& shader)
{
	glm::mat4 transform(1.0f);
	transform = glm::translate(transform, m_object->position());
	//transform = glm::rotate(transform, m_theta, glm::vec3(1.f, 0.f, 0.f));
	//transform = glm::rotate(transform, m_phi, glm::vec3(0.f, 1.f, 0.f));
	transform = glm::rotate(transform, m_object->rotation_amount(), m_object->rotation_axis());
	engine::renderer::submit(shader, transform, m_object);
}

bool missile::collision_detection(float y_plane)
{
	// Check for collision with the ground by looking at the y value of the ball's position
	if (m_object->position().y - m_object->bounding_shape().y < y_plane && m_object->velocity().y < 0) {
		return true;
	}
	return false;
}

void missile::collision_response(float y_plane)
{
	float convergenceThreshold = 0.5f;
	if (glm::length(m_object->velocity()) > convergenceThreshold) {
		// The ball has bounced!  Implement a bounce by flipping the y velocity
		m_object->set_velocity(glm::vec3(m_object->velocity().x, -m_object->velocity().y, m_object->velocity().z));
	}
	else {
		// Velocity of the ball is below a threshold.  Stop the ball. 
		m_object->set_velocity(glm::vec3(0.0f, 0.0f, 0.0f));
		m_object->set_acceleration(glm::vec3(0.0f, 0.0f, 0.0f));
		m_object->set_position(glm::vec3(m_object->position().x, m_object->bounding_shape().y + y_plane, m_object->position().z));
		m_object->set_angular_velocity(glm::vec3(0.0f, 0.0f, 0.0f));
	}
}
