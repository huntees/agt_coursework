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

	m_directionalLight.Color = glm::vec3(1.0f, 1.0f, 1.0f);
	m_directionalLight.AmbientIntensity = 0.25f;
	m_directionalLight.DiffuseIntensity = 0.6f;
	m_directionalLight.Direction = glm::normalize(glm::vec3(1.0f, -1.0f, 0.0f));

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


	// Skybox texture from http://www.vwall.it/wp-content/plugins/canvasio3dpro/inc/resource/cubeMaps/
	m_skybox = engine::skybox::create(50.f,
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
	mannequin_props.position = glm::vec3(3.0f, 0.5f, -5.0f);
	mannequin_props.type = 0;
	mannequin_props.bounding_shape = m_skinned_mesh->size() / 2.f * mannequin_props.scale.x;
	m_mannequin = engine::game_object::create(mannequin_props);

	// initialise player mesh
	m_player.initialise(m_mannequin);

	// Load the terrain texture and create a terrain mesh. Create a terrain object. Set its properties
	std::vector<engine::ref<engine::texture_2d>> terrain_textures = { engine::texture_2d::create("assets/textures/Slabs.jpg", false) };
	engine::ref<engine::terrain> terrain_shape = engine::terrain::create(100.f, 0.5f, 100.f);
	engine::game_object_properties terrain_props;
	terrain_props.meshes = { terrain_shape->mesh() };
	terrain_props.textures = terrain_textures;
	terrain_props.is_static = true;
	terrain_props.type = 0;
	terrain_props.bounding_shape = glm::vec3(100.f, 0.5f, 100.f);
	terrain_props.restitution = 0.92f;
	m_terrain = engine::game_object::create(terrain_props);

	// Load the cow model. Create a cow object. Set its properties
	engine::ref <engine::model> cow_model = engine::model::create("assets/models/static/cow4.3ds");
	engine::game_object_properties cow_props;
	cow_props.meshes = cow_model->meshes();
	cow_props.textures = cow_model->textures();
	float cow_scale = 1.f / glm::max(cow_model->size().x, glm::max(cow_model->size().y, cow_model->size().z));
	cow_props.position = { -4.f,0.5f, -5.f };
	cow_props.scale = glm::vec3(cow_scale);
	cow_props.bounding_shape = cow_model->size() / 2.f * cow_scale;
	m_cow = engine::game_object::create(cow_props);

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
	office_props.position = { 25.f, 0.5f, 0.f };
	m_office = engine::game_object::create(office_props);

	//Load sanfran model
	engine::ref <engine::model> sanfran_model = engine::model::create("assets/models/static/sanfran/sanfran.obj");
	engine::game_object_properties sanfran_props;
	sanfran_props.meshes = sanfran_model->meshes();
	sanfran_props.textures = sanfran_model->textures();
	sanfran_props.position = { -20.f, 1.72f, 10.f };
	m_sanfran = engine::game_object::create(sanfran_props);

	//Load skyscrapers model
	engine::ref <engine::model> skyscrapers_model = engine::model::create("assets/models/static/skyscrapers/skyscrapers.obj");
	engine::game_object_properties skyscrapers_props;
	skyscrapers_props.meshes = skyscrapers_model->meshes();
	skyscrapers_props.textures = skyscrapers_model->textures();
	skyscrapers_props.position = { -25.f, -3.5f, 33.f };
	m_skyscrapers = engine::game_object::create(skyscrapers_props);

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

	// Load sphere
	engine::ref<engine::sphere> sphere_shape = engine::sphere::create(10, 20, 0.5f);
	engine::game_object_properties sphere_props;
	sphere_props.position = { 0.f, 5.f, -5.f };
	sphere_props.meshes = { sphere_shape->mesh() };
	sphere_props.type = 1;
	sphere_props.bounding_shape = glm::vec3(0.5f);
	sphere_props.restitution = 0.92f;
	sphere_props.mass = 0.000001f;
	m_ball = engine::game_object::create(sphere_props);

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
	m_game_objects.push_back(m_ball);
	//m_game_objects.push_back(m_cow);
	//m_game_objects.push_back(m_tree);
	//m_game_objects.push_back(m_pickup);
	m_physics_manager = engine::bullet_manager::create(m_game_objects);

	m_text_manager = engine::text_manager::create();

	m_skinned_mesh->switch_animation(1);

	//Load Intro Screen Texture
	m_intro_screen = intro_screen::create("assets/textures/tempmenu.jpg", 1.6f, 0.9f);
}

example_layer::~example_layer() {}

void example_layer::on_update(const engine::timestep& time_step)
{
	m_intro_screen->on_update(time_step);


	if (engine::input::key_pressed(engine::key_codes::KEY_C)) {
		if (firstPerson && camSwitchDelayReady) {

			firstPerson = false;
			camSwitchDelayReady = false;
			camSwitchTimer = 0.15f;
			m_3d_camera.reset_camera();
		}
		else if (camSwitchDelayReady) {

			firstPerson = true;
			camSwitchDelayReady = false;
			camSwitchTimer = 0.15f;
			m_3d_camera.reset_camera();
		}
	}

	if (firstPerson) {
		//m_3d_camera.on_update(time_step);
		m_player.update_first_person_camera(m_3d_camera);
	}
	else {
		m_player.update_third_person_camera(m_3d_camera);
	}

	if (camSwitchTimer > 0.0f)
	{
		camSwitchTimer -= (float)time_step;

		if (camSwitchTimer < 0.0f)
		{
			camSwitchDelayReady = true;
		}
	}


	//m_player.update_camera(m_3d_camera);

	m_player.on_update(time_step);

	m_physics_manager->dynamics_world_update(m_game_objects, double(time_step));

	check_bounce();

}

void example_layer::on_render()
{
	engine::render_command::clear_color({ 0.2f, 0.3f, 0.3f, 1.0f });
	engine::render_command::clear();

	//const auto textured_shader = engine::renderer::shaders_library()->get("mesh_static");
	//engine::renderer::begin_scene(m_3d_camera, textured_shader);

	const auto textured_lighting_shader = engine::renderer::shaders_library()->get("mesh_lighting");


	//----------------------------------------------------3D Cam-------------------------------------------------------------------------
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

	engine::renderer::submit(textured_lighting_shader, m_tetrahedron);

	glm::mat4 benchTransform = glm::mat4(1.0f);
	benchTransform = glm::translate(benchTransform, glm::vec3(9.8f, 0.9f, 0.f));
	benchTransform = glm::scale(benchTransform, m_bench->scale());
	engine::renderer::submit(textured_lighting_shader, benchTransform, m_bench);

	glm::mat4 benchTransform2 = glm::mat4(1.0f);
	benchTransform2 = glm::translate(benchTransform2, glm::vec3(15.8f, 0.9f, 0.f));
	benchTransform2 = glm::scale(benchTransform2, m_bench->scale());
	engine::renderer::submit(textured_lighting_shader, benchTransform2, m_bench);

	float bench = 0;
	for (int i = 0; i <= 3; ++i) {

		if (i > 1) {
			bench = 7.f;
		}

		glm::mat4 benchTransform3 = glm::mat4(1.0f);
		benchTransform3 = glm::translate(benchTransform3, glm::vec3(-12.3f, 0.9f, -10.7f + (i * 5.7) + bench));
		benchTransform3 = glm::rotate(benchTransform3, m_bench->rotation_amount() + glm::radians(90.f), glm::vec3(0.f, 1.f, 0.f));
		benchTransform3 = glm::scale(benchTransform3, m_bench->scale());
		engine::renderer::submit(textured_lighting_shader, benchTransform3, m_bench);
	}

	glm::mat4 hexagontransform = glm::mat4(1.0f);
	hexagontransform = glm::translate(hexagontransform, glm::vec3(13.8f, 0.51f, 5.f));
	hexagontransform = glm::rotate(hexagontransform, m_hexagon->rotation_amount() + glm::radians(270.f), glm::vec3(1.f, 0.f, 0.f));
	hexagontransform = glm::scale(hexagontransform, m_hexagon->scale());
	engine::renderer::submit(textured_lighting_shader, hexagontransform, m_hexagon);

	glm::mat4 hexagontransform2 = glm::mat4(1.0f);
	hexagontransform2 = glm::translate(hexagontransform2, glm::vec3(13.8f, 0.51f, 15.f));
	hexagontransform2 = glm::rotate(hexagontransform2, m_hexagon->rotation_amount() + glm::radians(270.f), glm::vec3(1.f, 0.f, 0.f));
	hexagontransform2 = glm::scale(hexagontransform2, m_hexagon->scale());
	engine::renderer::submit(textured_lighting_shader, hexagontransform2, m_hexagon);

	//office trees
	for (int i = 0; i <= 3; ++i) {

		glm::mat4 tree_transform(1.0f);
		tree_transform = glm::translate(tree_transform, glm::vec3(9.5f + i * 3, 0.5, -1.0f));
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
		tree_transform = glm::translate(tree_transform, glm::vec3(-13.5f, 0.5, -13.5f + (i * 3) + tree));
		tree_transform = glm::rotate(tree_transform, m_tree->rotation_amount(), m_tree->rotation_axis());
		tree_transform = glm::scale(tree_transform, m_tree->scale());
		engine::renderer::submit(textured_lighting_shader, tree_transform, m_tree);
	}

	//glm::mat4 tree_transform(1.0f);
	//tree_transform = glm::translate(tree_transform, glm::vec3(4.f, 0.5, -5.0f));
	//tree_transform = glm::rotate(tree_transform, m_tree->rotation_amount(), m_tree->rotation_axis());
	//tree_transform = glm::scale(tree_transform, m_tree->scale());
	//engine::renderer::submit(textured_lighting_shader, tree_transform, m_tree);

	glm::mat4 cow_transform(1.0f);
	cow_transform = glm::translate(cow_transform, glm::vec3(9.3f, 0.5f, -0.2f));
	cow_transform = glm::rotate(cow_transform, m_cow->rotation_amount() + glm::radians(90.f), glm::vec3(0.f, 1.f, 0.f));
	cow_transform = glm::scale(cow_transform, m_cow->scale());
	engine::renderer::submit(textured_lighting_shader, cow_transform, m_cow);

	float jeep_x_rotation = 90.f * (glm::pi<float>() / 180.f); //90 degrees multiplied by pi/180 giving radians

	glm::mat4 jeep_transform(1.0f);
	jeep_transform = glm::translate(jeep_transform, m_jeep->position() + glm::vec3(-4.f, 0.f, 0.f));
	jeep_transform = glm::rotate(jeep_transform, m_jeep->rotation_amount(), m_jeep->rotation_axis());
	jeep_transform = glm::scale(jeep_transform, m_jeep->scale());
	engine::renderer::submit(textured_lighting_shader, jeep_transform, m_jeep);

	glm::mat4 pizza_transform(1.0f);
	pizza_transform = glm::translate(pizza_transform, glm::vec3(10.2f, 0.92f, -0.2f));
	pizza_transform = glm::rotate(pizza_transform, m_pizza->rotation_amount() + glm::radians(120.f), glm::vec3(0.f, 1.f, 0.f));
	pizza_transform = glm::scale(pizza_transform, m_pizza->scale());
	engine::renderer::submit(textured_lighting_shader, pizza_transform, m_pizza);

	glm::mat4 pizza_transform2(1.0f);
	pizza_transform2 = glm::translate(pizza_transform2, glm::vec3(11.5f, 0.92f, -0.2f));
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
	jet_transform3 = glm::translate(jet_transform3, glm::vec3(-2.f, 40.f, 18.f));
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

	engine::renderer::end_scene();

	// Set up material shader. (does not render textures, renders materials instead)
	const auto material_shader = engine::renderer::shaders_library()->get("mesh_material");
	engine::renderer::begin_scene(m_3d_camera, material_shader);

	m_material->submit(material_shader);
	std::dynamic_pointer_cast<engine::gl_shader>(material_shader)->set_uniform("gEyeWorldPos", m_3d_camera.position());

	engine::renderer::submit(material_shader, m_ball);

	engine::renderer::end_scene();

	const auto animated_mesh_shader = engine::renderer::shaders_library()->get("animated_mesh");
	engine::renderer::begin_scene(m_3d_camera, animated_mesh_shader);
	std::dynamic_pointer_cast<engine::gl_shader>(animated_mesh_shader)->set_uniform("gEyeWorldPos", m_3d_camera.position());

	glm::mat4 aniTransform = glm::mat4(1.0f);

	engine::renderer::submit(animated_mesh_shader, m_player.object());

	engine::renderer::end_scene();
	//----------------------------------------------------3D Cam End--------------------------------------------------------------------------

	// Render text
	const auto text_shader = engine::renderer::shaders_library()->get("text_2D");
	m_text_manager->render_text(text_shader, "Orange Text", 10.f, (float)engine::application::window().height() - 25.f, 0.5f, glm::vec4(1.f, 0.5f, 0.f, 1.f));

	//----------------------------------------------------2D Cam-------------------------------------------------------------------------------

	//old code for turning lighting on and off
	//const auto hud_lighting_shader = engine::renderer::shaders_library()->get("mesh_lighting");
	//std::dynamic_pointer_cast<engine::gl_shader>(textured_lighting_shader)->bind();
	//std::dynamic_pointer_cast<engine::gl_shader>(textured_lighting_shader)->set_uniform("lighting_on", false);

	engine::renderer::begin_scene(m_2d_camera, textured_lighting_shader);

	std::dynamic_pointer_cast<engine::gl_shader>(textured_lighting_shader)->set_uniform("lighting_on", false);

	m_intro_screen->on_render(textured_lighting_shader);

	std::dynamic_pointer_cast<engine::gl_shader>(textured_lighting_shader)->set_uniform("lighting_on", true);
	engine::renderer::end_scene();
	//----------------------------------------------------2D Cam End--------------------------------------------------------------------------
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
		if (e.key_code() == engine::key_codes::KEY_LEFT_SHIFT)
		{
			m_player.sprint(true);
		}
		if (e.key_code() == engine::key_codes::KEY_SPACE)
		{
			m_player.jump();
		}
		if (e.key_code() == engine::key_codes::KEY_ENTER)
		{
			m_intro_screen->deactivate();
		}

	}

	if (event.event_type() == engine::event_type_e::key_released)
	{
		auto& e = dynamic_cast<engine::key_released_event&>(event);
		if (e.key_code() == engine::key_codes::KEY_LEFT_SHIFT)
		{
			m_player.sprint(false);
		}
	}
}

void example_layer::check_bounce()
{
	//if (m_prev_sphere_y_vel < 0.f && m_game_objects.at(1)->velocity().y > 0.f)
		//m_audio_manager->play("bounce");
	m_prev_sphere_y_vel = m_game_objects.at(1)->velocity().y;
}
