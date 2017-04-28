#include "WorldFramework.hpp"

namespace SpookyTown
{
	WorldFramework::WorldFramework(bool loadedTextures, bool loadedShaders)
	{
		m_loadedTextures = loadedTextures;
		m_loadedShaders = loadedShaders;
	}

	void WorldFramework::load_shaders()
	{
		// Load in main shaders
		main_eff.add_shader("shaders/shadow.vert", GL_VERTEX_SHADER);
		vector<string> frag_shaders
		{
			"shaders/shadow.frag",
			"shaders/part_direction.frag",
			"shaders/part_point.frag",
			"shaders/part_spot.frag",
			"shaders/part_normal_map.frag",
			"shaders/part_shadow.frag"
		};
		main_eff.add_shader(frag_shaders, GL_FRAGMENT_SHADER);
		// Load in shadow effect
		shadow_eff.add_shader("shaders/spot.vert", GL_VERTEX_SHADER);
		shadow_eff.add_shader("shaders/spot.frag", GL_FRAGMENT_SHADER);
		// Load in skybox effect
		sky_eff.add_shader("shaders/skybox.vert", GL_VERTEX_SHADER);
		sky_eff.add_shader("shaders/skybox.frag", GL_FRAGMENT_SHADER);
		// Load in terrain effect
		terrain_eff.add_shader("shaders/terrain.vert", GL_VERTEX_SHADER);
		terrain_eff.add_shader(vector<string> { "shaders/terrain.frag", "shaders/part_weighted_texture.frag" }, GL_FRAGMENT_SHADER);
		// Load in billboard shader
		billboard_eff.add_shader("shaders/billboard.vert", GL_VERTEX_SHADER);
		billboard_eff.add_shader("shaders/billboard.geom", GL_GEOMETRY_SHADER);
		billboard_eff.add_shader("shaders/billboard.frag", GL_FRAGMENT_SHADER);
		// Load in post processing shader
		post_eff.add_shader("shaders/post.vert", GL_VERTEX_SHADER);
		post_eff.add_shader("shaders/post.frag", GL_FRAGMENT_SHADER);
		// Load in PBR shader
		pbr_eff.add_shader("shaders/PBR.vert", GL_VERTEX_SHADER);
		pbr_eff.add_shader("shaders/PBR.frag", GL_FRAGMENT_SHADER);
		
		// Build shader programs
		shadow_eff.build();
		sky_eff.build();
		terrain_eff.build();
		main_eff.build();
		billboard_eff.build();
		post_eff.build();
		pbr_eff.build();
	}

	void WorldFramework::load_textures()
	{
		// Load textures
		texs[0] = texture("textures/Half_Moon.tga", true, true, true);
		texs[1] = texture("textures/Full_Moon.tga", true, true, true);
		texs[2] = texture("textures/tractor.dds", true, true, true);
		texs[3] = texture("textures/EAGLE.jpg", true, true, false);
		texs[4] = texture("textures/bushy.png", true, true, false);
		normal_maps[0] = texture("textures/Moon_Normal.tga", true, true, false);
		normal_maps[1] = texture("textures/tractor_norm.dds", true, true, false);
		
		pbr_texs[0] = texture("textures/pbr/grass/albedo.png", true, true, true);
		pbr_texs[1] = texture("textures/pbr/grass/normal.png", true, true, false);
		pbr_texs[2] = texture("textures/pbr/grass/metallic.png", true, true, false);
		pbr_texs[3] = texture("textures/pbr/grass/roughness.png", true, true, false);
		pbr_texs[4] = texture("textures/pbr/grass/ao.png", true, true, false);
		pbr_texs[5] = texture("textures/pbr/harshbrick/albedo.png", true, true, true);
		pbr_texs[6] = texture("textures/pbr/harshbrick/normal.png", true, true, false);
		pbr_texs[7] = texture("textures/pbr/harshbrick/metallic.png", true, true, false);
		pbr_texs[8] = texture("textures/pbr/harshbrick/roughness.png", true, true, false);
		pbr_texs[9] = texture("textures/pbr/harshbrick/ao.png", true, true, false);
		pbr_texs[10] = texture("textures/pbr/mossy_ground/albedo.png", true, true, true);
		pbr_texs[11] = texture("textures/pbr/mossy_ground/normal.png", true, true, false);
		pbr_texs[12] = texture("textures/pbr/mossy_ground/metallic.png", true, true, false);
		pbr_texs[13] = texture("textures/pbr/mossy_ground/roughness.png", true, true, false);
		pbr_texs[14] = texture("textures/pbr/mossy_ground/ao.png", true, true, false);
		pbr_texs[15] = texture("textures/pbr/pineneedles/albedo.png", true, true, true);
		pbr_texs[16] = texture("textures/pbr/pineneedles/normal.png", true, true, false);
		pbr_texs[17] = texture("textures/pbr/pineneedles/metallic.png", true, true, false);
		pbr_texs[18] = texture("textures/pbr/pineneedles/roughness.png", true, true, false);
		pbr_texs[19] = texture("textures/pbr/pineneedles/ao.png", true, true, false);
		pbr_texs[20] = texture("textures/pbr/rusted_iron/albedo.png", true, true, true);
		pbr_texs[21] = texture("textures/pbr/rusted_iron/normal.png", true, true, false);
		pbr_texs[22] = texture("textures/pbr/rusted_iron/metallic.png", true, true, false);
		pbr_texs[23] = texture("textures/pbr/rusted_iron/roughness.png", true, true, false);
		pbr_texs[24] = texture("textures/pbr/rusted_iron/ao.png", true, true, false);
		pbr_texs[25] = texture("textures/pbr/sandy_dry_soil/albedo.png", true, true, true);
		pbr_texs[26] = texture("textures/pbr/sandy_dry_soil/normal.png", true, true, false);
		pbr_texs[27] = texture("textures/pbr/sandy_dry_soil/metallic.png", true, true, false);
		pbr_texs[28] = texture("textures/pbr/sandy_dry_soil/roughness.png", true, true, false);
		pbr_texs[29] = texture("textures/pbr/sandy_dry_soil/ao.png", true, true, false);
		pbr_texs[30] = texture("textures/pbr/wood_frame/albedo.png", true, true, true);
		pbr_texs[31] = texture("textures/pbr/wood_frame/normal.png", true, true, false);
		pbr_texs[32] = texture("textures/pbr/wood_frame/metallic.png", true, true, false);
		pbr_texs[33] = texture("textures/pbr/wood_frame/roughness.png", true, true, false);
		pbr_texs[34] = texture("textures/pbr/wood_frame/ao.png", true, true, false);
		pbr_texs[35] = texture("textures/pbr/harshbrick/depth.png", true, true, false);

		m_loadedTextures = true;
	}

	void WorldFramework::create_base_colours()
	{
		basic_colours["black"] = vec4(0.0f, 0.0f, 0.0f, 1.0f);
		basic_colours["white"] = vec4(1.0f, 1.0f, 1.0f, 1.0f);
		basic_colours["red"] = vec4(1.0f, 0.0f, 0.0f, 1.0f);
		basic_colours["green"] = vec4(0.0f, 1.0f, 0.0f, 1.0f);
		basic_colours["blue"] = vec4(0.0f, 0.0f, 1.0f, 1.0f);
		basic_colours["yellow"] = vec4(1.0f, 1.0f, 0.0f, 1.0f);
	}
}