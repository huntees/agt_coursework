#include "example_layer.h"
#include "platform/opengl/gl_shader.h"

#include <glm/gtc/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale, glm::perspective
#include <glm/gtc/type_ptr.hpp>
#include "engine/events/key_event.h"
#include "engine/utils/track.h"

#include "engine/core/input.h"
#include "engine/key_codes.h"

example_layer::example_layer()
	:m_2d_camera(-1.6f, 1.6f, -0.9f, 0.9f),
	m_3d_camera((float)engine::application::window().width(), (float)engine::application::window().height())


{

	// Hide the mouse and lock it inside the window
	//engine::input::anchor_mouse(true);
	engine::application::window().hide_mouse_cursor();

	// Initialise audio and play background music
	m_audio_manager = engine::audio_manager::instance();
	m_audio_manager->init();
	m_audio_manager->load_sound("assets/audio/bounce.wav", engine::sound_type::event, "bounce"); // Royalty free sound from freesound.org
	m_audio_manager->load_sound("assets/audio/DST-impuretechnology.mp3", engine::sound_type::track, "music");  // Royalty free music from http://www.nosoapradio.us/
	//m_audio_manager->play("music");


	// Initialise the shaders, materials and lights
	auto mesh__material_shader = engine::renderer::shaders_library()->get("mesh_material");
	auto mesh_lighting_shader = engine::renderer::shaders_library()->get("mesh_lighting");
	auto text_shader = engine::renderer::shaders_library()->get("text_2D");
	auto animated_mesh_shader = engine::renderer::shaders_library()->get("animated_mesh");

	m_directionalLight.Color = glm::vec3(0.8f, 0.8f, 0.8f);
	m_directionalLight.AmbientIntensity = 0.25f;
	m_directionalLight.DiffuseIntensity = 0.6f;
	m_directionalLight.Direction = glm::normalize(glm::vec3(1.0f, -1.0f, 0.0f));

	m_white_pointLight.Color = glm::vec3(1.0f, 1.0f, 1.0f);
	m_white_pointLight.AmbientIntensity = 0.3f;
	m_white_pointLight.DiffuseIntensity = 0.6f;
	m_white_pointLight.Position = glm::vec3(10.1f, 4.5f, 8.12f);
	m_white_pointLight.Attenuation.Constant = 1.0f;
	m_white_pointLight.Attenuation.Linear = 0.4f;
	m_white_pointLight.Attenuation.Exp = 0.01f;

	m_red_spotLight.Color = glm::vec3(1.0f, 0.0f, 0.0f);
	m_red_spotLight.AmbientIntensity = 0.8f; //0.25f
	m_red_spotLight.DiffuseIntensity = 0.6f;
	m_red_spotLight.Position = glm::vec3(9.47f, 1.8f, 11.94f);
	m_red_spotLight.Direction = glm::normalize(glm::vec3(1.f, 0.f, 0.f));
	m_red_spotLight.Cutoff = 0.5f;
	m_red_spotLight.Attenuation.Constant = 1.0f;
	m_red_spotLight.Attenuation.Linear = 0.4f;
	m_red_spotLight.Attenuation.Exp = 0.01f;

	// set color texture unit
	std::dynamic_pointer_cast<engine::gl_shader>(animated_mesh_shader)->bind();
	std::dynamic_pointer_cast<engine::gl_shader>(animated_mesh_shader)->set_uniform("gColorMap", 0);
	std::dynamic_pointer_cast<engine::gl_shader>(animated_mesh_shader)->set_uniform("lighting_on", true);
	m_directionalLight.submit(animated_mesh_shader);
	std::dynamic_pointer_cast<engine::gl_shader>(animated_mesh_shader)->set_uniform("gMatSpecularIntensity", 0.5f);
	std::dynamic_pointer_cast<engine::gl_shader>(animated_mesh_shader)->set_uniform("gSpecularPower", 5.f);
	std::dynamic_pointer_cast<engine::gl_shader>(animated_mesh_shader)->set_uniform("transparency", 1.0f);
	std::dynamic_pointer_cast<engine::gl_shader>(mesh__material_shader)->bind();
	std::dynamic_pointer_cast<engine::gl_shader>(mesh__material_shader)->set_uniform("lighting_on", true);
	m_directionalLight.submit(mesh__material_shader);
	std::dynamic_pointer_cast<engine::gl_shader>(mesh__material_shader)->set_uniform("gMatSpecularIntensity", 1.f);
	std::dynamic_pointer_cast<engine::gl_shader>(mesh__material_shader)->set_uniform("gSpecularPower", 10.f);
	std::dynamic_pointer_cast<engine::gl_shader>(mesh_lighting_shader)->bind();
	std::dynamic_pointer_cast<engine::gl_shader>(mesh_lighting_shader)->set_uniform("gColorMap", 0);
	std::dynamic_pointer_cast<engine::gl_shader>(mesh_lighting_shader)->set_uniform("lighting_on", true);
	m_directionalLight.submit(mesh_lighting_shader);
	std::dynamic_pointer_cast<engine::gl_shader>(mesh_lighting_shader)->set_uniform("gMatSpecularIntensity", 0.5f);
	std::dynamic_pointer_cast<engine::gl_shader>(mesh_lighting_shader)->set_uniform("gSpecularPower", 5.f);
	std::dynamic_pointer_cast<engine::gl_shader>(mesh_lighting_shader)->set_uniform("transparency", 1.0f);
	std::dynamic_pointer_cast<engine::gl_shader>(text_shader)->bind();
	std::dynamic_pointer_cast<engine::gl_shader>(text_shader)->set_uniform("projection",
		glm::ortho(0.f, (float)engine::application::window().width(), 0.f,
			(float)engine::application::window().height()));
	m_material = engine::material::create(1.0f, glm::vec3(1.0f, 0.1f, 0.07f),
		glm::vec3(1.0f, 0.1f, 0.07f), glm::vec3(0.5f, 0.5f, 0.5f), 1.0f);

	m_red_spotLight_material = engine::material::create(1.0f, m_red_spotLight.Color, m_red_spotLight.Color, m_red_spotLight.Color, 1.0f);

	m_white_pointLight_material = engine::material::create(1.0f, m_white_pointLight.Color, m_white_pointLight.Color, m_white_pointLight.Color, 1.0f);


	// Skybox texture from http://www.vwall.it/wp-content/plugins/canvasio3dpro/inc/resource/cubeMaps/
	m_skybox = engine::skybox::create(100.f,
		{ engine::texture_2d::create("assets/textures/skybox/posz.jpg", true),
		  engine::texture_2d::create("assets/textures/skybox/posx.jpg", true),
		  engine::texture_2d::create("assets/textures/skybox/negz.jpg", true),
		  engine::texture_2d::create("assets/textures/skybox/negx.jpg", true),
		  engine::texture_2d::create("assets/textures/skybox/posy.jpg", true),
		  engine::texture_2d::create("assets/textures/skybox/negy.jpg", true)
		});



	engine::ref<engine::skinned_mesh> m_skinned_mesh = engine::skinned_mesh::create("assets/models/animated/mannequin/free3Dmodel.dae");
	m_skinned_mesh->LoadAnimationFile("assets/models/animated/mannequin/walking.dae");
	m_skinned_mesh->LoadAnimationFile("assets/models/animated/mannequin/idle.dae");
	m_skinned_mesh->LoadAnimationFile("assets/models/animated/mannequin/jump.dae");
	m_skinned_mesh->LoadAnimationFile("assets/models/animated/mannequin/standard_run.dae");
	m_skinned_mesh->switch_root_movement(false);

	engine::game_object_properties mannequin_props;
	mannequin_props.animated_mesh = m_skinned_mesh;
	mannequin_props.scale = glm::vec3(1.f / glm::max(m_skinned_mesh->size().x, glm::max(m_skinned_mesh->size().y, m_skinned_mesh->size().z)));
	mannequin_props.position = glm::vec3(3.0f, 1.5f, -5.0f);
	mannequin_props.rotation_amount = engine::PI;
	mannequin_props.rotation_axis = glm::vec3(0.f, 1.f, 0.f);
	mannequin_props.type = 0;
	mannequin_props.bounding_shape = glm::vec3(m_skinned_mesh->size().x / 5.f,
		m_skinned_mesh->size().y / 2.f, m_skinned_mesh->size().x / 5.f);
	mannequin_props.friction = 0.0f;
	m_mannequin = engine::game_object::create(mannequin_props);
	m_mannequin->set_offset(m_skinned_mesh->offset());
	m_player.initialise(m_mannequin);
	m_player.set_box(mannequin_props.bounding_shape.x * 2.5f * mannequin_props.scale.x, mannequin_props.bounding_shape.y * 2.f *
		mannequin_props.scale.x, mannequin_props.bounding_shape.z * 2.5f * mannequin_props.scale.x, mannequin_props.position);

	// Load the terrain texture and create a terrain mesh. Create a terrain object. Set its properties
	std::vector<engine::ref<engine::texture_2d>> terrain_textures = { engine::texture_2d::create("assets/textures/Slabs.jpg", false) };
	engine::ref<engine::terrain> terrain_shape = engine::terrain::create(100.f, 0.5f, 100.f, 150.f);
	engine::game_object_properties terrain_props;
	terrain_props.meshes = { terrain_shape->mesh() };
	terrain_props.textures = terrain_textures;
	terrain_props.is_static = true;
	terrain_props.type = 0;
	terrain_props.bounding_shape = glm::vec3(100.f, 0.5f, 100.f);
	terrain_props.restitution = 0.92f;
	m_terrain = engine::game_object::create(terrain_props);

	//weapon holder
	std::vector<engine::ref<engine::texture_2d>> weapon_holder_textures = { engine::texture_2d::create("assets/textures/intersection.jpg", false) };
	engine::ref<engine::terrain> weapon_holder_shape = engine::terrain::create(10.f, 0.1f, 10.f, 1.f);
	engine::game_object_properties weapon_holder_props;
	weapon_holder_props.meshes = { weapon_holder_shape->mesh() };
	weapon_holder_props.textures = weapon_holder_textures;
	weapon_holder_props.position = { 0.f, -10.f, 0.f };
	weapon_holder_props.is_static = true;
	weapon_holder_props.type = 0;
	weapon_holder_props.bounding_shape = glm::vec3(10.f, 0.1f, 10.f);
	weapon_holder_props.restitution = 0.f;
	m_weapon_holder = engine::game_object::create(weapon_holder_props);

	//===============================================================Weapon Props============================================================================
	engine::ref <engine::model> missile_model = engine::model::create("assets/models/static/missile/missile.obj");
	engine::game_object_properties missile_props;
	missile_props.meshes = missile_model->meshes();
	missile_props.textures = missile_model->textures();
	missile_props.position = { -9.f, -9.f, 9.f };
	missile_props.type = 0;
	missile_props.bounding_shape = missile_model->size() / 2.f;
	missile_props.friction = 0.0f;
	missile_props.mass = 0.10f;
	m_missile = engine::game_object::create(missile_props);
	m_missile->set_offset(missile_model->offset());
	missile.initialise(m_missile);
	missile.set_box(missile_props.bounding_shape.x * 2.f, missile_props.bounding_shape.y * 2.f, missile_props.bounding_shape.z * 2.f,
		missile_props.position - glm::vec3(0.f, m_missile->offset().y, 0.f));

	missile_props.position = { -7.f, -9.f, 9.f };
	m_enemy_missile = engine::game_object::create(missile_props);
	m_enemy_missile->set_offset(missile_model->offset());
	enemy_missile.initialise(m_enemy_missile);
	enemy_missile.set_box(missile_props.bounding_shape.x * 2.f, missile_props.bounding_shape.y * 2.f, missile_props.bounding_shape.z * 2.f,
		missile_props.position - glm::vec3(0.f, m_enemy_missile->offset().y, 0.f));

	missile_props.position = { -6.f, -9.f, 9.f };
	m_enemy_missile2 = engine::game_object::create(missile_props);
	m_enemy_missile2->set_offset(missile_model->offset());
	enemy_missile2.initialise(m_enemy_missile2);
	enemy_missile2.set_box(missile_props.bounding_shape.x * 2.f, missile_props.bounding_shape.y * 2.f, missile_props.bounding_shape.z * 2.f,
		missile_props.position - glm::vec3(0.f, m_enemy_missile->offset().y, 0.f));

	// Load bouncynade
	float bouncynade_radius = 0.15f;
	engine::ref<engine::sphere> bouncynade_shape = engine::sphere::create(5, 10, bouncynade_radius);
	engine::game_object_properties bouncynade_props;
	bouncynade_props.position = { -8.f, -9.f, 9.f };
	bouncynade_props.meshes = { bouncynade_shape->mesh() };
	bouncynade_props.type = 1;
	bouncynade_props.bounding_shape = glm::vec3(bouncynade_radius);
	bouncynade_props.restitution = 0.8f;
	bouncynade_props.mass = 0.2f;
	bouncynade_props.rolling_friction = 0.1f;
	m_bouncynade = engine::game_object::create(bouncynade_props);
	bouncynade.initialise(m_bouncynade);


	//===============================================================Weapon Props End==========================================================================

	//===============================================================World Props=========================================================================

	//road
	std::vector<engine::ref<engine::texture_2d>> road_textures = { engine::texture_2d::create("assets/textures/road.png", false) };
	engine::ref<engine::terrain> road_shape = engine::terrain::create(8.f, 0.02f, 25.f, 1.f);
	engine::game_object_properties road_props;
	road_props.meshes = { road_shape->mesh() };
	road_props.textures = road_textures;
	road_props.position = { 3.5f, 0.50001f, -3.f };
	road_props.is_static = true;
	road_props.type = 0;
	road_props.restitution = 0.92f;
	m_road = engine::game_object::create(road_props);

	//intersection
	std::vector<engine::ref<engine::texture_2d>> intersection_textures = { engine::texture_2d::create("assets/textures/intersection.jpg", false) };
	engine::ref<engine::terrain> intersection_shape = engine::terrain::create(8.f, 0.02f, 8.f, 1.f);
	engine::game_object_properties intersection_props;
	intersection_props.meshes = { intersection_shape->mesh() };
	intersection_props.textures = intersection_textures;
	intersection_props.position = { 3.5f, 0.50001f, 30.f };
	intersection_props.is_static = true;
	intersection_props.type = 0;
	intersection_props.restitution = 0.92f;
	m_intersection = engine::game_object::create(intersection_props);

	//pointLight_object
	engine::ref<engine::terrain> pointLight_object_shape = engine::terrain::create(0.3f, 0.05f, 0.1f, 1.f);
	engine::game_object_properties pointLight_object_props;
	pointLight_object_props.meshes = { pointLight_object_shape->mesh() };
	m_pointLight_object = engine::game_object::create(pointLight_object_props);

	// Load the cow model. Create a cow object. Set its properties
	engine::ref <engine::model> cow_model = engine::model::create("assets/models/static/cow4.3ds");
	engine::game_object_properties cow_props;
	cow_props.meshes = cow_model->meshes();
	cow_props.textures = cow_model->textures();
	float cow_scale = 1.f / glm::max(cow_model->size().x, glm::max(cow_model->size().y, cow_model->size().z));
	cow_props.position = { 19.3f, 0.8f, -0.2f };
	cow_props.rotation_axis = glm::vec3(0.f, 1.f, 0.f);
	cow_props.rotation_amount = glm::radians(90.f);
	cow_props.scale = glm::vec3(cow_scale);
	cow_props.bounding_shape = cow_model->size() / 2.f;
	cow_props.type = 0;
	cow_props.is_static = true;
	m_cow = engine::game_object::create(cow_props);
	m_cow->set_offset(cow_model->offset());
	m_cow_box.set_box(cow_props.bounding_shape.x * 2.f * cow_scale, cow_props.bounding_shape.y * 2.f * cow_scale, cow_props.bounding_shape.z * 2.f
		* cow_scale, cow_props.position - glm::vec3(0.f, m_cow->offset().y, 0.f) * m_cow->scale());

	//load droid model
	engine::ref <engine::model> droid_model = engine::model::create("assets/models/static/droid/droid.obj");
	engine::game_object_properties droid_props;
	droid_props.meshes = droid_model->meshes();
	droid_props.textures = droid_model->textures();
	droid_props.position = glm::vec3(7.f, 0.5f, 0.f);
	droid_props.rotation_axis = glm::vec3(0.f, 1.f, 0.f);
	droid_props.scale = glm::vec3(1.f / glm::max(droid_model->size().x, glm::max(droid_model->size().y, droid_model->size().z)));
	droid_props.bounding_shape = glm::vec3(droid_model->size().x / 5.f, droid_model->size().y / 2.f, droid_model->size().z / 5.f);
	droid_props.type = 0;
	m_droid = engine::game_object::create(droid_props);
	m_droid->set_offset(droid_model->offset());
	m_droid_box.set_box(droid_props.bounding_shape.x * 8.f * droid_props.scale.x, droid_props.bounding_shape.y * 2.f * droid_props.scale.x, droid_props.bounding_shape.z * 10.f
		* droid_props.scale.x, droid_props.position - glm::vec3(0.f, m_droid->offset().y, 0.f) * m_droid->scale());
	m_enemy_droid.initialise(m_droid, droid_props.position, glm::vec3(1.f, 0.f, 0.f), false, false);


	//load mech model
	engine::ref <engine::model> mech_model = engine::model::create("assets/models/static/mech/mech.obj");
	engine::game_object_properties mech_props;
	mech_props.meshes = mech_model->meshes();
	mech_props.textures = mech_model->textures();
	mech_props.position = glm::vec3(4.f, 0.5f, 40.f);
	mech_props.rotation_axis = glm::vec3(0.f, 1.f, 0.f);
	mech_props.scale = mech_model->size();
	mech_props.bounding_shape = glm::vec3(mech_model->size().x / 3.f, mech_model->size().y / 2.f, mech_model->size().z / 3.f);
	mech_props.type = 0;
	m_mech = engine::game_object::create(mech_props);
	m_mech->set_offset(mech_model->offset());
	m_mech_box.set_box(mech_props.bounding_shape.x * 2.5f * mech_props.scale.x, mech_props.bounding_shape.y * 2.3f * mech_props.scale.x, mech_props.bounding_shape.z * 2.5f
		* mech_props.scale.x, mech_props.position - glm::vec3(0.f, m_mech->offset().y, 0.f) * m_mech->scale());
	m_enemy_mech.initialise(m_mech, mech_props.position, glm::vec3(1.f, 0.f, 0.f), false, true);

	//load drone model
	engine::ref <engine::model> drone_model = engine::model::create("assets/models/static/drone/drone.obj");
	engine::game_object_properties drone_props;
	drone_props.meshes = drone_model->meshes();
	drone_props.textures = drone_model->textures();
	drone_props.position = glm::vec3(-4.f, 2.f, 0.f);
	drone_props.rotation_axis = glm::vec3(0.f, 1.f, 0.f);
	drone_props.scale = drone_model->size();
	drone_props.bounding_shape = glm::vec3(drone_model->size().x / 4.f, drone_model->size().y / 10.f, drone_model->size().z / 4.f);
	drone_props.type = 0;
	m_drone = engine::game_object::create(drone_props);
	m_drone->set_offset(drone_model->offset());
	m_drone_box.set_box(drone_props.bounding_shape.x * 4.f * drone_props.scale.x, drone_props.bounding_shape.y * 10.f * drone_props.scale.x, drone_props.bounding_shape.z * 5.f
		* drone_props.scale.x, drone_props.position - glm::vec3(0.f, m_drone->offset().y, 0.f) * m_drone->scale());
	m_enemy_drone.initialise(m_drone, drone_props.position, glm::vec3(1.f, 0.f, 0.f), true);

	//load bb8 model
	engine::ref <engine::model> bb8_model = engine::model::create("assets/models/static/bb8/bb8.obj");
	engine::game_object_properties bb8_props;
	bb8_props.meshes = bb8_model->meshes();
	bb8_props.textures = bb8_model->textures();
	bb8_props.position = glm::vec3(4.f, 0.8f, 0.f);
	bb8_props.rotation_axis = glm::vec3(0.f, 1.f, 0.f);
	bb8_props.scale = (glm::vec3(1.f / glm::max(bb8_model->size().x, glm::max(bb8_model->size().y, bb8_model->size().z)))) * 0.8f;
	bb8_props.bounding_shape = glm::vec3(bb8_model->size().x / 3.f, bb8_model->size().y / 2.f, bb8_model->size().z / 2.f);
	bb8_props.type = 0;
	m_bb8 = engine::game_object::create(bb8_props);
	m_bb8->set_offset(bb8_model->offset());
	m_bb8_box.set_box(bb8_props.bounding_shape.x * 3.2f * bb8_props.scale.x, bb8_props.bounding_shape.y * 2.f * bb8_props.scale.x, bb8_props.bounding_shape.z * 2.5f 
		* bb8_props.scale.x, bb8_props.position - glm::vec3(0.f, m_bb8->offset().y, 0.f) * m_bb8->scale());
	m_enemy_bb8.initialise(m_bb8, bb8_props.position, glm::vec3(1.f, 0.f, 0.f), false);

	//load heart model
	engine::ref <engine::model> heart_model = engine::model::create("assets/models/static/heart/heart.obj");
	engine::game_object_properties heart_props;
	heart_props.meshes = heart_model->meshes();
	heart_props.textures = heart_model->textures();
	heart_props.position = glm::vec3(20.f, 1.f, 15.f);
	heart_props.rotation_axis = glm::vec3(0.f, 1.f, 0.f);
	heart_props.bounding_shape = glm::vec3(heart_model->size().x / 2.f, heart_model->size().y / 2.f, heart_model->size().z / 2.f);
	heart_props.type = 0;
	m_heart = engine::game_object::create(heart_props);
	m_heart->set_offset(heart_model->offset());
	m_heart_box.set_box(heart_props.bounding_shape.x * 2.f, heart_props.bounding_shape.y * 2.f, heart_props.bounding_shape.z * 2.f,
		heart_props.position - glm::vec3(0.f, m_heart->offset().y, 0.f) * m_heart->scale());

	//load shield model
	engine::ref <engine::model> shield_model = engine::model::create("assets/models/static/shield/shield.obj");
	engine::game_object_properties shield_props;
	shield_props.meshes = shield_model->meshes();
	shield_props.textures = shield_model->textures();
	shield_props.position = glm::vec3(20.f, 1.f, 7.f);
	shield_props.rotation_axis = glm::vec3(0.f, 1.f, 0.f);
	shield_props.bounding_shape = glm::vec3(shield_model->size().x / 2.f, shield_model->size().y / 2.f, shield_model->size().z / 2.f);
	shield_props.type = 0;
	m_shield = engine::game_object::create(shield_props);
	m_shield->set_offset(shield_model->offset());
	m_shield_box.set_box(shield_props.bounding_shape.x * 2.f, shield_props.bounding_shape.y * 2.f, shield_props.bounding_shape.z * 2.f,
		shield_props.position - glm::vec3(0.f, m_shield->offset().y, 0.f) * m_shield->scale());

	// Load the jeep model.
	engine::ref <engine::model> jeep_model = engine::model::create("assets/models/static/jeep1/jeep1.obj");
	engine::game_object_properties jeep_props;
	jeep_props.meshes = jeep_model->meshes();
	jeep_props.textures = jeep_model->textures();
	//float jeep_scale = 0.125f;
	jeep_props.position = { -5.f, 0.5f, 0.f };
	//jeep_props.scale = glm::vec3(jeep_scale);
	m_jeep = engine::game_object::create(jeep_props);

	// Load pizza model
	engine::ref <engine::model> pizza_model = engine::model::create("assets/models/static/pizza/pizza.obj");
	engine::game_object_properties pizza_props;
	pizza_props.meshes = pizza_model->meshes();
	pizza_props.textures = pizza_model->textures();
	pizza_props.position = { -1.f, 0.53f, 0.f };
	m_pizza = engine::game_object::create(pizza_props);

	//Load jet model
	engine::ref <engine::model> jet_model = engine::model::create("assets/models/static/jet/jet.obj");
	engine::game_object_properties jet_props;
	jet_props.meshes = jet_model->meshes();
	jet_props.textures = jet_model->textures();
	jet_props.position = { 14.f, 1.5f, 9.f };
	m_jet = engine::game_object::create(jet_props);

	//Load office model
	engine::ref <engine::model> office_model = engine::model::create("assets/models/static/office/office.obj");
	engine::game_object_properties office_props;
	office_props.meshes = office_model->meshes();
	office_props.textures = office_model->textures();
	office_props.position = { 35.f, 0.5f, 0.f };
	m_office = engine::game_object::create(office_props);

	//Load sanfran model
	engine::ref <engine::model> sanfran_model = engine::model::create("assets/models/static/sanfran/sanfran.obj");
	engine::game_object_properties sanfran_props;
	sanfran_props.meshes = sanfran_model->meshes();
	sanfran_props.textures = sanfran_model->textures();
	sanfran_props.position = { -20.f, 1.72f, 7.f };
	m_sanfran = engine::game_object::create(sanfran_props);

	//Load skyscrapers model
	engine::ref <engine::model> skyscrapers_model = engine::model::create("assets/models/static/skyscrapers/skyscrapers.obj");
	engine::game_object_properties skyscrapers_props;
	skyscrapers_props.meshes = skyscrapers_model->meshes();
	skyscrapers_props.textures = skyscrapers_model->textures();
	skyscrapers_props.position = { -25.f, -3.5f, 33.f };
	m_skyscrapers = engine::game_object::create(skyscrapers_props);

	//Load policeCar model
	engine::ref <engine::model> policeCar_model = engine::model::create("assets/models/static/policecar/policecar.obj");
	engine::game_object_properties policeCar_props;
	policeCar_props.meshes = policeCar_model->meshes();
	policeCar_props.textures = policeCar_model->textures();
	policeCar_props.position = { 9.f, 0.5f, 12.f };
	policeCar_props.rotation_axis = glm::vec3(0.f, 1.f, 0.f);
	policeCar_props.rotation_amount = glm::radians(-45.f);
	m_policeCar = engine::game_object::create(policeCar_props);

	// Load the tree model. Create a tree object. Set its properties
	engine::ref <engine::model> tree_model = engine::model::create("assets/models/static/elm.3ds");
	engine::game_object_properties tree_props;
	tree_props.meshes = tree_model->meshes();
	tree_props.textures = tree_model->textures();
	float tree_scale = 3.f / glm::max(tree_model->size().x, glm::max(tree_model->size().y, tree_model->size().z));
	tree_props.position = { 4.f, 0.5f, -5.f };
	tree_props.bounding_shape = tree_model->size() / 2.f * tree_scale;
	tree_props.scale = glm::vec3(tree_scale);
	m_tree = engine::game_object::create(tree_props);

	//===============================================================World Props End=========================================================================

	// Load sphere
	float radius = 0.5f;
	engine::ref<engine::sphere> sphere_shape = engine::sphere::create(10, 20, radius);
	engine::game_object_properties sphere_props;
	sphere_props.position = { 0.f, 5.f, -5.f };
	sphere_props.meshes = { sphere_shape->mesh() };
	sphere_props.type = 1;
	sphere_props.bounding_shape = glm::vec3(radius);
	sphere_props.restitution = 0.92f;
	sphere_props.mass = 0.1f;
	sphere_props.rolling_friction = 0.1f;
	m_ball = engine::game_object::create(sphere_props);
	m_red_spotLight_ball = engine::game_object::create(sphere_props);

	// Load tetrahedron
	std::vector<glm::vec3> tetrahedron_vertices;

	tetrahedron_vertices.push_back(glm::vec3(0.f, 10.f, 0.f));//0
	tetrahedron_vertices.push_back(glm::vec3(0.f, 0.f, 10.f));//1
	tetrahedron_vertices.push_back(glm::vec3(-10.f, 0.f, -10.f)); //2
	tetrahedron_vertices.push_back(glm::vec3(10.f, 0.f, -10.f)); //3

	//create tetrahedron mesh shape
	engine::ref<engine::tetrahedron> tetrahedron_shape = engine::tetrahedron::create(tetrahedron_vertices);
	engine::game_object_properties tetrahedron_props;
	tetrahedron_props.position = { 0.f, 0.5f, -20.f };

	//load tetrahedron texture
	std::vector<engine::ref<engine::texture_2d>> tetrahedron_textures =
	{ engine::texture_2d::create("assets/textures/Skyscraper.bmp", false) };

	tetrahedron_props.textures = tetrahedron_textures;

	tetrahedron_props.meshes = { tetrahedron_shape->mesh() };
	m_tetrahedron = engine::game_object::create(tetrahedron_props);



	m_tetrahedron_material = engine::material::create(32.0f,
		glm::vec3(1.0f, 0.5f, 0.0f),
		glm::vec3(1.0f, 0.5f, 0.0f),
		glm::vec3(0.5f, 0.5f, 0.5f),
		0.3f);

	// Load bench
	engine::ref<engine::bench> bench_shape = engine::bench::create();
	engine::game_object_properties bench_props;
	bench_props.position = { 0.f, 0.9f, 0.f };

	std::vector<engine::ref<engine::texture_2d>> bench_textures =
	{ engine::texture_2d::create("assets/textures/plastic.jpg", false) };
	bench_props.textures = bench_textures;

	bench_props.scale = glm::vec3(0.8f);
	bench_props.meshes = { bench_shape->mesh() };
	m_bench = engine::game_object::create(bench_props);

	// Load lamppost
	engine::ref<engine::lamppost> lamppost_shape = engine::lamppost::create();
	engine::game_object_properties lamppost_props;
	lamppost_props.position = { 11.25f, 0.5f, 8.f };

	std::vector<engine::ref<engine::texture_2d>> lamppost_textures =
	{ engine::texture_2d::create("assets/textures/lamppost.jpg", false) };
	lamppost_props.textures = lamppost_textures;

	lamppost_props.scale = glm::vec3(0.8f);
	lamppost_props.meshes = { lamppost_shape->mesh() };
	m_lamppost = engine::game_object::create(lamppost_props);

	// Load hexagon
	engine::ref<engine::hexagon> hexagon_shape = engine::hexagon::create();
	engine::game_object_properties hexagon_props;
	hexagon_props.position = { 0.f, 0.f, 0.f };

	std::vector<engine::ref<engine::texture_2d>> hexagon_textures =
	{ engine::texture_2d::create("assets/textures/manhole.jpg", false) };
	hexagon_props.textures = hexagon_textures;

	hexagon_props.scale = glm::vec3(0.35f);
	hexagon_props.meshes = { hexagon_shape->mesh() };
	m_hexagon = engine::game_object::create(hexagon_props);


	m_game_objects.push_back(m_terrain);
	m_game_objects.push_back(m_weapon_holder);
	m_game_objects.push_back(m_ball);
	m_game_objects.push_back(m_cow);
	//m_game_objects.push_back(m_tree);
	//m_game_objects.push_back(m_pickup);
	m_game_objects.push_back(m_red_spotLight_ball);
	m_game_objects.push_back(m_mannequin);
	m_game_objects.push_back(m_droid);
	m_game_objects.push_back(m_mech);
	m_game_objects.push_back(m_drone);
	m_game_objects.push_back(m_bb8);
	m_game_objects.push_back(m_missile);
	m_game_objects.push_back(m_enemy_missile);
	m_game_objects.push_back(m_enemy_missile2);
	m_game_objects.push_back(m_bouncynade);
	m_physics_manager = engine::bullet_manager::create(m_game_objects);

	m_text_manager = engine::text_manager::create();

	m_skinned_mesh->switch_animation(2);

	//Load Intro Screen Texture and HUD
	m_intro_screen = screen_render::create("assets/textures/tempmenu.jpg", 1.6f, 0.9f);
	m_intro_screen->deactivate();

	m_HUD = screen_render::create("assets/textures/HUD/tempHUD.png", 1.6f, 0.9f);
	m_HUD_range_finder = screen_render::create("assets/textures/HUD/range_finder.png", 1.6f, 0.9f);
	m_HUD_jetpack = screen_render::create("assets/textures/HUD/jetpack.png", 0.12f, 0.12f);
	m_HUD_missile = screen_render::create("assets/textures/HUD/missile.png", 0.12f, 0.12f);
	m_HUD_bouncynade = screen_render::create("assets/textures/HUD/bouncynade.png", 0.12f, 0.12f);

	m_jetpack_trail.load_texture("assets/textures/effects/trail.png");
	m_explosion = explosion::create("assets/textures/effects/Explosion.tga", 4, 5, 16);
	m_cross_fade = cross_fade::create("assets/textures/effects/screencrack.png", 2.0f, 0.4f, 0.2f);
}

example_layer::~example_layer() {}

void example_layer::on_render()
{
	engine::render_command::clear_color({ 0.2f, 0.3f, 0.3f, 1.0f });
	engine::render_command::clear();

	//const auto textured_shader = engine::renderer::shaders_library()->get("mesh_static");
	//engine::renderer::begin_scene(m_3d_camera, textured_shader);

	const auto textured_lighting_shader = engine::renderer::shaders_library()->get("mesh_lighting");


	//===============================================================3D Cam============================================================================
	engine::renderer::begin_scene(m_3d_camera, textured_lighting_shader);

	// Set up some of the scene's parameters in the shader
	std::dynamic_pointer_cast<engine::gl_shader>(textured_lighting_shader)->set_uniform("gEyeWorldPos", m_3d_camera.position());

	// Position the skybox centred on the player and render it
	glm::mat4 skybox_tranform(1.0f);
	skybox_tranform = glm::translate(skybox_tranform, m_3d_camera.position());
	for (const auto& texture : m_skybox->textures())
	{
		texture->bind();
	}
	engine::renderer::submit(textured_lighting_shader, m_skybox, skybox_tranform);

	engine::renderer::submit(textured_lighting_shader, m_terrain);

	engine::renderer::submit(textured_lighting_shader, m_weapon_holder);

	engine::renderer::submit(textured_lighting_shader, m_tetrahedron);


	std::dynamic_pointer_cast<engine::gl_shader>(textured_lighting_shader)->set_uniform("gNumSpotLights", (int)num_spot_lights);
	m_red_spotLight.submit(textured_lighting_shader, 0);

	std::dynamic_pointer_cast<engine::gl_shader>(textured_lighting_shader)->set_uniform("gNumPointLights", (int)num_point_lights);
	m_white_pointLight.submit(textured_lighting_shader, 0);



	//===============================================================Road System============================================================================

	engine::renderer::submit(textured_lighting_shader, m_intersection);

	glm::mat4 intersectionTransform = glm::mat4(1.0f);
	intersectionTransform = glm::translate(intersectionTransform, m_intersection->position() + glm::vec3(0.f, 0.f, -66.f));
	intersectionTransform = glm::rotate(intersectionTransform, m_intersection->rotation_amount(), m_intersection->rotation_axis());
	engine::renderer::submit(textured_lighting_shader, intersectionTransform, m_intersection);

	glm::mat4 intersectionTransform2 = glm::mat4(1.0f);
	intersectionTransform2 = glm::translate(intersectionTransform2, m_intersection->position() + glm::vec3(-66.f, 0.f, 0.f));
	intersectionTransform2 = glm::rotate(intersectionTransform2, m_intersection->rotation_amount(), m_intersection->rotation_axis());
	engine::renderer::submit(textured_lighting_shader, intersectionTransform2, m_intersection);

	glm::mat4 intersectionTransform3 = glm::mat4(1.0f);
	intersectionTransform3 = glm::translate(intersectionTransform3, m_intersection->position() + glm::vec3(-66.f, 0.f, -66.f));
	intersectionTransform3 = glm::rotate(intersectionTransform3, m_intersection->rotation_amount(), m_intersection->rotation_axis());
	engine::renderer::submit(textured_lighting_shader, intersectionTransform3, m_intersection);


	//===============================================================Vertical Roads============================================================================
	engine::renderer::submit(textured_lighting_shader, m_road);

	glm::mat4 roadTransform = glm::mat4(1.0f);
	roadTransform = glm::translate(roadTransform, m_road->position() + glm::vec3(0.f, 0.f, 66.f));
	roadTransform = glm::rotate(roadTransform, m_road->rotation_amount(), m_road->rotation_axis());
	engine::renderer::submit(textured_lighting_shader, roadTransform, m_road);

	glm::mat4 roadTransform2 = glm::mat4(1.0f);
	roadTransform2 = glm::translate(roadTransform2, m_road->position() + glm::vec3(-66.f, 0.f, 0.f));
	roadTransform2 = glm::rotate(roadTransform2, m_road->rotation_amount(), m_road->rotation_axis());
	engine::renderer::submit(textured_lighting_shader, roadTransform2, m_road);

	glm::mat4 roadTransform3 = glm::mat4(1.0f);
	roadTransform3 = glm::translate(roadTransform3, m_road->position() + glm::vec3(-66.f, 0.f, 66.f));
	roadTransform3 = glm::rotate(roadTransform3, m_road->rotation_amount(), m_road->rotation_axis());
	engine::renderer::submit(textured_lighting_shader, roadTransform3, m_road);

	//===============================================================Horizontal Roads============================================================================
	glm::mat4 roadHorizontalTransform = glm::mat4(1.0f);
	roadHorizontalTransform = glm::translate(roadHorizontalTransform, glm::vec3(-29.5f, 0.50001f, 30.f));
	roadHorizontalTransform = glm::rotate(roadHorizontalTransform, m_road->rotation_amount() + glm::radians(90.f), glm::vec3(0.f, 1.f, 0.f));
	engine::renderer::submit(textured_lighting_shader, roadHorizontalTransform, m_road);

	glm::mat4 roadHorizontalTransform2 = glm::mat4(1.0f);
	roadHorizontalTransform2 = glm::translate(roadHorizontalTransform2, glm::vec3(36.5f, 0.50001f, 30.f));
	roadHorizontalTransform2 = glm::rotate(roadHorizontalTransform2, m_road->rotation_amount() + glm::radians(90.f), glm::vec3(0.f, 1.f, 0.f));
	engine::renderer::submit(textured_lighting_shader, roadHorizontalTransform2, m_road);

	glm::mat4 roadHorizontalTransform3 = glm::mat4(1.0f);
	roadHorizontalTransform3 = glm::translate(roadHorizontalTransform3, glm::vec3(-29.5f, 0.50001f, -36.f));
	roadHorizontalTransform3 = glm::rotate(roadHorizontalTransform3, m_road->rotation_amount() + glm::radians(90.f), glm::vec3(0.f, 1.f, 0.f));
	engine::renderer::submit(textured_lighting_shader, roadHorizontalTransform3, m_road);

	glm::mat4 roadHorizontalTransform4 = glm::mat4(1.0f);
	roadHorizontalTransform4 = glm::translate(roadHorizontalTransform4, glm::vec3(36.5f, 0.50001f, -36.f));
	roadHorizontalTransform4 = glm::rotate(roadHorizontalTransform4, m_road->rotation_amount() + glm::radians(90.f), glm::vec3(0.f, 1.f, 0.f));
	engine::renderer::submit(textured_lighting_shader, roadHorizontalTransform4, m_road);

	//=========================================================Road System End============================================================================

	m_player.getBox().on_render(2.5f, 0.f, 0.f, textured_lighting_shader);
	m_cow_box.on_render(2.5f, 0.f, 0.f, textured_lighting_shader);
	m_droid_box.on_render(2.5f, 0.f, 0.f, textured_lighting_shader);
	m_heart_box.on_render(2.5f, 0.f, 0.f, textured_lighting_shader);
	m_shield_box.on_render(2.5f, 0.f, 0.f, textured_lighting_shader);
	m_mech_box.on_render(2.5f, 0.f, 0.f, textured_lighting_shader);
	m_drone_box.on_render(2.5f, 0.f, 0.f, textured_lighting_shader);
	m_bb8_box.on_render(2.5f, 0.f, 0.f, textured_lighting_shader);
	missile.getBox().on_render(2.5f, 0.f, 0.f, textured_lighting_shader);
	enemy_missile.getBox().on_render(2.5f, 0.f, 0.f, textured_lighting_shader);
	enemy_missile2.getBox().on_render(2.5f, 0.f, 0.f, textured_lighting_shader);

	glm::mat4 lamppostTransform = glm::mat4(1.0f);
	lamppostTransform = glm::translate(lamppostTransform, m_lamppost->position());
	lamppostTransform = glm::rotate(lamppostTransform, m_lamppost->rotation_amount() + glm::radians(-90.f), m_lamppost->rotation_axis());
	lamppostTransform = glm::scale(lamppostTransform, m_lamppost->scale());
	engine::renderer::submit(textured_lighting_shader, lamppostTransform, m_lamppost);

	glm::mat4 benchTransform = glm::mat4(1.0f);
	benchTransform = glm::translate(benchTransform, glm::vec3(19.8f, 0.9f, 0.f));
	benchTransform = glm::scale(benchTransform, m_bench->scale());
	engine::renderer::submit(textured_lighting_shader, benchTransform, m_bench);

	glm::mat4 benchTransform2 = glm::mat4(1.0f);
	benchTransform2 = glm::translate(benchTransform2, glm::vec3(25.8f, 0.9f, 0.f));
	benchTransform2 = glm::scale(benchTransform2, m_bench->scale());
	engine::renderer::submit(textured_lighting_shader, benchTransform2, m_bench);

	float bench = 0;
	for (int i = 0; i <= 3; ++i) {

		if (i > 1) {
			bench = 7.f;
		}

		glm::mat4 benchTransform3 = glm::mat4(1.0f);
		benchTransform3 = glm::translate(benchTransform3, glm::vec3(-12.3f, 0.9f, -13.7f + (i * 5.7) + bench));
		benchTransform3 = glm::rotate(benchTransform3, m_bench->rotation_amount() + glm::radians(90.f), glm::vec3(0.f, 1.f, 0.f));
		benchTransform3 = glm::scale(benchTransform3, m_bench->scale());
		engine::renderer::submit(textured_lighting_shader, benchTransform3, m_bench);
	}

	glm::mat4 hexagontransform = glm::mat4(1.0f);
	hexagontransform = glm::translate(hexagontransform, glm::vec3(15.8f, 0.51f, 5.f));
	hexagontransform = glm::rotate(hexagontransform, m_hexagon->rotation_amount() + glm::radians(270.f), glm::vec3(1.f, 0.f, 0.f));
	hexagontransform = glm::scale(hexagontransform, m_hexagon->scale());
	engine::renderer::submit(textured_lighting_shader, hexagontransform, m_hexagon);

	glm::mat4 hexagontransform2 = glm::mat4(1.0f);
	hexagontransform2 = glm::translate(hexagontransform2, glm::vec3(15.8f, 0.51f, 15.f));
	hexagontransform2 = glm::rotate(hexagontransform2, m_hexagon->rotation_amount() + glm::radians(270.f), glm::vec3(1.f, 0.f, 0.f));
	hexagontransform2 = glm::scale(hexagontransform2, m_hexagon->scale());
	engine::renderer::submit(textured_lighting_shader, hexagontransform2, m_hexagon);

	//office trees
	for (int i = 0; i <= 3; ++i) {

		glm::mat4 tree_transform(1.0f);
		tree_transform = glm::translate(tree_transform, glm::vec3(19.5f + i * 3, 0.5, -1.0f));
		tree_transform = glm::rotate(tree_transform, m_tree->rotation_amount(), m_tree->rotation_axis());
		tree_transform = glm::scale(tree_transform, m_tree->scale());
		engine::renderer::submit(textured_lighting_shader, tree_transform, m_tree);
	}

	//sanfran trees
	float tree = 0;

	for (int i = 0; i <= 7; ++i) {

		if (i > 3) {
			tree = 6.5f;
		}
		glm::mat4 tree_transform(1.0f);
		tree_transform = glm::translate(tree_transform, glm::vec3(-13.5f, 0.5, -16.5f + (i * 3) + tree));
		tree_transform = glm::rotate(tree_transform, m_tree->rotation_amount(), m_tree->rotation_axis());
		tree_transform = glm::scale(tree_transform, m_tree->scale());
		engine::renderer::submit(textured_lighting_shader, tree_transform, m_tree);
	}

	//glm::mat4 tree_transform(1.0f);
	//tree_transform = glm::translate(tree_transform, glm::vec3(4.f, 0.5, -5.0f));
	//tree_transform = glm::rotate(tree_transform, m_tree->rotation_amount(), m_tree->rotation_axis());
	//tree_transform = glm::scale(tree_transform, m_tree->scale());
	//engine::renderer::submit(textured_lighting_shader, tree_transform, m_tree);

	glm::mat4 droid_transform(1.0f);
	droid_transform = glm::translate(droid_transform, m_droid->position() - m_droid->offset() * m_droid->scale());
	droid_transform = glm::rotate(droid_transform, m_droid->rotation_amount(), m_droid->rotation_axis());
	droid_transform = glm::scale(droid_transform, m_droid->scale());
	engine::renderer::submit(textured_lighting_shader, droid_transform, m_droid);

	glm::mat4 mech_transform(1.0f);
	mech_transform = glm::translate(mech_transform, m_mech->position() - m_mech->offset() * m_mech->scale());
	mech_transform = glm::rotate(mech_transform, m_mech->rotation_amount(), m_mech->rotation_axis());
	mech_transform = glm::scale(mech_transform, m_mech->scale());
	engine::renderer::submit(textured_lighting_shader, mech_transform, m_mech);

	glm::mat4 drone_transform(1.0f);
	drone_transform = glm::translate(drone_transform, m_drone->position() - m_drone->offset() * m_drone->scale());
	drone_transform = glm::rotate(drone_transform, m_drone->rotation_amount(), m_drone->rotation_axis());
	drone_transform = glm::scale(drone_transform, m_drone->scale());
	engine::renderer::submit(textured_lighting_shader, drone_transform, m_drone);

	glm::mat4 bb8_transform(1.0f);
	bb8_transform = glm::translate(bb8_transform, m_bb8->position() - m_bb8->offset() * m_bb8->scale());
	bb8_transform = glm::rotate(bb8_transform, m_bb8->rotation_amount(), m_bb8->rotation_axis());
	bb8_transform = glm::scale(bb8_transform, m_bb8->scale());
	engine::renderer::submit(textured_lighting_shader, bb8_transform, m_bb8);

	glm::mat4 heart_transform(1.0f);
	heart_transform = glm::translate(heart_transform, m_heart->position() - m_heart->offset() * m_heart->scale());
	heart_transform = glm::rotate(heart_transform, m_heart->rotation_amount(), m_heart->rotation_axis());
	heart_transform = glm::scale(heart_transform, m_heart->scale());
	engine::renderer::submit(textured_lighting_shader, heart_transform, m_heart);

	glm::mat4 shield_transform(1.0f);
	shield_transform = glm::translate(shield_transform, m_shield->position() - m_shield->offset() * m_shield->scale());
	shield_transform = glm::rotate(shield_transform, m_shield->rotation_amount(), m_shield->rotation_axis());
	shield_transform = glm::scale(shield_transform, m_shield->scale());
	engine::renderer::submit(textured_lighting_shader, shield_transform, m_shield);

	glm::mat4 cow_transform(1.0f);
	cow_transform = glm::translate(cow_transform, m_cow->position() - m_cow->offset() * m_cow -> scale());
	cow_transform = glm::rotate(cow_transform, m_cow->rotation_amount(), m_cow->rotation_axis());
	cow_transform = glm::scale(cow_transform, m_cow->scale());
	engine::renderer::submit(textured_lighting_shader, cow_transform, m_cow);

	float jeep_x_rotation = 90.f * (glm::pi<float>() / 180.f); //90 degrees multiplied by pi/180 giving radians

	glm::mat4 jeep_transform(1.0f);
	jeep_transform = glm::translate(jeep_transform, m_jeep->position() + glm::vec3(-4.f, 0.f, 0.f));
	jeep_transform = glm::rotate(jeep_transform, m_jeep->rotation_amount(), m_jeep->rotation_axis());
	jeep_transform = glm::scale(jeep_transform, m_jeep->scale());
	engine::renderer::submit(textured_lighting_shader, jeep_transform, m_jeep);

	glm::mat4 pizza_transform(1.0f);
	pizza_transform = glm::translate(pizza_transform, glm::vec3(20.2f, 0.92f, -0.2f));
	pizza_transform = glm::rotate(pizza_transform, m_pizza->rotation_amount() + glm::radians(120.f), glm::vec3(0.f, 1.f, 0.f));
	pizza_transform = glm::scale(pizza_transform, m_pizza->scale());
	engine::renderer::submit(textured_lighting_shader, pizza_transform, m_pizza);

	glm::mat4 pizza_transform2(1.0f);
	pizza_transform2 = glm::translate(pizza_transform2, glm::vec3(21.5f, 0.92f, -0.2f));
	pizza_transform2 = glm::rotate(pizza_transform2, m_pizza->rotation_amount() + glm::radians(57.f), glm::vec3(0.f, 1.f, 0.f));
	pizza_transform2 = glm::scale(pizza_transform2, m_pizza->scale());
	engine::renderer::submit(textured_lighting_shader, pizza_transform2, m_pizza);

	glm::mat4 jet_transform(1.0f);
	jet_transform = glm::translate(jet_transform, glm::vec3(14.f, 1.5f, 30.f));
	jet_transform = glm::rotate(jet_transform, m_jet->rotation_amount() + glm::radians(-45.f), glm::vec3(0.f, 1.f, 0.f));
	jet_transform = glm::scale(jet_transform, m_jet->scale());
	engine::renderer::submit(textured_lighting_shader, jet_transform, m_jet);

	glm::mat4 jet_transform2(1.0f);
	jet_transform2 = glm::translate(jet_transform2, glm::vec3(14.f, 1.5f, 38.f));
	jet_transform2 = glm::rotate(jet_transform2, m_jet->rotation_amount() + glm::radians(-90.f), glm::vec3(0.f, 1.f, 0.f));
	jet_transform2 = glm::scale(jet_transform2, m_jet->scale());
	engine::renderer::submit(textured_lighting_shader, jet_transform2, m_jet);

	glm::mat4 jet_transform3(1.0f);
	jet_transform3 = glm::translate(jet_transform3, glm::vec3(-2.f, 60.f, 18.f));
	jet_transform3 = glm::rotate(jet_transform3, m_jet->rotation_amount() + glm::radians(-15.f), glm::vec3(0.f, 0.f, 1.f));
	jet_transform3 = glm::scale(jet_transform3, m_jet->scale());
	engine::renderer::submit(textured_lighting_shader, jet_transform3, m_jet);

	glm::mat4 office_transform(1.0f);
	office_transform = glm::translate(office_transform, m_office->position());
	office_transform = glm::rotate(office_transform, m_office->rotation_amount(), glm::vec3(0.f, 1.f, 0.f));
	office_transform = glm::scale(office_transform, m_office->scale());
	engine::renderer::submit(textured_lighting_shader, office_transform, m_office);

	glm::mat4 sanfran_transform(1.0f);
	sanfran_transform = glm::translate(sanfran_transform, m_sanfran->position());
	sanfran_transform = glm::rotate(sanfran_transform, m_sanfran->rotation_amount() + glm::radians(90.f), glm::vec3(0.f, 1.f, 0.f));
	sanfran_transform = glm::scale(sanfran_transform, m_sanfran->scale());
	engine::renderer::submit(textured_lighting_shader, sanfran_transform, m_sanfran);

	glm::mat4 skyscrapers_transform(1.0f);
	skyscrapers_transform = glm::translate(skyscrapers_transform, m_skyscrapers->position());
	skyscrapers_transform = glm::rotate(skyscrapers_transform, m_skyscrapers->rotation_amount(), glm::vec3(0.f, 1.f, 0.f));
	skyscrapers_transform = glm::scale(skyscrapers_transform, m_skyscrapers->scale());
	engine::renderer::submit(textured_lighting_shader, skyscrapers_transform, m_skyscrapers);

	glm::mat4 skyscrapers_transform2(1.0f);
	skyscrapers_transform2 = glm::translate(skyscrapers_transform2, glm::vec3(25.f, -3.5f, 60.f));
	skyscrapers_transform2 = glm::rotate(skyscrapers_transform2, m_skyscrapers->rotation_amount() + glm::radians(180.f), glm::vec3(0.f, 1.f, 0.f));
	skyscrapers_transform2 = glm::scale(skyscrapers_transform2, m_skyscrapers->scale());
	engine::renderer::submit(textured_lighting_shader, skyscrapers_transform2, m_skyscrapers);

	glm::mat4 policeCar_transform(1.0f);
	policeCar_transform = glm::translate(policeCar_transform, m_policeCar->position());
	policeCar_transform = glm::rotate(policeCar_transform, m_policeCar->rotation_amount(), m_policeCar->rotation_axis());
	policeCar_transform = glm::scale(policeCar_transform, m_policeCar->scale());
	engine::renderer::submit(textured_lighting_shader, policeCar_transform, m_policeCar);

	missile.on_render(textured_lighting_shader);
	enemy_missile.on_render(textured_lighting_shader);
	enemy_missile2.on_render(textured_lighting_shader);
	bouncynade.on_render(textured_lighting_shader);

	engine::renderer::end_scene();

	// Set up material shader. (does not render textures, renders materials instead)
	const auto material_shader = engine::renderer::shaders_library()->get("mesh_material");
	engine::renderer::begin_scene(m_3d_camera, material_shader);

	m_material->submit(material_shader);
	std::dynamic_pointer_cast<engine::gl_shader>(material_shader)->set_uniform("gEyeWorldPos", m_3d_camera.position());

	//might not be needed?
	//std::dynamic_pointer_cast<engine::gl_shader>(material_shader)->set_uniform("gNumSpotLights", (int)num_spot_lights);
	//m_red_spotLight.submit(material_shader, 0);



	engine::renderer::submit(material_shader, m_ball);


	//-------------------------------------------------------light ball--------------------------------------------------------
	std::dynamic_pointer_cast<engine::gl_shader>(material_shader)->set_uniform("lighting_on", false);

	//m_red_spotLight_material->submit(material_shader);
	//engine::renderer::submit(material_shader, m_red_spotLight_ball->meshes().at(0), glm::translate(glm::mat4(1.f), m_red_spotLight.Position));

	m_white_pointLight_material->submit(material_shader);
	//glm::mat4 pointLight_transform(1.0f);
	//pointLight_transform = glm::translate(pointLight_transform, m_white_pointLight.Position);
	//pointLight_transform = glm::scale(pointLight_transform, glm::vec3(0.2f));
	//engine::renderer::submit(material_shader, m_ball->meshes().at(0), pointLight_transform);
	engine::renderer::submit(material_shader, glm::translate(glm::mat4(1.f), m_white_pointLight.Position), m_pointLight_object);

	std::dynamic_pointer_cast<engine::gl_shader>(material_shader)->set_uniform("lighting_on", true);
	//-------------------------------------------------------light ball--------------------------------------------------------

	engine::renderer::end_scene();

	const auto animated_mesh_shader = engine::renderer::shaders_library()->get("animated_mesh");
	engine::renderer::begin_scene(m_3d_camera, animated_mesh_shader);
	std::dynamic_pointer_cast<engine::gl_shader>(animated_mesh_shader)->set_uniform("gEyeWorldPos", m_3d_camera.position());

	glm::mat4 aniTransform = glm::mat4(1.0f);

	//renders player
	engine::renderer::submit(animated_mesh_shader, m_player.object());

	engine::renderer::end_scene();
	//===============================================================3D Cam End============================================================================

	//================================================================Effects Render=======================================================================
	engine::renderer::begin_scene(m_3d_camera, textured_lighting_shader);

	m_explosion->on_render(m_3d_camera, textured_lighting_shader);

	m_jetpack_trail.on_render(m_3d_camera, textured_lighting_shader);

	engine::renderer::end_scene();

	// Render text
	const auto text_shader = engine::renderer::shaders_library()->get("text_2D");
	m_text_manager->render_text(text_shader, "Orange Text", 10.f, (float)engine::application::window().height() - 25.f, 0.5f, glm::vec4(1.f, 0.5f, 0.f, 1.f));
	m_text_manager->render_text(text_shader, std::to_string(m_player.get_health_point()), (float)engine::application::window().width() * 0.1f, (float)engine::application::window().height() * 0.106f, 1.3f, glm::vec4(0.74f, 0.71f, 0.71f, 1.f)); //1.0 scale for 720p

	//===============================================================2D Cam============================================================================

	//old code for turning lighting on and off
	//const auto hud_lighting_shader = engine::renderer::shaders_library()->get("mesh_lighting");
	//std::dynamic_pointer_cast<engine::gl_shader>(textured_lighting_shader)->bind();
	//std::dynamic_pointer_cast<engine::gl_shader>(textured_lighting_shader)->set_uniform("lighting_on", false);

	engine::renderer::begin_scene(m_2d_camera, textured_lighting_shader);

	std::dynamic_pointer_cast<engine::gl_shader>(textured_lighting_shader)->set_uniform("lighting_on", false);

	m_intro_screen->on_render(textured_lighting_shader, 0.f, 0.f, 0.f);

	m_HUD->on_render(textured_lighting_shader, 0.f, 0.f, 0.01f);

	//Moves range indicator up and down based on player height
	range_finder_height = (m_player.object()->position().y - 1.f) * 0.015f;
	if (range_finder_height > 1.19f) {
		range_finder_height = 1.18f;
	}
	m_HUD_range_finder->on_render(textured_lighting_shader, 0.f, range_finder_height, 0.02f);

	//Renders jetpack icon if hover is on
	if (jetpackHoverOn){
		m_HUD_jetpack->on_render(textured_lighting_shader, 1.3f, -0.4f, 0.03f);
	}

	//Renders which weapon is currently selected
	if (WeaponSlot == WeaponState::Slot1) {
		m_HUD_missile->on_render(textured_lighting_shader, 1.3f, -0.65f, 0.03f);
	}
	else if (WeaponSlot == WeaponState::Slot2) {
		m_HUD_bouncynade->on_render(textured_lighting_shader, 1.3f, -0.65f, 0.03f);
	}
	

	m_cross_fade->on_render(textured_lighting_shader);

	std::dynamic_pointer_cast<engine::gl_shader>(textured_lighting_shader)->set_uniform("lighting_on", true);
	engine::renderer::end_scene();
	//===============================================================2D Cam End============================================================================
}

void example_layer::on_update(const engine::timestep& time_step)
{
	m_intro_screen->on_update(time_step);

	m_physics_manager->dynamics_world_update(m_game_objects, double(time_step));


	//===============================================================Objects Update============================================================================

	//After about 9 hours of testing (yes genuinely 9 hours just on boxes), boxes are rendered wrongly opposed to where they actually are
	m_cow_box.on_update(m_cow->position() - glm::vec3(0.f, m_cow->offset().y, 0.f) * m_cow->scale(), m_cow->rotation_amount(), m_cow->rotation_axis());
	m_heart_box.on_update(m_heart->position() - glm::vec3(0.f, m_heart->offset().y, 0.f), m_heart->rotation_amount(), m_heart->rotation_axis());
	m_shield_box.on_update(m_shield->position() - glm::vec3(0.f, m_shield->offset().y, 0.f), m_shield->rotation_amount(), m_shield->rotation_axis());

	m_player.on_update(time_step);
	m_player.getBox().on_update(m_player.object()->position() - glm::vec3(0.f, m_player.object()->offset().y, 0.f) * m_player.object()->scale(),
		m_player.object()->rotation_amount(), m_player.object()->rotation_axis());

	m_enemy_drone.on_update(time_step, m_player.object()->position() + glm::vec3(0.f, 0.5f, 0.f));
	m_drone_box.on_update(m_drone->position() - glm::vec3(0.45f, m_drone->offset().y, 0.45f) * m_drone->scale(), m_drone->rotation_amount(), m_drone->rotation_axis());
	
	m_enemy_bb8.on_update(time_step, m_player.object()->position());
	m_bb8_box.on_update(m_bb8->position() - glm::vec3(0.3f, m_bb8->offset().y, 0.3f) * m_bb8->scale(), m_bb8->rotation_amount(), m_bb8->rotation_axis());

	missile.on_update(time_step);
	missile.getBox().on_update(missile.object()->position() - glm::vec3(0.f, missile.object()->offset().y, 0.f) * missile.object()->scale(),
		missile.object()->rotation_amount(), missile.object()->rotation_axis());

	enemy_missile.on_update(time_step);
	enemy_missile.getBox().on_update(enemy_missile.object()->position() - glm::vec3(0.f, enemy_missile.object()->offset().y, 0.f) * enemy_missile.object()->scale(),
		enemy_missile.object()->rotation_amount(), enemy_missile.object()->rotation_axis());

	enemy_missile2.on_update(time_step);
	enemy_missile2.getBox().on_update(enemy_missile2.object()->position() - glm::vec3(0.f, enemy_missile2.object()->offset().y, 0.f) * enemy_missile2.object()->scale(),
		enemy_missile2.object()->rotation_amount(), enemy_missile2.object()->rotation_axis());

	bouncynade.on_update(time_step);

	//Camera Switching Logic
	if (CamMode == CamState::FirstPerson) {
		m_player.update_first_person_camera(m_3d_camera);
	}
	else if (CamMode == CamState::ThirdPerson) {
		m_player.update_third_person_camera(m_3d_camera);
	}
	else if (CamMode == CamState::FreeView) {
		m_3d_camera.on_update(time_step);
	}

	if (heartRotation > 360.f) {
		heartRotation = 0.f;
	}
	heartRotation += 20.f * time_step;
	m_heart->set_rotation_amount(glm::radians(heartRotation));

	if (shieldRotation > 360.f) {
		shieldRotation = 0.f;
	}
	shieldRotation += 20.f * time_step;
	m_shield->set_rotation_amount(glm::radians(shieldRotation));


	if (spotLightRotation > 360.f) {
		spotLightRotation = 0.f;
	}

	spotLightRotation += 200.f * time_step;

	m_red_spotLight_ball->set_rotation_axis(glm::vec3(0.f, 1.f, 0.f));
	m_red_spotLight_ball->set_rotation_amount(glm::radians(spotLightRotation));
	m_red_spotLight.Direction = m_red_spotLight_ball->forward();



	//===============================================================Collision Update============================================================================

	//==============================================================Player Stuff=================================================================================
	//checks whether missile is colliding and whether the missile is active i.e if the missile has already exploded
	if (m_missile->is_colliding() && missile.is_active()) {

		m_explosion->activate(missile.object()->position(), 4.f, 4.f);
		if (missile.getBox().collision(m_drone_box)) {
			m_enemy_drone.set_health_point(m_enemy_drone.get_health_point() - missile_damage);
			std::cout << m_enemy_drone.get_health_point() << '\n';
		}
		else if (missile.getBox().collision(m_bb8_box)) {
			m_enemy_bb8.set_health_point(m_enemy_bb8.get_health_point() - missile_damage);
			std::cout << m_enemy_bb8.get_health_point() << '\n';
		}
		else if (missile.getBox().collision(m_droid_box)) {
			m_enemy_droid.set_health_point(m_enemy_droid.get_health_point() - missile_damage);
			std::cout << m_enemy_droid.get_health_point() << '\n';
		}
		else if (missile.getBox().collision(m_mech_box)) {
			m_enemy_mech.set_health_point(m_enemy_mech.get_health_point() - missile_damage);
			std::cout << m_enemy_mech.get_health_point() << '\n';
		}

		missile.object()->set_velocity(glm::vec3(0.f));
		missile.object()->set_acceleration(glm::vec3(0.f, 0.f, 0.f));
		missile.object()->set_position(glm::vec3(-9.f, -9.f, 9.f));
		missile.set_active(false);
	}

	if (m_bouncynade->is_colliding() && !bouncynade_armed) {
		bouncynade_armtime = 3.f;

		//sets to false otherwise it will arm at 3.f every frame
		bouncynade_armed = true;
	}

	//check whether it is armed and counting down
	if (bouncynade_armtime > 0.0f && bouncynade_armed)
	{
		bouncynade_armtime -= (float)time_step;

		if (bouncynade_armtime < 0.0f)
		{
			m_explosion->activate(bouncynade.object()->position(), 4.f, 4.f);
			bouncynade.object()->set_position(glm::vec3(-8.f, -9.f, 9.f));
		}
	}

	if (m_heart_box.collision(m_player.getBox()) && m_player.get_health_point() < 100) {

		m_player.set_health_point(m_player.get_health_point() + 10);
		if (m_player.get_health_point() > 100) {
			m_player.set_health_point(100);
		}
		m_heart->set_position(glm::vec3(-4.f, -9.f, 9.f));
		heart_timer = heart_respawn_time;
	}

	if (heart_timer > 0.0f)
	{
		heart_timer -= (float)time_step;

		if (heart_timer < 0.0f)
		{
			m_heart->set_position(glm::vec3(20.f, 1.f, 15.f));
		}
	}

	if (m_shield_box.collision(m_player.getBox())) {
		player_immunity_timer = 5.f;
		m_shield->set_position(glm::vec3(-4.f, -9.f, 9.f));
		shield_timer = shield_respawn_time;
	}

	if (shield_timer > 0.0f)
	{
		shield_timer -= (float)time_step;

		if (shield_timer < 0.0f)
		{
			m_shield->set_position(glm::vec3(20.f, 1.f, 7.f));
		}
	}

	//==============================================================Enemy Attacks===============================================================================
	//Drone melee check
	if (m_drone_box.collision(m_player.getBox())) {

		//m_enemy_drone.object()->set_velocity(glm::vec3(0.f));
		//m_enemy_drone.object()->set_acceleration(glm::vec3(0.f, 9.8f, 0.f));

		if (!player_immunity) {
			damage_player(3);
		}
	}

	//BB8 melee check
	if (m_bb8_box.collision(m_player.getBox())) {

		m_enemy_bb8.object()->set_velocity(glm::vec3(0.f));

		if (!player_immunity) {
			damage_player(5);
		}
	}

	//Mech melee check
	if (m_mech_box.collision(m_player.getBox())) {

		m_enemy_mech.object()->set_velocity(glm::vec3(0.f));
		//m_enemy_drone.object()->set_acceleration(glm::vec3(0.f, 9.8f, 0.f));

		if (!player_immunity) {
			damage_player(30);
		}
	}

	//Mech missile check
	if (m_enemy_missile->is_colliding() && enemy_missile.is_active()) {

		//m_explosion->activate(enemy_missile.object()->position(), 4.f, 4.f);
		if (enemy_missile.getBox().collision(m_player.getBox())) {
			if (!player_immunity) {
				damage_player(10);
			}
		}

		enemy_missile.object()->set_velocity(glm::vec3(0.f));
		enemy_missile.object()->set_acceleration(glm::vec3(0.f, 0.f, 0.f));
		enemy_missile.object()->set_position(glm::vec3(-7.f, -9.f, 9.f));
		enemy_missile.set_active(false);
	}

	//Droid missile check
	if (m_enemy_missile2->is_colliding() && enemy_missile2.is_active()) {

		//m_explosion->activate(enemy_missile2.object()->position(), 4.f, 4.f);
		if (enemy_missile2.getBox().collision(m_player.getBox())) {
			if (!player_immunity) {
				damage_player(5);
			}
		}

		enemy_missile2.object()->set_velocity(glm::vec3(0.f));
		enemy_missile2.object()->set_acceleration(glm::vec3(0.f, 0.f, 0.f));
		enemy_missile2.object()->set_position(glm::vec3(-6.f, -9.f, 9.f));
		enemy_missile2.set_active(false);
	}

	if (player_immunity_timer > 0.0f)
	{
		if (!player_immunity) {
			player_immunity = true;
		}

		player_immunity_timer -= (float)time_step;

		if (player_immunity_timer < 0.0f)
		{
			player_immunity = false;
		}
	}

	check_bounce();


	if (m_ball->is_colliding() && m_ball->collision_objects().size() > 1)
	{
		m_material->set_ambient(glm::vec3(0.f, 1.f, 0.f));
		m_material->set_diffuse(glm::vec3(0.f, 1.f, 0.f));
	}
	else {
		m_material->set_ambient(glm::vec3(1.f, 0.1f, 0.07f));
		m_material->set_diffuse(glm::vec3(1.f, 0.1f, 0.07f));
	}


	//===============================================================Effects Update============================================================================

	m_explosion->on_update(time_step);

	m_jetpack_trail.on_update(time_step);

	m_cross_fade->on_update(time_step);

	//jetpack particle logic
	if (jetpackHoverOn) {

		jetpackTrailReady = false;
		if (jetpackTrailTimer < 0.0f) {
			jetpackTrailTimer = 0.03f;
		}
	}

	if (engine::input::key_pressed(engine::key_codes::KEY_SPACE))
	{
		if (jetpackTrailReady) {

			jetpackTrailReady = false;
			jetpackTrailTimer = 0.02f;
		}
	}

	if (jetpackTrailTimer > 0.0f)
	{
		jetpackTrailTimer -= (float)time_step;

		if (jetpackTrailTimer < 0.0f)
		{
			jetpackTrailReady = true;
			m_jetpack_trail.initialise(m_player.object()->position() + glm::vec3(0.f, -0.15f, 0.f), glm::vec3(0.f, 0.f, 0.f), 1, 1.f, 0.3f);
		}
	}

	//putting shooter enemies down here, otherwise an issue with some update ordering causing explosion to appear infront of the shooter
	m_enemy_mech.on_update(time_step, m_player.object()->position(), enemy_missile);
	m_mech_box.on_update(m_mech->position() - glm::vec3(0.6f, m_mech->offset().y, 0.6f) * m_mech->scale(), m_mech->rotation_amount(), m_mech->rotation_axis());
	
	m_enemy_droid.on_update(time_step, m_player.object()->position(), enemy_missile2);
	m_droid_box.on_update(m_droid->position() - glm::vec3(0.8f, m_droid->offset().y, 0.8f) * m_droid->scale(), m_droid->rotation_amount(), m_droid->rotation_axis());

	if (m_enemy_drone.get_health_point() <= 0) {
		m_enemy_drone.object()->set_position(glm::vec3(0.f, -9.f, 0.f));
		m_enemy_mech.object()->set_forward(glm::vec3(0.f, 0.f, -1.f));
		m_enemy_drone.reset_health();
	}
	if (m_enemy_bb8.get_health_point() <= 0) {
		m_enemy_bb8.object()->set_position(glm::vec3(2.f, -9.f, 0.f));
		m_enemy_mech.object()->set_forward(glm::vec3(0.f, 0.f, -1.f));
		m_enemy_bb8.reset_health();
	}
	if (m_enemy_droid.get_health_point() <= 0) {
		m_enemy_droid.object()->set_position(glm::vec3(4.f, -9.f, 0.f));
		m_enemy_mech.object()->set_forward(glm::vec3(0.f, 0.f, -1.f));
		m_enemy_droid.reset_health();
	}
	if (m_enemy_mech.get_health_point() <= 0) {
		m_enemy_mech.object()->set_position(glm::vec3(5.f, -9.f, 0.f));
		m_enemy_mech.object()->set_forward(glm::vec3(0.f, 0.f, -1.f));
		m_enemy_mech.reset_health();
	}
}

void example_layer::on_event(engine::event& event)
{
	if (event.event_type() == engine::event_type_e::key_pressed)
	{
		auto& e = dynamic_cast<engine::key_pressed_event&>(event);
		if (e.key_code() == engine::key_codes::KEY_TAB)
		{
			engine::render_command::toggle_wireframe();
		}
		if (e.key_code() == engine::key_codes::KEY_C) {
			if (CamMode != CamState::FirstPerson) {

				CamMode = CamState::FirstPerson;
				m_3d_camera.reset_camera();
			}
			else if (CamMode != CamState::ThirdPerson) {

				CamMode = CamState::ThirdPerson;
				m_3d_camera.reset_camera();
			}
		}
		if (e.key_code() == engine::key_codes::KEY_T) {
			CamMode = CamState::FreeView;
		}
		if (e.key_code() == engine::key_codes::KEY_F) {
			jetpackHoverOn = m_player.hover();
		}
		if (e.key_code() == engine::key_codes::KEY_LEFT_SHIFT)
		{
			m_player.sprint(true);
		}
		if (e.key_code() == engine::key_codes::KEY_ENTER)
		{
			m_intro_screen->deactivate();
		}
		if (e.key_code() == engine::key_codes::KEY_1)
		{
			WeaponSlot = WeaponState::Slot1;
		}
		if (e.key_code() == engine::key_codes::KEY_2)
		{
			WeaponSlot = WeaponState::Slot2;
		}

		//======================Remove these in final ver=======================
		if (e.key_code() == engine::key_codes::KEY_G)
		{
			if (WeaponSlot == WeaponState::Slot1) {
				missile.fire(m_3d_camera, 180.0f, m_player.object()->position());
				missile.set_active(true);
			}
			else if (WeaponSlot == WeaponState::Slot2) {
				bouncynade.fire(m_3d_camera, 60.0f, m_player.object()->position());
				bouncynade_armed = false;
			}
		}
		if (e.key_code() == engine::key_codes::KEY_H)
		{
			bouncynade.fire(m_3d_camera, 60.0f, m_player.object()->position());
			bouncynade_armed = false;
		}
		if (e.key_code() == engine::key_codes::KEY_9)
		{
			m_cross_fade->activate();
		}
		if (e.key_code() == engine::key_codes::KEY_0)
		{
			enemy_missile2.enemy_fire(m_enemy_droid, 180.0f);
			enemy_missile2.set_active(true);
		}
		//======================================================================

	}

	if (event.event_type() == engine::event_type_e::key_released)
	{
		auto& e = dynamic_cast<engine::key_released_event&>(event);
		if (e.key_code() == engine::key_codes::KEY_LEFT_SHIFT)
		{
			m_player.sprint(false);
		}
	}

	if (event.event_type() == engine::event_type_e::mouse_button_pressed)
	{
		if (engine::input::mouse_button_pressed(0)) {
			if (WeaponSlot == WeaponState::Slot1) {
				missile.fire(m_3d_camera, 180.0f, m_player.object()->position());
				missile.set_active(true);
			}
			else if (WeaponSlot == WeaponState::Slot2) {
				bouncynade.fire(m_3d_camera, 60.0f, m_player.object()->position());
				bouncynade_armed = false;
			}
		}
	}
}

void example_layer::check_bounce()
{
	//if (m_prev_sphere_y_vel < 0.f && m_game_objects.at(1)->velocity().y > 0.f)
		//m_audio_manager->play("bounce");
	m_prev_sphere_y_vel = m_game_objects.at(1)->velocity().y;
}

void example_layer::damage_player(int damage) {

	m_cross_fade->activate();
	m_player.set_health_point(m_player.get_health_point() - damage);
	player_immunity_timer = immune_time;
}
