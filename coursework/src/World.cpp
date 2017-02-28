#include "World.h"

World::World()
{
	// Empty constructor
	/** The init() fuction will be called for initialization instead of constructor! **/
}

bool World::init_cursor()
{
	glfwSetInputMode(renderer::get_window(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwGetCursorPos(renderer::get_window(), &cursor_x, &cursor_y);
	
	return true;
}

void World::create_terrain()
{
	meshes["terrain"] = mesh(geometry_builder::create_cylinder(100, 100, vec3(1.0f)));
	meshes["terrain"].get_transform().scale = vec3(1100.0f, 0.0f, 1100.0f);
}

void World::create_skybox()
{
	// Create box geometry for skybox
	skybox = mesh(geometry_builder::create_box());
	// Scale box by 100
	skybox.get_transform().scale = vec3(800.0f, 600.0f, 800.0f);
	skybox.get_transform().translate(vec3(0.0f));
	// Load the cubemap
	array<string, 6> filenames = {
		"textures/crim_sky/criminal-intentions_ft.tga", "textures/crim_sky/criminal-intentions_bk.tga",
		"textures/crim_sky/criminal-intentions_up.tga", "textures/crim_sky/criminal-intentions_dn.tga",
		"textures/crim_sky/criminal-intentions_rt.tga", "textures/crim_sky/criminal-intentions_lf.tga"
	};
	// Create cube_map
	cube_map = cubemap(filenames);
}

void World::render_skybox()
{
	// Render skybox
	glDisable(GL_DEPTH_TEST);
	glDepthMask(GL_FALSE);
	glDisable(GL_CULL_FACE);

	// Bind skybox effect
	renderer::bind(sky_eff);

	// Calculate MVP for the skybox
	mat4 M = skybox.get_transform().get_transform_matrix();
	mat4 V = mainCam.get_free_camera().get_view();
	mat4 P = mainCam.get_free_camera().get_projection();
	mat4 MVP = P * V * M;

	// Set MVP matrix uniform
	glUniformMatrix4fv(sky_eff.get_uniform_location("MVP"), 1, GL_FALSE, value_ptr(MVP));

	// Set cubemap uniform
	renderer::bind(cube_map, 0);
	glUniform1i(sky_eff.get_uniform_location("cubemap"), 0);

	// Render skybox
	renderer::render(skybox);

	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
	glEnable(GL_CULL_FACE);
}

void World::render_shadow()
{
	// TODO: Implement...
}

void World::create_base_colours()
{
	basic_colours["black"] = vec4(0.0f, 0.0f, 0.0f, 1.0f);
	basic_colours["white"] = vec4(1.0f, 1.0f, 1.0f, 1.0f);
	basic_colours["red"] = vec4(1.0f, 0.0f, 0.0f, 1.0f);
	basic_colours["green"] = vec4(0.0f, 1.0f, 0.0f, 1.0f);
	basic_colours["blue"] = vec4(0.0f, 0.0f, 1.0f, 1.0f);
	basic_colours["yellow"] = vec4(1.0f, 1.0f, 0.0f, 1.0f);
}

void World::load_shaders()
{
	// list of all the vertex shaders to be called
	vector<string> vertex_shaders
	{
		"shaders/multi_lights.vert"
	};
	// list of all the fragment shaders to be called
	vector<string> fragment_shaders
	{
		"shaders/multi_lights.frag",
		"shaders/part_direction.frag",
		"shaders/part_point.frag",
		"shaders/part_spot.frag",
		"shaders/part_shadow.frag",
		"shaders/part_normal_map.frag"
	};

	// Load general shaders files
	eff.add_shader(vertex_shaders, GL_VERTEX_SHADER);
	eff.add_shader(fragment_shaders, GL_FRAGMENT_SHADER);
	// Load shadow effect
	shadow_eff.add_shader("shaders/spot.vert", GL_VERTEX_SHADER);
	shadow_eff.add_shader("shaders/spot.frag", GL_FRAGMENT_SHADER);
	// Load in skybox effect
	sky_eff.add_shader("shaders/skybox.vert", GL_VERTEX_SHADER);
	sky_eff.add_shader("shaders/skybox.frag", GL_FRAGMENT_SHADER);

	// Build shader programs
	eff.build();
	shadow_eff.build();
	sky_eff.build();
}

void World::load_textures()
{
	// ********************************************
	texs[0] = texture("textures/snow.jpg");
	texs[1] = texture("textures/wall.tga");
	texs[2] = texture("textures/sun.jpg");
	texs[3] = texture("textures/brick.jpg");
	texs[4] = texture("textures/snow.jpg");

	blend_map = texture("textures/blend_map1.png");
	
	normal_maps.resize(2);
	normal_maps[0] = texture("textures/wall_norm.tga");
	normal_maps[1] = texture("textures/brick_normalmap.jpg");
	// ********************************************
}


bool World::load_content()
{
	// Create shadow map - use screen size
	shadow = shadow_map(renderer::get_screen_width(), renderer::get_screen_height());

	// Create terrain
	create_terrain();

	// Create basic colours
	create_base_colours();

	// Set materials
	vec4 emissive = basic_colours["black"];
	vec4 specular = basic_colours["white"];
	float shininess = 10.0f;

	/*** Meshes ***/
	// Sun mesh
	meshes["sun"] = mesh(geometry_builder::create_sphere(100U, 100U));
	meshes["sun"].get_transform().translate(vec3(400.0f, 400.0f, 200.0f));
	meshes["sun"].get_transform().rotate(vec3(-pi<float>(), pi<float>(), 0.0f));
	meshes["sun"].get_transform().scale = vec3(10.0, 10.0f, 10.0f);
	meshes["sun"].get_material().set_diffuse(vec4(1.0f, 1.0f, 1.0f, 1.0f));
	meshes["sun"].get_material().set_specular(specular);
	meshes["sun"].get_material().set_shininess(shininess);

	// House mesh
	meshes["house1"] = mesh(geometry("models/medieval-house/medieval-house.3DS"));
	meshes["house1"].get_transform().scale = vec3(1.5f, 1.5f, 1.5f);
	meshes["house1"].get_transform().translate(vec3(100.0f, 0.0f, 100.0f));
	meshes["house1"].get_transform().rotate(vec3(-half_pi<float>(), 0.0f, 0.0f));
	meshes["house1"].get_material().set_emissive(emissive);
	meshes["house1"].get_material().set_diffuse(basic_colours["white"]);
	meshes["house1"].get_material().set_specular(specular);
	meshes["house1"].get_material().set_shininess(shininess);

	// House mesh
	meshes["house2"] = mesh(geometry("models/medieval-house/medieval-house.3DS"));
	meshes["house2"].get_transform().scale = vec3(1.5f, 1.5f, 1.5f);
	meshes["house2"].get_transform().translate(vec3(-100.0f, 0.0f, -100.0f));
	meshes["house2"].get_transform().rotate(vec3(-half_pi<float>(), 0.0f, 0.0f));
	meshes["house2"].get_material().set_emissive(emissive);
	meshes["house2"].get_material().set_diffuse(basic_colours["white"]);
	meshes["house2"].get_material().set_specular(specular);
	meshes["house2"].get_material().set_shininess(shininess);

	// Academy Fort mesh
	meshes["FortWall1x1"] = mesh(geometry_builder::create_box());
	meshes["FortWall1x1"].get_transform().translate(vec3(114.0f, 25.0f, 200.0f));
	meshes["FortWall1x1"].get_transform().scale = vec3(76.0, 50.0f, 10.0f);
	meshes["FortWall1x2"] = mesh(geometry_builder::create_box());
	meshes["FortWall1x2"].get_transform().translate(vec3(38.0f, 25.0f, 200.0f));
	meshes["FortWall1x2"].get_transform().scale = vec3(76.0, 50.0f, 10.0f);
	meshes["FortWall1x3"] = mesh(geometry_builder::create_box());
	meshes["FortWall1x3"].get_transform().translate(vec3(-38.0f, 25.0f, 200.0f));
	meshes["FortWall1x3"].get_transform().scale = vec3(76.0, 50.0f, 10.0f);
	meshes["FortWall1x4"] = mesh(geometry_builder::create_box());
	meshes["FortWall1x4"].get_transform().translate(vec3(-114.0f, 25.0f, 200.0f));
	meshes["FortWall1x4"].get_transform().scale = vec3(76.0, 50.0f, 10.0f);

	meshes["FortWall2x1"] = mesh(geometry_builder::create_box());
	meshes["FortWall2x1"].get_transform().translate(vec3(114.0f, 25.0f, -200.0f));
	meshes["FortWall2x1"].get_transform().scale = vec3(76.0f, 50.0f, 10.0f);
	meshes["FortWall2x2"] = mesh(geometry_builder::create_box());
	meshes["FortWall2x2"].get_transform().translate(vec3(38.0f, 25.0f, -200.0f));
	meshes["FortWall2x2"].get_transform().scale = vec3(76.0f, 50.0f, 10.0f);
	meshes["FortWall2x3"] = mesh(geometry_builder::create_box());
	meshes["FortWall2x3"].get_transform().translate(vec3(-38.0f, 25.0f, -200.0f));
	meshes["FortWall2x3"].get_transform().scale = vec3(76.0f, 50.0f, 10.0f);
	meshes["FortWall2x4"] = mesh(geometry_builder::create_box());
	meshes["FortWall2x4"].get_transform().translate(vec3(-114.0f, 25.0f, -200.0f));
	meshes["FortWall2x4"].get_transform().scale = vec3(76.0f, 50.0f, 10.0f);

	meshes["FortWall3x1"] = mesh(geometry_builder::create_box());
	meshes["FortWall3x1"].get_transform().translate(vec3(-200.0f, 25.0f, 114.0f));
	meshes["FortWall3x1"].get_transform().scale = vec3(10.0f, 50.0f, 76.0f);
	meshes["FortWall3x2"] = mesh(geometry_builder::create_box());
	meshes["FortWall3x2"].get_transform().translate(vec3(-200.0f, 25.0f, 38.0f));
	meshes["FortWall3x2"].get_transform().scale = vec3(10.0f, 50.0f, 76.0f);
	meshes["FortWall3x3"] = mesh(geometry_builder::create_box());
	meshes["FortWall3x3"].get_transform().translate(vec3(-200.0f, 25.0f, -38.0f));
	meshes["FortWall3x3"].get_transform().scale = vec3(10.0f, 50.0f, 76.0f);
	meshes["FortWall3x4"] = mesh(geometry_builder::create_box());
	meshes["FortWall3x4"].get_transform().translate(vec3(-200.0f, 25.0f, -114.0f));
	meshes["FortWall3x4"].get_transform().scale = vec3(10.0f, 50.0f, 76.0f);

	meshes["FortWallSmall1"] = mesh(geometry_builder::create_box());
	meshes["FortWallSmall1"].get_transform().translate(vec3(200.0f, 25.0f, 114.0f));
	meshes["FortWallSmall1"].get_transform().scale = vec3(10.0f, 50.0f, 76.0f);
	meshes["FortWallSmall2"] = mesh(geometry_builder::create_box());
	meshes["FortWallSmall2"].get_transform().translate(vec3(200.0f, 25.0f, -114.0f));
	meshes["FortWallSmall2"].get_transform().scale = vec3(10.0f, 50.0f, 76.0f);

	meshes["FortWallGate1"] = mesh(geometry_builder::create_box());
	meshes["FortWallGate1"].get_transform().translate(vec3(200.0f, 55.0f, -57.0f));
	meshes["FortWallGate1"].get_transform().scale = vec3(10.0f, 10.0f, 38.0f);
	meshes["FortWallGate2"] = mesh(geometry_builder::create_box());
	meshes["FortWallGate2"].get_transform().translate(vec3(200.0f, 55.0f, -19.0f));
	meshes["FortWallGate2"].get_transform().scale = vec3(10.0f, 10.0f, 38.0f);
	meshes["FortWallGate3"] = mesh(geometry_builder::create_box());
	meshes["FortWallGate3"].get_transform().translate(vec3(200.0f, 55.0f, 19.0f));
	meshes["FortWallGate3"].get_transform().scale = vec3(10.0f, 10.0f, 38.0f);
	meshes["FortWallGate4"] = mesh(geometry_builder::create_box());
	meshes["FortWallGate4"].get_transform().translate(vec3(200.0f, 55.0f, 57.0f));
	meshes["FortWallGate4"].get_transform().scale = vec3(10.0f, 10.0f, 38.0f);

	meshes["FortWallGateSmall1"] = mesh(geometry_builder::create_box());
	meshes["FortWallGateSmall1"].get_transform().translate(vec3(200.0f, 25.0f, -71.0f));
	meshes["FortWallGateSmall1"].get_transform().scale = vec3(10.0f, 50.0f, 10.0f);

	meshes["FortWallGateSmall2"] = mesh(geometry_builder::create_box());
	meshes["FortWallGateSmall2"].get_transform().translate(vec3(200.0f, 25.0f, 71.0f));
	meshes["FortWallGateSmall2"].get_transform().scale = vec3(10.0f, 50.0f, 10.0f);

	meshes["FortWallGateTop1"] = mesh(geometry_builder::create_sphere(20, 20));
	meshes["FortWallGateTop1"].get_transform().translate(vec3(200.0f, 69.0f, 70.0f));
	meshes["FortWallGateTop1"].get_transform().scale = vec3(10.0f, 10.0f, 10.0f);

	meshes["FortWallGateTop2"] = mesh(geometry_builder::create_sphere(20, 20));
	meshes["FortWallGateTop2"].get_transform().translate(vec3(200.0f, 69.0f, -70.0f));
	meshes["FortWallGateTop2"].get_transform().scale = vec3(10.0f, 10.0f, 10.0f);

	meshes["FortWallSide1"] = mesh(geometry_builder::create_box());
	meshes["FortWallSide1"].get_transform().translate(vec3(-175.0f, 25.0f, 175.0f));
	meshes["FortWallSide1"].get_transform().scale = vec3(75.0f, 50.0f, 10.0f);
	meshes["FortWallSide1"].get_transform().rotate(vec3(0.0f, -quarter_pi<float>(), 0.0f));
	meshes["FortWallSide2"] = mesh(geometry_builder::create_box());
	meshes["FortWallSide2"].get_transform().translate(vec3(175.0f, 25.0f, 175.0f));
	meshes["FortWallSide2"].get_transform().scale = vec3(75.0f, 50.0f, 10.0f);
	meshes["FortWallSide2"].get_transform().rotate(vec3(0.0f, quarter_pi<float>(), 0.0f));
	meshes["FortWallSide3"] = mesh(geometry_builder::create_box());
	meshes["FortWallSide3"].get_transform().translate(vec3(-175.0f, 25.0f, -175.0f));
	meshes["FortWallSide3"].get_transform().scale = vec3(75.0f, 50.0f, 10.0f);
	meshes["FortWallSide3"].get_transform().rotate(vec3(0.0f, quarter_pi<float>(), 0.0f));
	meshes["FortWallSide4"] = mesh(geometry_builder::create_box());
	meshes["FortWallSide4"].get_transform().translate(vec3(175.0f, 25.0f, -175.0f));
	meshes["FortWallSide4"].get_transform().scale = vec3(75.0f, 50.0f, 10.0f);
	meshes["FortWallSide4"].get_transform().rotate(vec3(0.0f, -quarter_pi<float>(), 0.0f));

	// Create skybox
	create_skybox();

	/*** Lights ***/
	points.resize(3);
	spots.resize(1);

	// Directional light
	light.set_ambient_intensity(vec4(0.1f, 0.1f, 0.1f, 0.1f));
	light.set_light_colour(basic_colours["black"]);
	light.set_direction(-(meshes["sun"].get_transform().position));

	// Point lights
	points[0].set_position(vec3(meshes["sun"].get_transform().position.x-40.0f, meshes["sun"].get_transform().position.y+20.0f, meshes["sun"].get_transform().position.z-0.0f));
	points[0].set_light_colour(vec4(basic_colours["white"]));
	points[0].set_range(100.0f);
	
	points[1].set_position(vec3(10.0, 30.0f, 10.0f));
	points[1].set_light_colour(vec4(basic_colours["white"]));
	points[1].set_range(50.0f);

	points[2].set_position(vec3(-20.0, 30.0f, -20.0f));
	points[2].set_light_colour(vec4(basic_colours["white"]));
	points[2].set_range(50.0f);

	// Spot lights
	spots[0].set_position(vec3(meshes["house1"].get_transform().position) + vec3(15.0f, 5.0f, 5.0f));
	spots[0].set_light_colour(basic_colours["white"]);
	spots[0].set_direction(vec3(1.0f, 1.0f, -1.0f));
	spots[0].set_range(20.0f);
	spots[0].set_power(5.5f);

	// Load textures
	load_textures();

	// Load&Build shaders
	load_shaders();

	// Set camera properties
	mainCam.set_free_camera();
	
	return true;
}

bool World::update(float delta_time)
{
	// Rotate a mesh
	meshes["FortWallGateTop1"].get_transform().rotate(vec3(0.0f, -half_pi<float>(), 0.0f) * delta_time * 0.5f);
	meshes["FortWallGateTop2"].get_transform().rotate(vec3(0.0f, -half_pi<float>(), 0.0f) * delta_time * 0.5f);

	// Update the shadow map properties from the spot light
	shadow.light_position = spots[0].get_position();
	shadow.light_dir = spots[0].get_direction();

	/*** CAMERA ***/
	vec3 transVec;
	if (glfwGetKey(renderer::get_window(), GLFW_KEY_W)) {
		transVec += vec3(0.0f, 0.0f, 0.5f);
	}
	if (glfwGetKey(renderer::get_window(), GLFW_KEY_S)) {
		transVec += vec3(0.0f, 0.0f, -0.5f);
	}
	if (glfwGetKey(renderer::get_window(), GLFW_KEY_A)) {
		transVec += vec3(-0.5f, 0.0f, 0.0f);
	}
	if (glfwGetKey(renderer::get_window(), GLFW_KEY_D)) {
		transVec += vec3(0.5f, 0.0f, 0.0f);
	}
	
	// Update the camera
	mainCam.rotate_free_camera(get_cursor_x(), get_cursor_y());
	mainCam.move_free_camera(transVec);
	mainCam.update_free_camera(delta_time);
	// update cursor position
	set_cursor_pos();

	shadow.light_position += vec3(1.0f);

	return true;
}

bool World::render()
{
	// Render skybox
	render_skybox();

	// Set render target to shadow map
	renderer::set_render_target(shadow);
	// Clear depth buffer bit
	glClear(GL_DEPTH_BUFFER_BIT);
	// Set render mode to cull face
	glCullFace(GL_FRONT);

	// Render meshes
	for (auto &e : meshes)
	{
		auto m = e.second;
		// Bind shader
		renderer::bind(shadow_eff);
		// Create MVP matrix
		mat4 M = m.get_transform().get_transform_matrix();
		mat4 V = shadow.get_view();
		mat4 P = mainCam.get_free_camera().get_projection();
		mat4 MVP = P * V * M;
		
		// Set MVP matrix uniform
		glUniformMatrix4fv(shadow_eff.get_uniform_location("MVP"), 1, GL_FALSE, value_ptr(MVP));
		
		// Render mesh
		renderer::render(m);
	}
	// Set render target back to the screen
	renderer::set_render_target();
	// Set cull face to back
	glCullFace(GL_BACK);	

	// Render meshes
	for (auto &e : meshes)
	{
		mesh m = e.second;
		// Bind effect
		renderer::bind(eff);
		// Create MVP matrix
		mat4 M = m.get_transform().get_transform_matrix();
		mat4 V = mainCam.get_free_camera().get_view();
		mat4 P = mainCam.get_free_camera().get_projection();
		mat4 N = m.get_transform().get_normal_matrix();
		mat4 MVP = P * V * M;
		mat4 lightMVP = P * shadow.get_view() * M;

		// Set MVP matrix uniform
		glUniformMatrix4fv(eff.get_uniform_location("MVP"), 1, GL_FALSE, value_ptr(MVP));
		
		// Set M matrix uniform
		glUniformMatrix4fv(eff.get_uniform_location("M"), 1, GL_FALSE, value_ptr(M));

		// Set N matrix uniform - remember - 3x3 matrix
		glUniformMatrix3fv(eff.get_uniform_location("N"), 1, GL_FALSE, value_ptr(N));

		// Set light transform
		glUniformMatrix4fv(eff.get_uniform_location("lightMVP"), 1, GL_FALSE, value_ptr(lightMVP));

		// Bind material
		renderer::bind(m.get_material(), "mat");

		// Bind directional light
		renderer::bind(light, "light");

		// Bind point lights
		renderer::bind(points, "points");

		// Bind spot lights
		renderer::bind(spots, "spots");

		// Set eye position - Get this from active camera
		glUniform3fv(eff.get_uniform_location("eye_pos"), 1, value_ptr(mainCam.get_free_camera().get_position()));

		// Bind textures to mesh
		if (e.first == "terrain")
		{
			renderer::bind(texs[0], 1);
			glUniform1i(eff.get_uniform_location("tex"), 1);
		}
		else if (e.first == "sun")
		{
			renderer::bind(texs[2], 2);
			glUniform1i(eff.get_uniform_location("tex"), 2);
		}
		else if (e.first.find("FortWall") != string::npos)
		{
			renderer::bind(texs[3], 3);
			glUniform1i(eff.get_uniform_location("tex"), 3);
			
			// Bind shadow map texture
			renderer::bind(texs[3], 4);
			glUniform1i(eff.get_uniform_location("shadow_map"), 4);

			// Bind normal map texture
			renderer::bind(normal_maps[1], 5);
			glUniform1i(eff.get_uniform_location("normal_map"), 5);
		}
		else if (e.first.find("house") != string::npos)
		{
			renderer::bind(texs[1], 6);
			glUniform1i(eff.get_uniform_location("tex"), 6);
			
			// Bind normal map texture
			renderer::bind(normal_maps[0], 7);
			glUniform1i(eff.get_uniform_location("normal_map"), 7);
		}

		// Render mesh
		renderer::render(m);
	}
	
	return true;
}

void World::init()
{
	// Create application | Fullscreen | HD resolution
	app application("Graphics Coursework | Academy Fort @ Night", renderer::windowed, 1600, 900);
	
	// Set load content, update and render methods
	application.set_load_content([&]() { return load_content(); });
	application.set_initialise([&]() { return init_cursor(); });
	application.set_update([&](float delta_time) { return update(delta_time); });
	application.set_render([&]() { return render(); });

	// Run application
	application.run();
}