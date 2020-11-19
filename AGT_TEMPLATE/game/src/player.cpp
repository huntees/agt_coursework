#include "pch.h"
#include "player.h"
#include "engine/core/input.h"
#include "engine/key_codes.h"


player::player() : m_timer(0.0f), m_speed(1.0f)
{}

player::~player()
{}

void player::initialise(engine::ref<engine::game_object> object)
{
	m_object = object;
	m_object->set_forward(glm::vec3(0.f, 0.f, -1.f));
	m_object->set_position(glm::vec3(0.f, 0.5, 10.f));
	m_object->animated_mesh()->set_default_animation(1);
}

void player::on_update(const engine::timestep& time_step)
{

	//hold right click to rotate around player
	rotateAround = false;
	if (engine::input::mouse_button_pressed(1)) {
		rotateAround = true;
	}

	if ((rotateAround && !isFirstPerson) ) {
		
	}
	else {
		m_object->set_forward(player_front);
	}


	//WASD Movement
	//m_object->set_position(m_object->position() += m_object->forward() * m_speed * (float)time_step);

	if (engine::input::key_pressed(engine::key_codes::KEY_W)) {
		m_object->set_position(m_object->position() += player_front * m_speed * (float)time_step);
	}
	else if (engine::input::key_pressed(engine::key_codes::KEY_S)) {
		m_object->set_position(m_object->position() -= player_front * m_speed * (float)time_step);
	}
	if (engine::input::key_pressed(engine::key_codes::KEY_A)) {
		m_object->set_position(m_object->position() -= player_right * m_speed * (float)time_step);
	}
	else if (engine::input::key_pressed(engine::key_codes::KEY_D)) {
		m_object->set_position(m_object->position() += player_right * m_speed * (float)time_step);
	}

	//third person movement?
	//if (engine::input::key_pressed(engine::key_codes::KEY_W)) {
	//	m_object->set_position(m_object->position() += glm::vec3(0.f, 0.f, -m_speed * (float)time_step));
	//}
	//else if (engine::input::key_pressed(engine::key_codes::KEY_S)) {
	//	m_object->set_position(m_object->position() -= glm::vec3(0.f, 0.f, m_speed * (float)time_step));
	//}
	//if (engine::input::key_pressed(engine::key_codes::KEY_A)) {
	//	m_object->set_position(m_object->position() += glm::vec3(-m_speed * (float)time_step, 0.f, 0.f));
	//}
	//else if (engine::input::key_pressed(engine::key_codes::KEY_D)) {
	//	m_object->set_position(m_object->position() += glm::vec3(m_speed * (float)time_step, 0.f, 0.f));
	//}

	//JUMP to be implemented down the line


	m_object->set_rotation_amount(atan2(m_object->forward().x, m_object->forward().z));

	if (engine::input::key_pressed(engine::key_codes::KEY_1)) {
		turn(1.0f * time_step);
	}
	else if (engine::input::key_pressed(engine::key_codes::KEY_2)) {
		turn(-1.0f * time_step);
	}

	//if (engine::input::key_pressed(engine::key_codes::KEY_SPACE)) {
	//	jump();
	//}

	if (m_timer > 0.0f)
	{
		m_timer -= (float)time_step;

		if (m_timer < 0.0f)
		{
			m_object->animated_mesh()->switch_root_movement(false);
			m_object->animated_mesh()->switch_animation(m_object->animated_mesh() -> default_animation());
		}
	}

	m_object->animated_mesh()->on_update(time_step * animation_speed);
}

void player::turn(float angle) {
	m_object->set_forward(glm::rotate(m_object->forward(), angle, glm::vec3(0.f, 1.f, 0.f)));
}

void player::update_first_person_camera(engine::perspective_camera& camera) {

	isFirstPerson = true;

	glm::vec3 mousepos = process_mouse();
	
	float A = 0.95f;
	float B = 0.1f;

	glm::vec3 cam_pos = m_object->position() + glm::normalize(m_object->forward()) * B;;
	cam_pos.y += A;

	camera.set_view_matrix(cam_pos, mousepos, mousepos);


}

void player::update_third_person_camera(engine::perspective_camera& camera) {

	isFirstPerson = false;

	glm::vec3 mousepos = process_mouse();
	mousepos.x *= 2.f;
	mousepos.y *= 2.f;
	mousepos.z *= 2.f;
	 
	glm::vec3 cam_pos = m_object->position() - mousepos;
	cam_pos.y += 1.5f;

	glm::vec3 cam_look_at = m_object->position();
	cam_look_at.y = 1.5f;

	camera.set_view_matrix(cam_pos, cam_look_at);
}

glm::vec3 player::process_mouse() {

	//from lab
	auto [mouse_delta_x, mouse_delta_y] = engine::input::mouse_position();
	mouse_delta_x *= 0.1f;
	mouse_delta_y *= 0.1f;

	m_yaw += mouse_delta_x;
	m_pitch += mouse_delta_y;

	static const float pitch_limit = 89.0f;
	if (m_pitch > pitch_limit) {
		m_pitch = pitch_limit;
	}
	if (m_pitch < -pitch_limit) {
		m_pitch = -pitch_limit;
	}

	glm::vec3 mousepos(0.f);
	float yaw_radians = glm::radians(m_yaw);
	float pitch_radians = glm::radians(m_pitch);
	mousepos.x = cos(yaw_radians) * cos(pitch_radians);
	mousepos.y = sin(pitch_radians);
	mousepos.z = sin(yaw_radians) * cos(pitch_radians);

	//for player positional usage
	player_front = mousepos;
	//prevents from going up by camera direct
	player_front.y = 0.f;

	player_front = glm::normalize(player_front);

	//// Also re-calculate the Right and Up vector
	//// Normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
	player_right = glm::normalize(glm::cross(player_front, glm::vec3(0.f,1.f,0.f)));
	player_up = glm::normalize(glm::cross(player_right, player_front));

	//player_front = mousepos;
	return mousepos;
}

void player::jump() {

	m_object->animated_mesh()->switch_root_movement(true);
	m_object->animated_mesh()->switch_animation(3);
	animation_speed = 1.5f;

	m_timer = (m_object->animated_mesh()->animations().at(3)->mDuration * 0.5f);

}

void player::sprint(const bool& activateSprint) {

	if (activateSprint) {
		m_speed = 2.0f;
		animation_speed = 1.5f;
	}
	else {

		m_speed = 1.0f;
		animation_speed = 1.f;
	}


}
