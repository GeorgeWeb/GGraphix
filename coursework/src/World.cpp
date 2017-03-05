#include "World.h"
#include <glm/gtx/rotate_vector.hpp>

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
	// Create plane mesh
	meshes["plane"] = mesh(geometry_builder::create_plane());
	meshes["plane"].get_transform().scale = vec3(1000.0f, 0.0f, 1000.0f);
	meshes["plane"].get_material().set_emissive(vec4(0.0f, 0.0f, 0.0f, 1.0f));
	meshes["plane"].get_material().set_diffuse(vec4(1.0f, 1.0f, 1.0f, 1.0f));
	meshes["plane"].get_material().set_specular(vec4(1.0f, 1.0f, 1.0f, 1.0f));
	meshes["plane"].get_material().set_shininess(25.0f);
}

void World::create_skybox()
{
	// Create box geometry for skybox
	skybox = mesh(geometry_builder::create_box());
	// Scale box by 100
	skybox.get_transform().scale = vec3(800.0f);
	skybox.get_transform().translate(vec3(0.0f));
	// Load the cubemap
	array<string, 6> filenames = {
		"textures/night_sky/nightsky_ft.tga", "textures/night_sky/nightsky_bk.tga",
		"textures/night_sky/nightsky_up.tga", "textures/night_sky/nightsky_dn.tga",
		"textures/night_sky/nightsky_rt.tga", "textures/night_sky/nightsky_lf.tga"
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
	mat4 V = cams["freeCam"] ? mainCam.get_free_camera().get_view() : mainCam.get_chase_camera().get_view();
	mat4 P = cams["freeCam"] ? mainCam.get_free_camera().get_projection() : mainCam.get_chase_camera().get_projection();
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
		"shaders/final_shader.vert"
	};
	// list of all the fragment shaders to be called
	vector<string> fragment_shaders
	{
		"shaders/final_shader.frag",
		"shaders/part_direction.frag",
		"shaders/part_point.frag",
		"shaders/part_spot.frag",
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

	// Load in shaders
	main_eff.add_shader("shaders/shadow.vert", GL_VERTEX_SHADER);
	vector<string> shadow_frag_shaders{ "shaders/shadow.frag", "shaders/part_shadow.frag" };
	main_eff.add_shader(shadow_frag_shaders, GL_FRAGMENT_SHADER);

	// Build shader programs
	eff.build();
	shadow_eff.build();
	sky_eff.build();
	main_eff.build();
}

void World::load_textures()
{
	// Load textures
	tex = texture("textures/brickwall.jpg");
	texs[0] = texture("textures/snow.jpg", true, true, true);
	texs[1] = texture("textures/wall.tga", true, true, true);
	texs[2] = texture("textures/Half_Moon.tga", true, true, true);
	texs[3] = texture("textures/brickwall.jpg", true, true, true);
	texs[4] = texture("textures/tractor.dds", true, true, true);
	texs[5] = texture("textures/EAGLE.jpg", true, true, true);
	texs[6] = texture("textures/Full_Moon.tga", true, true, true);
	normal_maps[0] = texture("textures/wall_norm.tga");
	normal_maps[1] = texture("textures/brickwall_normal.jpg");
	normal_maps[2] = texture("textures/Moon_Normal.tga");
	normal_maps[3] = texture("textures/tractor_norm.dds");
	normal_maps[4] = texture("textures/crystal_norm.jpg");
}

bool World::load_content()
{
	// *********************************
	// Create shadow map - use screen size
	shadow = shadow_map(renderer::get_screen_width(), renderer::get_screen_height());	

	// Set spot properties
	spot.set_position(vec3(80.0f, 80.0f, -80.0f));
	spot.set_light_colour(vec4(1.0f, 1.0f, 0.8f, 1.0f));
	spot.set_direction(normalize(vec3(0.0f, -1.0f, 0.0f)));
	spot.set_range(50.0f);
	spot.set_power(2.0f);
	
	// Create terrain
	create_terrain();

	// Create basic colours
	create_base_colours();

	// Set materials
	vec4 emissive = basic_colours["black"];
	vec4 specular = basic_colours["white"];
	float shininess = 5.0f;

	// Meshes
	// Moon mesh
	meshes["moon"] = mesh(geometry_builder::create_sphere(50U, 50U));
	meshes["moon"].get_transform().translate(vec3(400.0f, 400.0f, 200.0f));
	meshes["moon"].get_transform().rotate(vec3(-pi<float>(), pi<float>(), 0.0f));
	meshes["moon"].get_transform().scale = vec3(25.0, 25.0f, 25.0f);
	meshes["moon"].get_material().set_emissive(emissive);
	meshes["moon"].get_material().set_diffuse(basic_colours["white"]);
	meshes["moon"].get_material().set_specular(specular);
	meshes["moon"].get_material().set_shininess(10.0f);

	// House mesh
	meshes["house1"] = mesh(geometry("models/medieval-house.3DS"));
	meshes["house1"].get_transform().scale = vec3(1.5f, 1.5f, 1.5f);
	meshes["house1"].get_transform().translate(vec3(100.0f, 0.0f, 100.0f));
	meshes["house1"].get_transform().rotate(vec3(-half_pi<float>(), 0.0f, 0.0f));
	meshes["house1"].get_material().set_emissive(emissive);
	meshes["house1"].get_material().set_diffuse(basic_colours["white"]);
	meshes["house1"].get_material().set_specular(specular);
	meshes["house1"].get_material().set_shininess(shininess);

	// Tractor mesh
	meshes["tractor"] = mesh(geometry("models/tractor.obj"));
	meshes["tractor"].get_transform().scale = vec3(7.5f, 7.5f, 7.5f);
	meshes["tractor"].get_transform().translate(vec3(0.0f, 0.0f, -75.0f));
	meshes["tractor"].get_transform().rotate(vec3(0.0f, 0.0f, 0.0f));
	meshes["tractor"].get_material().set_emissive(emissive);
	meshes["tractor"].get_material().set_diffuse(basic_colours["white"]);
	meshes["tractor"].get_material().set_specular(specular);
	meshes["tractor"].get_material().set_shininess(shininess);

	origTractorPosX = meshes["tractor"].get_transform().position.x;
	origTractorPosZ = meshes["tractor"].get_transform().position.z;

	// Eagle mesh
	meshes["eagle1"] = mesh(geometry("models/EAGLE_1.obj"));
	meshes["eagle1"].get_transform().scale = vec3(50.0f, 50.0f, 50.0f);
	meshes["eagle1"].get_transform().translate(meshes["house1"].get_transform().position);
	meshes["eagle1"].get_transform().position.x += 35.0f;
	meshes["eagle1"].get_transform().position.y += 40.0f;
	meshes["eagle1"].get_transform().position.z -= 20.0f;
	meshes["eagle1"].get_transform().rotate(vec3(0.0f, pi<float>(), 0.0f));
	meshes["eagle1"].get_material().set_emissive(emissive);
	meshes["eagle1"].get_material().set_diffuse(basic_colours["white"]);
	meshes["eagle1"].get_material().set_specular(specular);
	meshes["eagle1"].get_material().set_shininess(shininess);

	// Flying Eagle mesh
	meshes["eagle2"] = mesh(geometry("models/EAGLE_2.3ds"));
	meshes["eagle2"].get_transform().scale = vec3(30.0f, 30.0f, 30.0f);
	meshes["eagle2"].get_transform().translate(vec3(60.0f, 60.0f, -60.0f));
	meshes["eagle2"].get_transform().rotate(vec3(-half_pi<float>(), 0.0f, 0.0f));
	meshes["eagle2"].get_material().set_emissive(emissive);
	meshes["eagle2"].get_material().set_diffuse(basic_colours["white"]);
	meshes["eagle2"].get_material().set_specular(specular);
	meshes["eagle2"].get_material().set_shininess(shininess);

	// Flying Eagle mesh
	meshes["eagle3"] = mesh(geometry("models/EAGLE_2.3ds"));
	meshes["eagle3"].get_transform().scale = vec3(50.0f, 50.0f, 50.0f);
	meshes["eagle3"].get_transform().translate(vec3(60.0f, 200.0f, -60.0f));
	meshes["eagle3"].get_transform().rotate(vec3(-half_pi<float>(), 0.0f, 0.0f));
	meshes["eagle3"].get_material().set_emissive(emissive);
	meshes["eagle3"].get_material().set_diffuse(basic_colours["white"]);
	meshes["eagle3"].get_material().set_specular(specular);
	meshes["eagle3"].get_material().set_shininess(shininess);

	// Academy Fort mesh
	meshes["FortWall1x1"] = mesh(geometry_builder::create_box());
	meshes["FortWall1x1"].get_transform().translate(vec3(114.0f, 25.0f, 200.0f));
	meshes["FortWall1x1"].get_transform().scale = vec3(76.0, 50.0f, 10.0f);
	meshes["FortWall1x1"].get_material().set_emissive(emissive);
	meshes["FortWall1x1"].get_material().set_diffuse(basic_colours["white"]);
	meshes["FortWall1x1"].get_material().set_specular(specular);
	meshes["FortWall1x1"].get_material().set_shininess(shininess);
	meshes["FortWall1x2"] = mesh(geometry_builder::create_box());
	meshes["FortWall1x2"].get_transform().translate(vec3(38.0f, 25.0f, 200.0f));
	meshes["FortWall1x2"].get_transform().scale = vec3(76.0, 50.0f, 10.0f);
	meshes["FortWall1x2"].get_material().set_emissive(emissive);
	meshes["FortWall1x2"].get_material().set_diffuse(basic_colours["white"]);
	meshes["FortWall1x2"].get_material().set_specular(specular);
	meshes["FortWall1x2"].get_material().set_shininess(shininess);
	meshes["FortWall1x3"] = mesh(geometry_builder::create_box());
	meshes["FortWall1x3"].get_transform().translate(vec3(-38.0f, 25.0f, 200.0f));
	meshes["FortWall1x3"].get_transform().scale = vec3(76.0, 50.0f, 10.0f);
	meshes["FortWall1x3"].get_material().set_emissive(emissive);
	meshes["FortWall1x3"].get_material().set_diffuse(basic_colours["white"]);
	meshes["FortWall1x3"].get_material().set_specular(specular);
	meshes["FortWall1x3"].get_material().set_shininess(shininess);
	meshes["FortWall1x4"] = mesh(geometry_builder::create_box());
	meshes["FortWall1x4"].get_transform().translate(vec3(-114.0f, 25.0f, 200.0f));
	meshes["FortWall1x4"].get_transform().scale = vec3(76.0, 50.0f, 10.0f);
	meshes["FortWall1x4"].get_material().set_emissive(emissive);
	meshes["FortWall1x4"].get_material().set_diffuse(basic_colours["white"]);
	meshes["FortWall1x4"].get_material().set_specular(specular);
	meshes["FortWall1x4"].get_material().set_shininess(shininess);

	meshes["FortWall2x1"] = mesh(geometry_builder::create_box());
	meshes["FortWall2x1"].get_transform().translate(vec3(114.0f, 25.0f, -200.0f));
	meshes["FortWall2x1"].get_transform().scale = vec3(76.0f, 50.0f, 10.0f);
	meshes["FortWall2x1"].get_material().set_emissive(emissive);
	meshes["FortWall2x1"].get_material().set_diffuse(basic_colours["white"]);
	meshes["FortWall2x1"].get_material().set_specular(specular);
	meshes["FortWall2x1"].get_material().set_shininess(shininess);
	meshes["FortWall2x2"] = mesh(geometry_builder::create_box());
	meshes["FortWall2x2"].get_transform().translate(vec3(38.0f, 25.0f, -200.0f));
	meshes["FortWall2x2"].get_transform().scale = vec3(76.0f, 50.0f, 10.0f);
	meshes["FortWall2x2"].get_material().set_emissive(emissive);
	meshes["FortWall2x2"].get_material().set_diffuse(basic_colours["white"]);
	meshes["FortWall2x2"].get_material().set_specular(specular);
	meshes["FortWall2x2"].get_material().set_shininess(shininess);
	meshes["FortWall2x3"] = mesh(geometry_builder::create_box());
	meshes["FortWall2x3"].get_transform().translate(vec3(-38.0f, 25.0f, -200.0f));
	meshes["FortWall2x3"].get_transform().scale = vec3(76.0f, 50.0f, 10.0f);
	meshes["FortWall2x3"].get_material().set_emissive(emissive);
	meshes["FortWall2x3"].get_material().set_diffuse(basic_colours["white"]);
	meshes["FortWall2x3"].get_material().set_specular(specular);
	meshes["FortWall2x3"].get_material().set_shininess(shininess);
	meshes["FortWall2x4"] = mesh(geometry_builder::create_box());
	meshes["FortWall2x4"].get_transform().translate(vec3(-114.0f, 25.0f, -200.0f));
	meshes["FortWall2x4"].get_transform().scale = vec3(76.0f, 50.0f, 10.0f);
	meshes["FortWall2x4"].get_material().set_emissive(emissive);
	meshes["FortWall2x4"].get_material().set_diffuse(basic_colours["white"]);
	meshes["FortWall2x4"].get_material().set_specular(specular);
	meshes["FortWall2x4"].get_material().set_shininess(shininess);

	meshes["FortWall3x1"] = mesh(geometry_builder::create_box());
	meshes["FortWall3x1"].get_transform().translate(vec3(-200.0f, 25.0f, 114.0f));
	meshes["FortWall3x1"].get_transform().scale = vec3(10.0f, 50.0f, 76.0f);
	meshes["FortWall3x1"].get_material().set_emissive(emissive);
	meshes["FortWall3x1"].get_material().set_diffuse(basic_colours["white"]);
	meshes["FortWall3x1"].get_material().set_specular(specular);
	meshes["FortWall3x1"].get_material().set_shininess(shininess);
	meshes["FortWall3x2"] = mesh(geometry_builder::create_box());
	meshes["FortWall3x2"].get_transform().translate(vec3(-200.0f, 25.0f, 38.0f));
	meshes["FortWall3x2"].get_transform().scale = vec3(10.0f, 50.0f, 76.0f);
	meshes["FortWall3x2"].get_material().set_emissive(emissive);
	meshes["FortWall3x2"].get_material().set_diffuse(basic_colours["white"]);
	meshes["FortWall3x2"].get_material().set_specular(specular);
	meshes["FortWall3x2"].get_material().set_shininess(shininess);
	meshes["FortWall3x3"] = mesh(geometry_builder::create_box());
	meshes["FortWall3x3"].get_transform().translate(vec3(-200.0f, 25.0f, -38.0f));
	meshes["FortWall3x3"].get_transform().scale = vec3(10.0f, 50.0f, 76.0f);
	meshes["FortWall3x3"].get_material().set_emissive(emissive);
	meshes["FortWall3x3"].get_material().set_diffuse(basic_colours["white"]);
	meshes["FortWall3x3"].get_material().set_specular(specular);
	meshes["FortWall3x3"].get_material().set_shininess(shininess);
	meshes["FortWall3x4"] = mesh(geometry_builder::create_box());
	meshes["FortWall3x4"].get_transform().translate(vec3(-200.0f, 25.0f, -114.0f));
	meshes["FortWall3x4"].get_transform().scale = vec3(10.0f, 50.0f, 76.0f);
	meshes["FortWall3x4"].get_material().set_emissive(emissive);
	meshes["FortWall3x4"].get_material().set_diffuse(basic_colours["white"]);
	meshes["FortWall3x4"].get_material().set_specular(specular);
	meshes["FortWall3x4"].get_material().set_shininess(shininess);

	meshes["FortWallSmall1"] = mesh(geometry_builder::create_box());
	meshes["FortWallSmall1"].get_transform().translate(vec3(200.0f, 25.0f, 114.0f));
	meshes["FortWallSmall1"].get_transform().scale = vec3(10.0f, 50.0f, 76.0f);
	meshes["FortWallSmall1"].get_material().set_emissive(emissive);
	meshes["FortWallSmall1"].get_material().set_diffuse(basic_colours["white"]);
	meshes["FortWallSmall1"].get_material().set_specular(specular);
	meshes["FortWallSmall1"].get_material().set_shininess(shininess);
	meshes["FortWallSmall2"] = mesh(geometry_builder::create_box());
	meshes["FortWallSmall2"].get_transform().translate(vec3(200.0f, 25.0f, -114.0f));
	meshes["FortWallSmall2"].get_transform().scale = vec3(10.0f, 50.0f, 76.0f);
	meshes["FortWallSmall2"].get_material().set_emissive(emissive);
	meshes["FortWallSmall2"].get_material().set_diffuse(basic_colours["white"]);
	meshes["FortWallSmall2"].get_material().set_specular(specular);
	meshes["FortWallSmall2"].get_material().set_shininess(shininess);

	meshes["FortWallGate1"] = mesh(geometry_builder::create_box());
	meshes["FortWallGate1"].get_transform().translate(vec3(200.0f, 55.0f, -57.0f));
	meshes["FortWallGate1"].get_transform().scale = vec3(10.0f, 10.0f, 38.0f);
	meshes["FortWallGate1"].get_material().set_emissive(emissive);
	meshes["FortWallGate1"].get_material().set_diffuse(basic_colours["white"]);
	meshes["FortWallGate1"].get_material().set_specular(specular);
	meshes["FortWallGate1"].get_material().set_shininess(shininess);
	meshes["FortWallGate2"] = mesh(geometry_builder::create_box());
	meshes["FortWallGate2"].get_transform().translate(vec3(200.0f, 55.0f, -19.0f));
	meshes["FortWallGate2"].get_transform().scale = vec3(10.0f, 10.0f, 38.0f);
	meshes["FortWallGate2"].get_material().set_emissive(emissive);
	meshes["FortWallGate2"].get_material().set_diffuse(basic_colours["white"]);
	meshes["FortWallGate2"].get_material().set_specular(specular);
	meshes["FortWallGate2"].get_material().set_shininess(shininess);
	meshes["FortWallGate3"] = mesh(geometry_builder::create_box());
	meshes["FortWallGate3"].get_transform().translate(vec3(200.0f, 55.0f, 19.0f));
	meshes["FortWallGate3"].get_transform().scale = vec3(10.0f, 10.0f, 38.0f);
	meshes["FortWallGate3"].get_material().set_emissive(emissive);
	meshes["FortWallGate3"].get_material().set_diffuse(basic_colours["white"]);
	meshes["FortWallGate3"].get_material().set_specular(specular);
	meshes["FortWallGate3"].get_material().set_shininess(shininess);
	meshes["FortWallGate4"] = mesh(geometry_builder::create_box());
	meshes["FortWallGate4"].get_transform().translate(vec3(200.0f, 55.0f, 57.0f));
	meshes["FortWallGate4"].get_transform().scale = vec3(10.0f, 10.0f, 38.0f);
	meshes["FortWallGate4"].get_material().set_emissive(emissive);
	meshes["FortWallGate4"].get_material().set_diffuse(basic_colours["white"]);
	meshes["FortWallGate4"].get_material().set_specular(specular);
	meshes["FortWallGate4"].get_material().set_shininess(shininess);

	meshes["FortWallGateSmall1"] = mesh(geometry_builder::create_box());
	meshes["FortWallGateSmall1"].get_transform().translate(vec3(200.0f, 25.0f, -71.0f));
	meshes["FortWallGateSmall1"].get_transform().scale = vec3(10.0f, 50.0f, 10.0f);
	meshes["FortWallGateSmall1"].get_material().set_emissive(emissive);
	meshes["FortWallGateSmall1"].get_material().set_diffuse(basic_colours["white"]);
	meshes["FortWallGateSmall1"].get_material().set_specular(specular);
	meshes["FortWallGateSmall1"].get_material().set_shininess(shininess);

	meshes["FortWallGateSmall2"] = mesh(geometry_builder::create_box());
	meshes["FortWallGateSmall2"].get_transform().translate(vec3(200.0f, 25.0f, 71.0f));
	meshes["FortWallGateSmall2"].get_transform().scale = vec3(10.0f, 50.0f, 10.0f);
	meshes["FortWallGateSmall2"].get_material().set_emissive(emissive);
	meshes["FortWallGateSmall2"].get_material().set_diffuse(basic_colours["white"]);
	meshes["FortWallGateSmall2"].get_material().set_specular(specular);
	meshes["FortWallGateSmall2"].get_material().set_shininess(shininess);

	meshes["GateTop1"] = mesh(geometry_builder::create_sphere(50U, 50U));
	meshes["GateTop1"].get_transform().translate(vec3(200.0f, 69.0f, 70.0f));
	meshes["GateTop1"].get_transform().scale = vec3(10.0f, 10.0f, 10.0f);
	meshes["GateTop1"].get_material().set_emissive(emissive);
	meshes["GateTop1"].get_material().set_diffuse(basic_colours["yellow"]);
	meshes["GateTop1"].get_material().set_specular(specular);
	meshes["GateTop1"].get_material().set_shininess(shininess);

	meshes["GateTop2"] = mesh(geometry_builder::create_sphere(50U, 50U));
	meshes["GateTop2"].get_transform().translate(vec3(200.0f, 69.0f, -70.0f));
	meshes["GateTop2"].get_transform().scale = vec3(10.0f, 10.0f, 10.0f);
	meshes["GateTop2"].get_material().set_emissive(emissive);
	meshes["GateTop2"].get_material().set_diffuse(basic_colours["green"]);
	meshes["GateTop2"].get_material().set_specular(specular);
	meshes["GateTop2"].get_material().set_shininess(shininess);

	meshes["FortWallSide1"] = mesh(geometry_builder::create_box());
	meshes["FortWallSide1"].get_transform().translate(vec3(-175.0f, 25.0f, 175.0f));
	meshes["FortWallSide1"].get_transform().scale = vec3(75.0f, 50.0f, 10.0f);
	meshes["FortWallSide1"].get_transform().rotate(vec3(0.0f, -quarter_pi<float>(), 0.0f));
	meshes["FortWallSide1"].get_material().set_emissive(emissive);
	meshes["FortWallSide1"].get_material().set_diffuse(basic_colours["white"]);
	meshes["FortWallSide1"].get_material().set_specular(specular);
	meshes["FortWallSide1"].get_material().set_shininess(shininess);
	meshes["FortWallSide2"] = mesh(geometry_builder::create_box());
	meshes["FortWallSide2"].get_transform().translate(vec3(175.0f, 25.0f, 175.0f));
	meshes["FortWallSide2"].get_transform().scale = vec3(75.0f, 50.0f, 10.0f);
	meshes["FortWallSide2"].get_transform().rotate(vec3(0.0f, quarter_pi<float>(), 0.0f));
	meshes["FortWallSide2"].get_material().set_emissive(emissive);
	meshes["FortWallSide2"].get_material().set_diffuse(basic_colours["white"]);
	meshes["FortWallSide2"].get_material().set_specular(specular);
	meshes["FortWallSide2"].get_material().set_shininess(shininess);
	meshes["FortWallSide3"] = mesh(geometry_builder::create_box());
	meshes["FortWallSide3"].get_transform().translate(vec3(-175.0f, 25.0f, -175.0f));
	meshes["FortWallSide3"].get_transform().scale = vec3(75.0f, 50.0f, 10.0f);
	meshes["FortWallSide3"].get_transform().rotate(vec3(0.0f, quarter_pi<float>(), 0.0f));
	meshes["FortWallSide3"].get_material().set_emissive(emissive);
	meshes["FortWallSide3"].get_material().set_diffuse(basic_colours["white"]);
	meshes["FortWallSide3"].get_material().set_specular(specular);
	meshes["FortWallSide3"].get_material().set_shininess(shininess);
	meshes["FortWallSide4"] = mesh(geometry_builder::create_box());
	meshes["FortWallSide4"].get_transform().translate(vec3(175.0f, 25.0f, -175.0f));
	meshes["FortWallSide4"].get_transform().scale = vec3(75.0f, 50.0f, 10.0f);
	meshes["FortWallSide4"].get_transform().rotate(vec3(0.0f, -quarter_pi<float>(), 0.0f));
	meshes["FortWallSide4"].get_material().set_emissive(emissive);
	meshes["FortWallSide4"].get_material().set_diffuse(basic_colours["white"]);
	meshes["FortWallSide4"].get_material().set_specular(specular);
	meshes["FortWallSide4"].get_material().set_shininess(shininess);

	// Create skybox
	create_skybox();

	// Lights
	points.resize(1);
	spots.resize(2);

	// Directional light
	light.set_ambient_intensity(vec4(0.03f, 0.03f, 0.03f, 0.03f));
	light.set_light_colour(basic_colours["black"]);
	light.set_direction(normalize(-(meshes["moon"].get_transform().position)));

	// Point lights
	points[0].set_position(vec3(
		meshes["moon"].get_transform().position.x - 40.0f,
		meshes["moon"].get_transform().position.y + 20.0f,
		meshes["moon"].get_transform().position.z - 0.0f)
	);
	points[0].set_light_colour(vec4(basic_colours["white"]));
	points[0].set_range(50.0f);

	// Spot lights
	spots[0].set_position(vec3(meshes["house1"].get_transform().position) + vec3(30.0f, 5.0f, 5.0f));
	spots[0].set_light_colour(vec4(1.0f, 1.0f, 0.8f, 1.0f));
	spots[0].set_direction(normalize(vec3(5.0f, 5.0f, -5.0f)));
	spots[0].set_range(10.0f);
	spots[0].set_power(2.0f);

	spots[1].set_position(vec3(meshes["tractor"].get_transform().position) + vec3(0.0f, 20.0f, 30.0f));
	spots[1].set_light_colour(vec4(1.0f, 1.0f, 0.8f, 1.0f));
	spots[1].set_direction(vec3(-2.0f, 2.0f, 5.0f));
	spots[1].set_range(2.0f);
	spots[1].set_power(6.0f);

	// Load textures
	load_textures();

	// Load&Build shaders
	load_shaders();
	
	// Set camera properties
	cams["freeCam"] = true;
	cams["chaseCam"] = false;
	mainCam.set_free_camera();

	return true;
}

bool World::update(float delta_time)
{
	static mesh chaser = meshes["eagle3"];
	
	// Rotate a mesh
	meshes["GateTop1"].get_transform().rotate(vec3(0.0f, -half_pi<float>(), 0.0f) * delta_time * 0.5f);
	meshes["GateTop2"].get_transform().rotate(vec3(0.0f, -half_pi<float>(), 0.0f) * delta_time * 0.5f);

	elapsedTime += delta_time;

	meshes["eagle2"].get_transform().rotate(vec3(0.0f, 0.0f, quarter_pi<float>() * delta_time * 0.5f));
	meshes["eagle2"].get_transform().translate(vec3(sin(elapsedTime), 0.0, cos(elapsedTime)));

	meshes["tractor"].get_transform().translate(tractorMove);

	// Forward
	if (meshes["tractor"].get_transform().position.z >= origTractorPosZ + 200.0f)
	{
		meshes["tractor"].get_transform().rotate(vec3(0.0f, -half_pi<float>(), 0.0f));
		origTractorPosZ = meshes["tractor"].get_transform().position.z;
		tractorMove = vec3(-moveSpeed, 0.0f, 0.0f);
		spots[1].set_direction(vec3(-2.0f, 2.0f, 2.0f));
		spots[1].set_power(10.0f);
	}
	// Right
	if (meshes["tractor"].get_transform().position.x <= origTractorPosX - 100.0f)
	{
		meshes["tractor"].get_transform().rotate(vec3(0.0f, -half_pi<float>(), 0.0f));
		origTractorPosX = meshes["tractor"].get_transform().position.x;
		tractorMove = vec3(0.0f, 0.0f, -moveSpeed);
		spots[1].set_direction(vec3(-1.0f, -1.0f, 2.0f));
		spots[1].set_range(10.0f);
		spots[1].set_power(5.0f);
	}
	// Backward
	if (meshes["tractor"].get_transform().position.z <= origTractorPosZ - 200.0f)
	{
		meshes["tractor"].get_transform().rotate(vec3(0.0f, -half_pi<float>(), 0.0f));
		origTractorPosZ = meshes["tractor"].get_transform().position.z;
		tractorMove = vec3(moveSpeed, 0.0f, 0.0f);
		spots[1].set_direction(vec3(2.0f, -1.0f, 1.0f));
		spots[1].set_power(5.0f);
	}
	// Left
	if (meshes["tractor"].get_transform().position.x >= origTractorPosX + 100.0f)
	{
		meshes["tractor"].get_transform().rotate(vec3(0.0f, -half_pi<float>(), 0.0f));
		origTractorPosX = meshes["tractor"].get_transform().position.x;
		tractorMove = vec3(0.0f, 0.0f, moveSpeed);
		spots[1].set_direction(vec3(1.0f, 2.0f, 1.0f));
		spots[1].set_power(10.0f);
	}

	spots[1].move(tractorMove);
	spot.move((vec3(sin(elapsedTime), 0.0, cos(elapsedTime))));

	// Update the shadow map properties from the spot light
	shadow.light_position = spot.get_position();
	shadow.light_dir = spot.get_direction();

	// Update the camera
	if (glfwGetKey(renderer::get_window(), GLFW_KEY_1))
	{
		cams["chaseCam"] = false;
		cams["freeCam"] = true;
		mainCam.set_free_camera();
	}

	if (glfwGetKey(renderer::get_window(), GLFW_KEY_2))
	{
		cams["chaseCam"] = true;
		cams["freeCam"] = false;
		mainCam.set_chase_camera(chaser);
	}

	if (cams["freeCam"])
	{
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

		mainCam.rotate_free_camera(get_cursor_x(), get_cursor_y());
		mainCam.move_free_camera(transVec);
		mainCam.update_free_camera(delta_time);
	}
	else if (cams["chaseCam"])
	{
		mainCam.rotate_chase_camera(get_cursor_x(), get_cursor_y());
		// Use keyboard to rotate target_mesh - QE rotate on y-axis
		if (glfwGetKey(renderer::get_window(), GLFW_KEY_Q)) {
			meshes["eagle3"].get_transform().rotate(vec3(0.0f, 0.05f, 0.0f));
		}

		if (glfwGetKey(renderer::get_window(), GLFW_KEY_E)) {
			meshes["eagle3"].get_transform().rotate(vec3(0.0f, -0.05f, 0.0f));
		}

		// Also remember to translate camera
		vec3 transVec;
		if (glfwGetKey(renderer::get_window(), GLFW_KEY_W)) {
			transVec += vec3(0.0f, 0.0f, 0.5f);
		}
		if (glfwGetKey(renderer::get_window(), GLFW_KEY_S)) {
			transVec += vec3(0.0f, 0.0f, -0.5f);
		}
		if (glfwGetKey(renderer::get_window(), GLFW_KEY_A)) {
			transVec += vec3(0.5f, 0.0f, 0.0f);
		}
		if (glfwGetKey(renderer::get_window(), GLFW_KEY_D)) {
			transVec += vec3(-0.5f, 0.0f, 0.0f);
		}
		meshes["eagle3"].get_transform().translate(transVec);

		vec3 distVec = meshes["eagle3"].get_transform().position - mainCam.get_chase_camera().get_position();
		mainCam.get_chase_camera().move(meshes["eagle3"].get_transform().position, eulerAngles(meshes["eagle3"].get_transform().orientation));

		mainCam.update_chase_camera(delta_time);
	}

	// update cursor position
	set_cursor_pos();

	return true;
}

bool World::render()
{
	// Render skybox
	render_skybox();

	// *********************************
	// Set render target to shadow map
	renderer::set_render_target(shadow);
	// Clear depth buffer bit
	glClear(GL_DEPTH_BUFFER_BIT);
	// Set render mode to cull face
	glCullFace(GL_FRONT);
	// *********************************

	mat4 LightProjectionMat = perspective<float>(90.f, renderer::get_screen_aspect(), 0.1f, 1000.f);

	// Bind shader
	renderer::bind(shadow_eff);

	// Render meshes
	for (auto &e : meshes)
	{
		if (e.first == "eagle2" || e.first == "plane")
		{
			auto m = e.second;
			// Create MVP matrix
			auto M = m.get_transform().get_transform_matrix();
			// View matrix taken from shadow map
			auto V = shadow.get_view();
			// *********************************
			auto MVP = LightProjectionMat * V * M;
			// Set MVP matrix uniform
			glUniformMatrix4fv(shadow_eff.get_uniform_location("MVP"), // Location of uniform
				1,                                      // Number of values - 1 mat4
				GL_FALSE,                               // Transpose the matrix?
				value_ptr(MVP));                        // Pointer to matrix data
														// Render mesh
			renderer::render(m);
		}
	}

	// *********************************
	// Set render target back to the screen
	renderer::set_render_target();
	// Set cull face to back
	glCullFace(GL_BACK);
	// *********************************

	// Bind shader
	renderer::bind(main_eff);

	// Render meshes
	for (auto &e : meshes)
	{
		if (e.first == "eagle2" || e.first == "plane")
		{
			auto m = e.second;
			// Create MVP matrix
			auto M = m.get_transform().get_transform_matrix();
			auto V = cams["freeCam"] ? mainCam.get_free_camera().get_view() : mainCam.get_chase_camera().get_view();
			auto P = cams["freeCam"] ? mainCam.get_free_camera().get_projection() : mainCam.get_chase_camera().get_projection();
			auto MVP = P * V * M;

			auto lightMVP = LightProjectionMat * shadow.get_view() * M;

			// Set MVP matrix uniform
			glUniformMatrix4fv(main_eff.get_uniform_location("MVP"), 1, GL_FALSE, value_ptr(MVP));
			// Set Model matrix uniform
			glUniformMatrix4fv(main_eff.get_uniform_location("M"), 1, GL_FALSE, value_ptr(M));
			// Set Normal matrix uniform
			glUniformMatrix3fv(main_eff.get_uniform_location("N"), 1, GL_FALSE, value_ptr(m.get_transform().get_normal_matrix()));
			// *********************************
			// Set light transform
			glUniformMatrix4fv(main_eff.get_uniform_location("lightMVP"), 1, GL_FALSE, value_ptr(lightMVP));

			// Bind material
			renderer::bind(m.get_material(), "mat");

			// Bind spot lights
			renderer::bind(spot, "spot");

			// Bind textures to mesh
			if (e.first == "plane")
			{
				renderer::bind(texs[0], 0);
				glUniform1i(main_eff.get_uniform_location("tex"), 0);
			}
			else if (e.first == "eagle2")
			{
				renderer::bind(texs[5], 1);
				glUniform1i(main_eff.get_uniform_location("tex"), 1);
			}

			// Set eye position
			glUniform3fv(eff.get_uniform_location("eye_pos"), 1, value_ptr(cams["freeCam"] ? mainCam.get_free_camera().get_position()
				: mainCam.get_chase_camera().get_position()));

			// Bind shadow map texture - use texture unit 1
			renderer::bind(shadow.buffer->get_depth(), 1);
			glUniform1i(main_eff.get_uniform_location("shadow_map"), 1);

			// Render mesh
			renderer::render(m);
		}
	}
	
	// Render meshes with multiple lights, normals  and gamma correction
	for (auto &e : meshes)
	{
		mesh m = e.second;		
		// Bind effect
		renderer::bind(eff);

		// Create MVP matrix
		mat4 M = m.get_transform().get_transform_matrix();
		mat4 V = cams["freeCam"] ? mainCam.get_free_camera().get_view() : mainCam.get_chase_camera().get_view();
		mat4 P = cams["freeCam"] ? mainCam.get_free_camera().get_projection() : mainCam.get_chase_camera().get_projection();
		mat4 N = m.get_transform().get_normal_matrix();
		mat4 MVP = P * V * M;

		// Set MVP matrix uniform
		glUniformMatrix4fv(eff.get_uniform_location("MVP"), 1, GL_FALSE, value_ptr(MVP));

		// Set M matrix uniform
		glUniformMatrix4fv(eff.get_uniform_location("M"), 1, GL_FALSE, value_ptr(M));

		// Set N matrix uniform - remember - 3x3 matrix
		glUniformMatrix3fv(eff.get_uniform_location("N"), 1, GL_FALSE, value_ptr(N));

		// Bind material
		renderer::bind(m.get_material(), "mat");

		// Bind directional light
		renderer::bind(light, "light");

		// Bind point lights
		renderer::bind(points, "points");

		// Bind spot lights	
		renderer::bind(spots, "spots");

		// Set eye position - Get this from active camera
		glUniform3fv(eff.get_uniform_location("eye_pos"), 1, value_ptr(cams["freeCam"] ? mainCam.get_free_camera().get_position()
			: mainCam.get_chase_camera().get_position()));

		// Set gamma correction
		glUniform1i(eff.get_uniform_location("gamma"), gamma);

		// Bind textures to mesh
		if (e.first == "plane")
		{
			renderer::bind(texs[0], 1);
			glUniform1i(eff.get_uniform_location("tex"), 1);
		}
		else if (e.first == "moon")
		{
			renderer::bind(texs[2], 3);
			glUniform1i(eff.get_uniform_location("tex"), 3);

			// Bind normal map texture
			renderer::bind(normal_maps[2], 5);
			glUniform1i(eff.get_uniform_location("normal_map"), 5);
		}
		else if (e.first.find("FortWall") != string::npos)
		{
			renderer::bind(texs[3], 6);
			glUniform1i(eff.get_uniform_location("tex"), 6);

			// Bind normal map texture
			renderer::bind(normal_maps[1], 8);
			glUniform1i(eff.get_uniform_location("normal_map"), 8);
		}
		else if (e.first.find("house") != string::npos)
		{
			renderer::bind(texs[1], 9);
			glUniform1i(eff.get_uniform_location("tex"), 9);

			// Bind normal map texture
			renderer::bind(normal_maps[0], 11);
			glUniform1i(eff.get_uniform_location("normal_map"), 11);
		}
		else if (e.first == "tractor")
		{
			renderer::bind(texs[4], 12);
			glUniform1i(eff.get_uniform_location("tex"), 12);

			// Bind normal map texture
			renderer::bind(normal_maps[3], 14);
			glUniform1i(eff.get_uniform_location("normal_map"), 14);
		}
		else if (e.first.find("GateTop") != string::npos)
		{
			renderer::bind(texs[6], 15);
			glUniform1i(eff.get_uniform_location("tex"), 15);

			// Bind normal map texture
			renderer::bind(normal_maps[4], 17);
			glUniform1i(eff.get_uniform_location("normal_map"), 17);
		}
		else if (e.first.find("eagle") != string::npos)
		{
			renderer::bind(texs[5], 18);
			glUniform1i(eff.get_uniform_location("tex"), 18);
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