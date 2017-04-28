#include "WorldCreator.hpp"

namespace SpookyTown
{
	WorldCreator::WorldCreator() : WorldFramework(false, false)
	{
		create_base_colours();
	}

	WorldCreator::~WorldCreator()
	{ /* TODO */ }

	void WorldCreator::create_terrain()
	{
		meshes["tern"] = mesh(geometry(terrain("textures/terrain_heightmap.png").get_geometry()));
		meshes["tern"].get_transform().scale = vec3(7.5f, 1.0f, 7.5f);
		meshes["tern"].get_material().set_emissive(vec4(0.15f, 0.15f, 0.15f, 1.0f));
		meshes["tern"].get_material().set_diffuse(vec4(1.0f, 1.0f, 1.0f, 1.0f));
		meshes["tern"].get_material().set_specular(vec4(1.0f, 1.0f, 1.0f, 1.0f));
		meshes["tern"].get_material().set_shininess(10.0f);
	}

	void WorldCreator::create_world_meshes(float &movablePosX, float &movablePosZ)
	{
		// Set materials
		vec4 emissive = basic_colours["black"];
		vec4 specular = basic_colours["white"];
		shininess = 5.0f;

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
		meshes["house1"].get_transform().translate(vec3(100.0f, 12.0f, 100.0f));
		meshes["house1"].get_transform().rotate(vec3(-half_pi<float>(), 0.0f, 0.0f));
		meshes["house1"].get_material().set_emissive(emissive);
		meshes["house1"].get_material().set_diffuse(basic_colours["white"]);
		meshes["house1"].get_material().set_specular(specular);
		meshes["house1"].get_material().set_shininess(shininess);

		// Tractor mesh
		meshes["tractor"] = mesh(geometry("models/tractor.obj"));
		meshes["tractor"].get_transform().scale = vec3(7.5f, 7.5f, 7.5f);
		meshes["tractor"].get_transform().translate(vec3(0.0f, 10.0f, -75.0f));
		meshes["tractor"].get_transform().rotate(vec3(0.0f, 0.0f, 0.0f));
		meshes["tractor"].get_material().set_emissive(emissive);
		meshes["tractor"].get_material().set_diffuse(basic_colours["white"]);
		meshes["tractor"].get_material().set_specular(specular);
		meshes["tractor"].get_material().set_shininess(shininess);

		movablePosX = meshes["tractor"].get_transform().position.x;
		movablePosZ = meshes["tractor"].get_transform().position.z;

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
		meshes["eagle2"].get_transform().scale = vec3(40.0f, 40.0f, 40.0f);
		meshes["eagle2"].get_transform().translate(vec3(60.0f, 75.0f, -60.0f));
		meshes["eagle2"].get_transform().rotate(vec3(-half_pi<float>(), 0.0f, 0.0f));
		meshes["eagle2"].get_material().set_emissive(emissive);
		meshes["eagle2"].get_material().set_diffuse(basic_colours["white"]);
		meshes["eagle2"].get_material().set_specular(specular);
		meshes["eagle2"].get_material().set_shininess(shininess);

		// Flying Eagle mesh
		meshes["eagle3"] = mesh(geometry("models/EAGLE_2.3ds"));
		meshes["eagle3"].get_transform().scale = vec3(50.0f, 50.0f, 50.0f);
		meshes["eagle3"].get_transform().translate(vec3(60.0f, 215.0f, -60.0f));
		meshes["eagle3"].get_transform().rotate(vec3(-half_pi<float>(), 0.0f, 0.0f));
		meshes["eagle3"].get_material().set_emissive(emissive);
		meshes["eagle3"].get_material().set_diffuse(basic_colours["white"]);
		meshes["eagle3"].get_material().set_specular(specular);
		meshes["eagle3"].get_material().set_shininess(shininess);

		// Academy Fort mesh
		meshes["FortWall1x1"] = mesh(geometry_builder::create_box());
		meshes["FortWall1x1"].get_transform().translate(vec3(114.0f, 35.0f, 200.0f));
		meshes["FortWall1x1"].get_transform().scale = vec3(76.0, 50.0f, 10.0f);
		meshes["FortWall1x1"].get_material().set_emissive(emissive);
		meshes["FortWall1x1"].get_material().set_diffuse(basic_colours["white"]);
		meshes["FortWall1x1"].get_material().set_specular(specular);
		meshes["FortWall1x1"].get_material().set_shininess(shininess);
		meshes["FortWall1x2"] = mesh(geometry_builder::create_box());
		meshes["FortWall1x2"].get_transform().translate(vec3(38.0f, 35.0f, 200.0f));
		meshes["FortWall1x2"].get_transform().scale = vec3(76.0, 50.0f, 10.0f);
		meshes["FortWall1x2"].get_material().set_emissive(emissive);
		meshes["FortWall1x2"].get_material().set_diffuse(basic_colours["white"]);
		meshes["FortWall1x2"].get_material().set_specular(specular);
		meshes["FortWall1x2"].get_material().set_shininess(shininess);
		meshes["FortWall1x3"] = mesh(geometry_builder::create_box());
		meshes["FortWall1x3"].get_transform().translate(vec3(-38.0f, 35.0f, 200.0f));
		meshes["FortWall1x3"].get_transform().scale = vec3(76.0, 50.0f, 10.0f);
		meshes["FortWall1x3"].get_material().set_emissive(emissive);
		meshes["FortWall1x3"].get_material().set_diffuse(basic_colours["white"]);
		meshes["FortWall1x3"].get_material().set_specular(specular);
		meshes["FortWall1x3"].get_material().set_shininess(shininess);
		meshes["FortWall1x4"] = mesh(geometry_builder::create_box());
		meshes["FortWall1x4"].get_transform().translate(vec3(-114.0f, 35.0f, 200.0f));
		meshes["FortWall1x4"].get_transform().scale = vec3(76.0, 50.0f, 10.0f);
		meshes["FortWall1x4"].get_material().set_emissive(emissive);
		meshes["FortWall1x4"].get_material().set_diffuse(basic_colours["white"]);
		meshes["FortWall1x4"].get_material().set_specular(specular);
		meshes["FortWall1x4"].get_material().set_shininess(shininess);

		meshes["FortWall2x1"] = mesh(geometry_builder::create_box());
		meshes["FortWall2x1"].get_transform().translate(vec3(114.0f, 35.0f, -200.0f));
		meshes["FortWall2x1"].get_transform().scale = vec3(76.0f, 50.0f, 10.0f);
		meshes["FortWall2x1"].get_material().set_emissive(emissive);
		meshes["FortWall2x1"].get_material().set_diffuse(basic_colours["white"]);
		meshes["FortWall2x1"].get_material().set_specular(specular);
		meshes["FortWall2x1"].get_material().set_shininess(shininess);
		meshes["FortWall2x2"] = mesh(geometry_builder::create_box());
		meshes["FortWall2x2"].get_transform().translate(vec3(38.0f, 35.0f, -200.0f));
		meshes["FortWall2x2"].get_transform().scale = vec3(76.0f, 50.0f, 10.0f);
		meshes["FortWall2x2"].get_material().set_emissive(emissive);
		meshes["FortWall2x2"].get_material().set_diffuse(basic_colours["white"]);
		meshes["FortWall2x2"].get_material().set_specular(specular);
		meshes["FortWall2x2"].get_material().set_shininess(shininess);
		meshes["FortWall2x3"] = mesh(geometry_builder::create_box());
		meshes["FortWall2x3"].get_transform().translate(vec3(-38.0f, 35.0f, -200.0f));
		meshes["FortWall2x3"].get_transform().scale = vec3(76.0f, 50.0f, 10.0f);
		meshes["FortWall2x3"].get_material().set_emissive(emissive);
		meshes["FortWall2x3"].get_material().set_diffuse(basic_colours["white"]);
		meshes["FortWall2x3"].get_material().set_specular(specular);
		meshes["FortWall2x3"].get_material().set_shininess(shininess);
		meshes["FortWall2x4"] = mesh(geometry_builder::create_box());
		meshes["FortWall2x4"].get_transform().translate(vec3(-114.0f, 35.0f, -200.0f));
		meshes["FortWall2x4"].get_transform().scale = vec3(76.0f, 50.0f, 10.0f);
		meshes["FortWall2x4"].get_material().set_emissive(emissive);
		meshes["FortWall2x4"].get_material().set_diffuse(basic_colours["white"]);
		meshes["FortWall2x4"].get_material().set_specular(specular);
		meshes["FortWall2x4"].get_material().set_shininess(shininess);

		meshes["FortWall3x1"] = mesh(geometry_builder::create_box());
		meshes["FortWall3x1"].get_transform().translate(vec3(-200.0f, 35.0f, 114.0f));
		meshes["FortWall3x1"].get_transform().scale = vec3(10.0f, 50.0f, 76.0f);
		meshes["FortWall3x1"].get_material().set_emissive(emissive);
		meshes["FortWall3x1"].get_material().set_diffuse(basic_colours["white"]);
		meshes["FortWall3x1"].get_material().set_specular(specular);
		meshes["FortWall3x1"].get_material().set_shininess(shininess);
		meshes["FortWall3x2"] = mesh(geometry_builder::create_box());
		meshes["FortWall3x2"].get_transform().translate(vec3(-200.0f, 35.0f, 38.0f));
		meshes["FortWall3x2"].get_transform().scale = vec3(10.0f, 50.0f, 76.0f);
		meshes["FortWall3x2"].get_material().set_emissive(emissive);
		meshes["FortWall3x2"].get_material().set_diffuse(basic_colours["white"]);
		meshes["FortWall3x2"].get_material().set_specular(specular);
		meshes["FortWall3x2"].get_material().set_shininess(shininess);
		meshes["FortWall3x3"] = mesh(geometry_builder::create_box());
		meshes["FortWall3x3"].get_transform().translate(vec3(-200.0f, 35.0f, -38.0f));
		meshes["FortWall3x3"].get_transform().scale = vec3(10.0f, 50.0f, 76.0f);
		meshes["FortWall3x3"].get_material().set_emissive(emissive);
		meshes["FortWall3x3"].get_material().set_diffuse(basic_colours["white"]);
		meshes["FortWall3x3"].get_material().set_specular(specular);
		meshes["FortWall3x3"].get_material().set_shininess(shininess);
		meshes["FortWall3x4"] = mesh(geometry_builder::create_box());
		meshes["FortWall3x4"].get_transform().translate(vec3(-200.0f, 35.0f, -114.0f));
		meshes["FortWall3x4"].get_transform().scale = vec3(10.0f, 50.0f, 76.0f);
		meshes["FortWall3x4"].get_material().set_emissive(emissive);
		meshes["FortWall3x4"].get_material().set_diffuse(basic_colours["white"]);
		meshes["FortWall3x4"].get_material().set_specular(specular);
		meshes["FortWall3x4"].get_material().set_shininess(shininess);

		meshes["FortWallSmall1"] = mesh(geometry_builder::create_box());
		meshes["FortWallSmall1"].get_transform().translate(vec3(200.0f, 35.0f, 114.0f));
		meshes["FortWallSmall1"].get_transform().scale = vec3(10.0f, 50.0f, 76.0f);
		meshes["FortWallSmall1"].get_material().set_emissive(emissive);
		meshes["FortWallSmall1"].get_material().set_diffuse(basic_colours["white"]);
		meshes["FortWallSmall1"].get_material().set_specular(specular);
		meshes["FortWallSmall1"].get_material().set_shininess(shininess);
		meshes["FortWallSmall2"] = mesh(geometry_builder::create_box());
		meshes["FortWallSmall2"].get_transform().translate(vec3(200.0f, 35.0f, -114.0f));
		meshes["FortWallSmall2"].get_transform().scale = vec3(10.0f, 50.0f, 76.0f);
		meshes["FortWallSmall2"].get_material().set_emissive(emissive);
		meshes["FortWallSmall2"].get_material().set_diffuse(basic_colours["white"]);
		meshes["FortWallSmall2"].get_material().set_specular(specular);
		meshes["FortWallSmall2"].get_material().set_shininess(shininess);

		meshes["FortWallGate1"] = mesh(geometry_builder::create_box());
		meshes["FortWallGate1"].get_transform().translate(vec3(200.0f, 65.0f, -57.0f));
		meshes["FortWallGate1"].get_transform().scale = vec3(10.0f, 10.0f, 38.0f);
		meshes["FortWallGate1"].get_material().set_emissive(emissive);
		meshes["FortWallGate1"].get_material().set_diffuse(basic_colours["white"]);
		meshes["FortWallGate1"].get_material().set_specular(specular);
		meshes["FortWallGate1"].get_material().set_shininess(shininess);
		meshes["FortWallGate2"] = mesh(geometry_builder::create_box());
		meshes["FortWallGate2"].get_transform().translate(vec3(200.0f, 65.0f, -19.0f));
		meshes["FortWallGate2"].get_transform().scale = vec3(10.0f, 10.0f, 38.0f);
		meshes["FortWallGate2"].get_material().set_emissive(emissive);
		meshes["FortWallGate2"].get_material().set_diffuse(basic_colours["white"]);
		meshes["FortWallGate2"].get_material().set_specular(specular);
		meshes["FortWallGate2"].get_material().set_shininess(shininess);
		meshes["FortWallGate3"] = mesh(geometry_builder::create_box());
		meshes["FortWallGate3"].get_transform().translate(vec3(200.0f, 65.0f, 19.0f));
		meshes["FortWallGate3"].get_transform().scale = vec3(10.0f, 10.0f, 38.0f);
		meshes["FortWallGate3"].get_material().set_emissive(emissive);
		meshes["FortWallGate3"].get_material().set_diffuse(basic_colours["white"]);
		meshes["FortWallGate3"].get_material().set_specular(specular);
		meshes["FortWallGate3"].get_material().set_shininess(shininess);
		meshes["FortWallGate4"] = mesh(geometry_builder::create_box());
		meshes["FortWallGate4"].get_transform().translate(vec3(200.0f, 65.0f, 57.0f));
		meshes["FortWallGate4"].get_transform().scale = vec3(10.0f, 10.0f, 38.0f);
		meshes["FortWallGate4"].get_material().set_emissive(emissive);
		meshes["FortWallGate4"].get_material().set_diffuse(basic_colours["white"]);
		meshes["FortWallGate4"].get_material().set_specular(specular);
		meshes["FortWallGate4"].get_material().set_shininess(shininess);

		meshes["FortWallGateSmall1"] = mesh(geometry_builder::create_box());
		meshes["FortWallGateSmall1"].get_transform().translate(vec3(200.0f, 35.0f, -71.0f));
		meshes["FortWallGateSmall1"].get_transform().scale = vec3(10.0f, 50.0f, 10.0f);
		meshes["FortWallGateSmall1"].get_material().set_emissive(emissive);
		meshes["FortWallGateSmall1"].get_material().set_diffuse(basic_colours["white"]);
		meshes["FortWallGateSmall1"].get_material().set_specular(specular);
		meshes["FortWallGateSmall1"].get_material().set_shininess(shininess);

		meshes["FortWallGateSmall2"] = mesh(geometry_builder::create_box());
		meshes["FortWallGateSmall2"].get_transform().translate(vec3(200.0f, 35.0f, 71.0f));
		meshes["FortWallGateSmall2"].get_transform().scale = vec3(10.0f, 50.0f, 10.0f);
		meshes["FortWallGateSmall2"].get_material().set_emissive(emissive);
		meshes["FortWallGateSmall2"].get_material().set_diffuse(basic_colours["white"]);
		meshes["FortWallGateSmall2"].get_material().set_specular(specular);
		meshes["FortWallGateSmall2"].get_material().set_shininess(shininess);

		meshes["GateTop1"] = mesh(geometry_builder::create_sphere(200U, 200U));
		meshes["GateTop1"].get_transform().translate(vec3(200.0f, 80.0f, 70.0f));
		meshes["GateTop1"].get_transform().scale = vec3(10.0f, 10.0f, 10.0f);
		meshes["GateTop1"].get_material().set_emissive(emissive);
		meshes["GateTop1"].get_material().set_diffuse(basic_colours["white"]);
		meshes["GateTop1"].get_material().set_specular(specular);
		meshes["GateTop1"].get_material().set_shininess(shininess);

		meshes["GateTop2"] = mesh(geometry_builder::create_sphere(200U, 200U));
		meshes["GateTop2"].get_transform().translate(vec3(200.0f, 80.0f, -70.0f));
		meshes["GateTop2"].get_transform().scale = vec3(10.0f, 10.0f, 10.0f);
		meshes["GateTop2"].get_material().set_emissive(emissive);
		meshes["GateTop2"].get_material().set_diffuse(basic_colours["white"]);
		meshes["GateTop2"].get_material().set_specular(specular);
		meshes["GateTop2"].get_material().set_shininess(shininess);

		meshes["FortWallSide1"] = mesh(geometry_builder::create_box());
		meshes["FortWallSide1"].get_transform().translate(vec3(-175.0f, 35.0f, 175.0f));
		meshes["FortWallSide1"].get_transform().scale = vec3(75.0f, 50.0f, 10.0f);
		meshes["FortWallSide1"].get_transform().rotate(vec3(0.0f, -quarter_pi<float>(), 0.0f));
		meshes["FortWallSide1"].get_material().set_emissive(emissive);
		meshes["FortWallSide1"].get_material().set_diffuse(basic_colours["white"]);
		meshes["FortWallSide1"].get_material().set_specular(specular);
		meshes["FortWallSide1"].get_material().set_shininess(shininess);
		meshes["FortWallSide2"] = mesh(geometry_builder::create_box());
		meshes["FortWallSide2"].get_transform().translate(vec3(175.0f, 35.0f, 175.0f));
		meshes["FortWallSide2"].get_transform().scale = vec3(75.0f, 50.0f, 10.0f);
		meshes["FortWallSide2"].get_transform().rotate(vec3(0.0f, quarter_pi<float>(), 0.0f));
		meshes["FortWallSide2"].get_material().set_emissive(emissive);
		meshes["FortWallSide2"].get_material().set_diffuse(basic_colours["white"]);
		meshes["FortWallSide2"].get_material().set_specular(specular);
		meshes["FortWallSide2"].get_material().set_shininess(shininess);
		meshes["FortWallSide3"] = mesh(geometry_builder::create_box());
		meshes["FortWallSide3"].get_transform().translate(vec3(-175.0f, 35.0f, -175.0f));
		meshes["FortWallSide3"].get_transform().scale = vec3(75.0f, 50.0f, 10.0f);
		meshes["FortWallSide3"].get_transform().rotate(vec3(0.0f, quarter_pi<float>(), 0.0f));
		meshes["FortWallSide3"].get_material().set_emissive(emissive);
		meshes["FortWallSide3"].get_material().set_diffuse(basic_colours["white"]);
		meshes["FortWallSide3"].get_material().set_specular(specular);
		meshes["FortWallSide3"].get_material().set_shininess(shininess);
		meshes["FortWallSide4"] = mesh(geometry_builder::create_box());
		meshes["FortWallSide4"].get_transform().translate(vec3(175.0f, 35.0f, -175.0f));
		meshes["FortWallSide4"].get_transform().scale = vec3(75.0f, 50.0f, 10.0f);
		meshes["FortWallSide4"].get_transform().rotate(vec3(0.0f, -quarter_pi<float>(), 0.0f));
		meshes["FortWallSide4"].get_material().set_emissive(emissive);
		meshes["FortWallSide4"].get_material().set_diffuse(basic_colours["white"]);
		meshes["FortWallSide4"].get_material().set_specular(specular);
		meshes["FortWallSide4"].get_material().set_shininess(shininess);
	}

	void WorldCreator::create_skybox()
	{
		// Create box geometry for skybox
		skybox = mesh(geometry_builder::create_box());
		// Scale box by 100
		skybox.get_transform().scale = vec3(1000.0f);
		skybox.get_transform().translate(vec3(0.0f));
		// Load the cubemap
		array<string, 6> filenames = 
		{
			"textures/cool_sky/city_ft.tga", "textures/cool_sky/city_bk.tga",
			"textures/cool_sky/city_up.tga", "textures/cool_sky/city_dn.tga",
			"textures/cool_sky/city_rt.tga", "textures/cool_sky/city_lf.tga"
		};
		// Create cube_map
		cube_map = cubemap(filenames);
	}
}