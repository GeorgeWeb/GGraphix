#include "WorldRenderer.hpp"

namespace SpookyTown
{
	WorldRenderer::WorldRenderer() : WorldFramework(true, true)
	{
		// create a general light projection matrix
		lightProjectionMat = perspective<float>(90.0f, renderer::get_screen_aspect(), 0.1f, 1000.f);

		// load the textures on create;
		load_textures();
		// load in the shaders & create program
		load_shaders();
	}

	void WorldRenderer::render_skybox(map<string, mesh> meshes, mesh skybox, cubemap cube_map, map<string, bool> cams, CameraFramework *mainCam)
	{
		if (!m_loadedShaders)
			load_shaders();

		if(!m_loadedTextures)
			load_textures();

		// Render skybox
		glDisable(GL_DEPTH_TEST);
		glDepthMask(GL_FALSE);
		glDisable(GL_CULL_FACE);

		// Bind skybox effect
		renderer::bind(sky_eff);

		// Calculate MVP for the skybox
		mat4 M = skybox.get_transform().get_transform_matrix();
		mat4 V = cams["freeCam"] ? mainCam->get_free_camera().get_view() : mainCam->get_chase_camera().get_view();
		mat4 P = cams["freeCam"] ? mainCam->get_free_camera().get_projection() : mainCam->get_chase_camera().get_projection();
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

	void WorldRenderer::render_shadows(map<string, mesh> meshes, shadow_map shadow)
	{
		if (!m_loadedShaders)
			load_shaders();

		if(!m_loadedTextures)
			load_textures();

		mat4 lightPV = lightProjectionMat * shadow.get_view();;
		
		mesh m;
		mat4 MVP;

		// Set render target to shadow map
		renderer::set_render_target(shadow);
		// Clear depth buffer bit
		glClear(GL_DEPTH_BUFFER_BIT);
		// Set render mode to cull face
		glCullFace(GL_FRONT);
		// *********************************
		// Bind shader
		renderer::bind(shadow_eff);
		// Render meshes
		for (auto &e : meshes)
		{
			m = e.second;
			MVP = lightPV * m.get_transform().get_transform_matrix();
			// Set MVP matrix uniform
			glUniformMatrix4fv(shadow_eff.get_uniform_location("MVP"), 1, GL_FALSE, value_ptr(MVP));
			// Render mesh
			renderer::render(m);
		}
		// Set render target back to the screen
		renderer::set_render_target();
		// Set cull face to back
		glCullFace(GL_BACK);
	}

	void WorldRenderer::render_terrain(map<string, mesh> meshes, map<string, bool> cams, CameraFramework *mainCam, directional_light light)
	{
		if (!m_loadedShaders)
			load_shaders();

		if (!m_loadedTextures)
			load_textures();

		// Bind effect
		renderer::bind(terrain_eff);
		// Create MVP matrix
		mat4 M = meshes["tern"].get_transform().get_transform_matrix();
		mat4 V = cams["freeCam"] ? mainCam->get_free_camera().get_view() : mainCam->get_chase_camera().get_view();
		mat4 P = cams["freeCam"] ? mainCam->get_free_camera().get_projection() : mainCam->get_chase_camera().get_projection();
		mat4 MVP = P * V * M;
		glUniformMatrix4fv(terrain_eff.get_uniform_location("MVP"), 1, GL_FALSE, value_ptr(MVP));
		// Set M matrix uniform
		glUniformMatrix4fv(terrain_eff.get_uniform_location("M"), 1, GL_FALSE, value_ptr(M));
		// Set N matrix uniform
		glUniformMatrix3fv(terrain_eff.get_uniform_location("N"), 1, GL_FALSE, value_ptr(meshes["tern"].get_transform().get_normal_matrix()));
		//Bind Terrian Material
		renderer::bind(meshes["tern"].get_material(), "mat");
		renderer::bind(light, "light");
		renderer::bind(pbr_texs[15], 0);
		glUniform1i(terrain_eff.get_uniform_location("tex[0]"), 0);
		renderer::bind(pbr_texs[0], 1);
		glUniform1i(terrain_eff.get_uniform_location("tex[1]"), 1);
		renderer::bind(pbr_texs[25], 2);
		glUniform1i(terrain_eff.get_uniform_location("tex[2]"), 2);
		renderer::bind(pbr_texs[10], 3);
		glUniform1i(terrain_eff.get_uniform_location("tex[3]"), 3);
		renderer::render(meshes["tern"]);
	}

	void WorldRenderer::render_actors(ActorFramework *actor, CameraFramework *mainCam, map<string, bool> cams)
	{
		#pragma region initialization checks
		if (!m_loadedShaders)
			load_shaders();

		if (!m_loadedTextures)
			load_textures();
		#pragma endregion
		actor->renderNPC(billboard_eff, mainCam, cams, texs[4]);
	}

	void WorldRenderer::render_world(map<string, mesh> meshes, map<string, bool> cams, CameraFramework *mainCam, 
		directional_light light, point_light point, vector<spot_light> spots, shadow_map shadow, GLint parallax)
	{
		#pragma region initialization checks
		if (!m_loadedShaders)
			load_shaders();

		if (!m_loadedTextures)
			load_textures();
		#pragma endregion

		mat4 V = cams["freeCam"] ? mainCam->get_free_camera().get_view() : mainCam->get_chase_camera().get_view();
		mat4 P = cams["freeCam"] ? mainCam->get_free_camera().get_projection() : mainCam->get_chase_camera().get_projection();
		mat4 PV = P * V;
		mat4 lightPV = lightProjectionMat * shadow.get_view();

		mat4 M;
		mat4 MVP;
		mat4 lightMVP;

		// Bind shader
		renderer::bind(main_eff);

		// Bind directional light
		renderer::bind(light, "light");

		// Bind point light(s)
		renderer::bind(point, "point");

		// Bind spot light(s)
		renderer::bind(spots, "spots");

		// Set eye position
		glUniform3fv(main_eff.get_uniform_location("eye_pos"), 1, value_ptr(cams["freeCam"] ? mainCam->get_free_camera().get_position()
			: mainCam->get_chase_camera().get_position()));

		// Set parralax mapping related uniforms
		glUniform1f(main_eff.get_uniform_location("height_scale"), height_scale);
		glUniform1i(main_eff.get_uniform_location("parallax"), parallax);

		// Bind depth map
		if (parallax == 1)
		{
			renderer::bind(pbr_texs[35], 17);
			glUniform1i(main_eff.get_uniform_location("depth_map"), 17);
		}

		// Render meshes
		for (auto &e : meshes)
		{
			#pragma region initialization checks
			if (!m_loadedShaders)
				load_shaders();

			if (!m_loadedTextures)
				load_textures();
			#pragma endregion

			mesh m = e.second;
			// Create MVP matrix
			M = m.get_transform().get_transform_matrix();
			MVP = PV * M;
			lightMVP = lightPV * M;

			// Set MVP matrix uniform
			glUniformMatrix4fv(main_eff.get_uniform_location("MVP"), 1, GL_FALSE, value_ptr(MVP));
			// Set Model matrix uniform
			glUniformMatrix4fv(main_eff.get_uniform_location("M"), 1, GL_FALSE, value_ptr(M));
			// Set Normal matrix uniform
			glUniformMatrix3fv(main_eff.get_uniform_location("N"), 1, GL_FALSE, value_ptr(m.get_transform().get_normal_matrix()));
			// Set light transform
			glUniformMatrix4fv(main_eff.get_uniform_location("lightMVP"), 1, GL_FALSE, value_ptr(lightMVP));

			// Bind shadow map texture - use texture unit 1
			renderer::bind(shadow.buffer->get_depth(), 1);
			glUniform1i(main_eff.get_uniform_location("shadow_map"), 1);

			// Bind material
			renderer::bind(m.get_material(), "mat");

			if (e.first == "tern")
			{
				continue;
			}
			else if (e.first == "moon")
			{
				parallax = 0;
				renderer::bind(texs[0], 3);
				glUniform1i(main_eff.get_uniform_location("tex"), 3);
				renderer::bind(normal_maps[0], 4);
				glUniform1i(main_eff.get_uniform_location("normal_map"), 4);
			}
			else if (e.first.find("FortWall") != string::npos)
			{
				renderer::bind(pbr_texs[5], 5);
				glUniform1i(main_eff.get_uniform_location("tex"), 5);
				renderer::bind(pbr_texs[6], 6);
				glUniform1i(main_eff.get_uniform_location("normal_map"), 6);
			}
			else if (e.first.find("house") != string::npos)
			{
				parallax = 0;
				renderer::bind(pbr_texs[30], 7);
				glUniform1i(main_eff.get_uniform_location("tex"), 7);
				renderer::bind(pbr_texs[31], 8);
				glUniform1i(main_eff.get_uniform_location("normal_map"), 8);
			}
			else if (e.first == "tractor")
			{
				parallax = 0;
				renderer::bind(texs[2], 9);
				glUniform1i(main_eff.get_uniform_location("tex"), 9);
				renderer::bind(normal_maps[1], 10);
				glUniform1i(main_eff.get_uniform_location("normal_map"), 10);
			}
			else if (e.first.find("GateTop") != string::npos)
			{
				parallax = 0;
				renderer::bind(pbr_texs[20], 11);
				glUniform1i(main_eff.get_uniform_location("tex"), 11);
				renderer::bind(pbr_texs[21], 12);
				glUniform1i(main_eff.get_uniform_location("normal_map"), 12);
			}
			else if (e.first.find("eagle") != string::npos)
			{
				parallax = 0;
				renderer::bind(texs[3], 13);
				glUniform1i(main_eff.get_uniform_location("tex"), 13);
				renderer::bind(pbr_texs[26], 14);
				glUniform1i(main_eff.get_uniform_location("normal_map"), 14);
			}
			else
			{
				parallax = 0;
				renderer::bind(pbr_texs[25], 15);
				glUniform1i(main_eff.get_uniform_location("tex"), 15);
				renderer::bind(pbr_texs[26], 16);
				glUniform1i(main_eff.get_uniform_location("normal_map"), 16);
			}

			// Render mesh
			renderer::render(m);
		}
	}

	void WorldRenderer::render_worldPBR(map<string, mesh> meshes, map<string, bool> cams, CameraFramework *mainCam,
		directional_light light, point_light point, vector<spot_light> spots, shadow_map shadow, GLint parallax)
	{
		#pragma region initialization checks
		if (!m_loadedShaders)
			load_shaders();

		if (!m_loadedTextures)
			load_textures();
		#pragma endregion

		// Bind shader
		renderer::bind(pbr_eff);

		// Bind directional light
		renderer::bind(light, "light");

		// Bind point light(s)
		renderer::bind(point, "point");

		// Bind spot light(s)
		renderer::bind(spots, "spots");

		// Set eye position
		glUniform3fv(pbr_eff.get_uniform_location("eye_pos"), 1, value_ptr(cams["freeCam"] ? mainCam->get_free_camera().get_position()
			: mainCam->get_chase_camera().get_position()));

		// Bind shadow map texture - use texture unit 1
		renderer::bind(shadow.buffer->get_depth(), 1);
		glUniform1i(pbr_eff.get_uniform_location("shadow_map"), 1);

		// Set parralax mapping related uniforms
		glUniform1f(pbr_eff.get_uniform_location("height_scale"), (height_scale = 0.25f));
		glUniform1i(pbr_eff.get_uniform_location("parallax"), parallax);

		// Bind depth map
		if (parallax == 1)
		{
			renderer::bind(pbr_texs[35], 35);
			glUniform1i(pbr_eff.get_uniform_location("depth_map"), 35);
		}

		// Render meshes
		for (auto &e : meshes)
		{
			mesh m = e.second;
			// Create MVP matrix
			mat4 M = m.get_transform().get_transform_matrix();
			mat4 V = cams["freeCam"] ? mainCam->get_free_camera().get_view() : mainCam->get_chase_camera().get_view();
			mat4 P = cams["freeCam"] ? mainCam->get_free_camera().get_projection() : mainCam->get_chase_camera().get_projection();
			mat4 MVP = P * V * M;

			auto lightMVP = lightProjectionMat * shadow.get_view() * M;

			// Set MVP matrix uniform
			glUniformMatrix4fv(pbr_eff.get_uniform_location("MVP"), 1, GL_FALSE, value_ptr(MVP));
			// Set Model matrix uniform
			glUniformMatrix4fv(pbr_eff.get_uniform_location("M"), 1, GL_FALSE, value_ptr(M));
			// Set Normal matrix uniform
			glUniformMatrix3fv(pbr_eff.get_uniform_location("N"), 1, GL_FALSE, value_ptr(m.get_transform().get_normal_matrix()));
			// Set light transform
			glUniformMatrix4fv(pbr_eff.get_uniform_location("lightMVP"), 1, GL_FALSE, value_ptr(lightMVP));

			// Bind material
			renderer::bind(m.get_material(), "mat");

			if (e.first == "tern")
			{
				continue;
			}
			else if (e.first == "moon")
			{
				parallax = 0;
				renderer::bind(texs[0], 0);
				glUniform1i(pbr_eff.get_uniform_location("albedo_map"), 0);
				renderer::bind(normal_maps[0], 1);
				glUniform1i(pbr_eff.get_uniform_location("normal_map"), 1);
				renderer::bind(pbr_texs[22], 2);
				glUniform1i(pbr_eff.get_uniform_location("metallic_map"), 2);
				renderer::bind(pbr_texs[23], 3);
				glUniform1i(pbr_eff.get_uniform_location("roughness_map"), 3);
				renderer::bind(pbr_texs[24], 4);
				glUniform1i(pbr_eff.get_uniform_location("ao_map"), 4);
			}
			else if (e.first.find("FortWall") != string::npos)
			{
				// if (parallax == 0) parallax = 1;
				renderer::bind(pbr_texs[5], 5);
				glUniform1i(pbr_eff.get_uniform_location("albedo_map"), 5);
				renderer::bind(pbr_texs[6], 6);
				glUniform1i(pbr_eff.get_uniform_location("normal_map"), 6);
				renderer::bind(pbr_texs[7], 7);
				glUniform1i(pbr_eff.get_uniform_location("metallic_map"), 7);
				renderer::bind(pbr_texs[8], 8);
				glUniform1i(pbr_eff.get_uniform_location("roughness_map"), 8);
				renderer::bind(pbr_texs[9], 9);
				glUniform1i(pbr_eff.get_uniform_location("ao_map"), 9);
			}
			else if (e.first.find("house") != string::npos)
			{
				parallax = 0;
				renderer::bind(pbr_texs[30], 10);
				glUniform1i(pbr_eff.get_uniform_location("albedo_map"), 10);
				renderer::bind(pbr_texs[31], 11);
				glUniform1i(pbr_eff.get_uniform_location("normal_map"), 11);
				renderer::bind(pbr_texs[32], 12);
				glUniform1i(pbr_eff.get_uniform_location("metallic_map"), 12);
				renderer::bind(pbr_texs[33], 13);
				glUniform1i(pbr_eff.get_uniform_location("roughness_map"), 13);
				renderer::bind(pbr_texs[34], 14);
				glUniform1i(pbr_eff.get_uniform_location("ao_map"), 14);
			}
			else if (e.first == "tractor")
			{
				parallax = 0;
				renderer::bind(texs[2], 15);
				glUniform1i(pbr_eff.get_uniform_location("albedo_map"), 15);
				renderer::bind(normal_maps[1], 16);
				glUniform1i(pbr_eff.get_uniform_location("normal_map"), 16);
				renderer::bind(pbr_texs[22], 17);
				glUniform1i(pbr_eff.get_uniform_location("metallic_map"), 17);
				renderer::bind(pbr_texs[23], 18);
				glUniform1i(pbr_eff.get_uniform_location("roughness_map"), 18);
				renderer::bind(pbr_texs[24], 19);
				glUniform1i(pbr_eff.get_uniform_location("ao_map"), 19);
			}
			else if (e.first.find("GateTop") != string::npos)
			{
				parallax = 0;
				renderer::bind(pbr_texs[20], 20);
				glUniform1i(pbr_eff.get_uniform_location("albedo_map"), 20);
				renderer::bind(pbr_texs[21], 21);
				glUniform1i(pbr_eff.get_uniform_location("normal_map"), 21);
				renderer::bind(pbr_texs[22], 22);
				glUniform1i(pbr_eff.get_uniform_location("metallic_map"), 22);
				renderer::bind(pbr_texs[23], 23);
				glUniform1i(pbr_eff.get_uniform_location("roughness_map"), 23);
				renderer::bind(pbr_texs[24], 24);
				glUniform1i(pbr_eff.get_uniform_location("ao_map"), 24);
			}
			else if (e.first.find("eagle") != string::npos)
			{
				parallax = 0;
				renderer::bind(texs[3], 25);
				glUniform1i(pbr_eff.get_uniform_location("albedo_map"), 25);
				renderer::bind(pbr_texs[26], 26);
				glUniform1i(pbr_eff.get_uniform_location("normal_map"), 26);
				renderer::bind(pbr_texs[27], 27);
				glUniform1i(pbr_eff.get_uniform_location("metallic_map"), 27);
				renderer::bind(pbr_texs[28], 28);
				glUniform1i(pbr_eff.get_uniform_location("roughness_map"), 28);
				renderer::bind(pbr_texs[29], 29);
				glUniform1i(pbr_eff.get_uniform_location("ao_map"), 29);
			}
			else
			{
				parallax = 0;
				renderer::bind(pbr_texs[25], 30);
				glUniform1i(pbr_eff.get_uniform_location("albedo_map"), 30);
				renderer::bind(pbr_texs[26], 31);
				glUniform1i(pbr_eff.get_uniform_location("normal_map"), 31);
				renderer::bind(pbr_texs[27], 32);
				glUniform1i(pbr_eff.get_uniform_location("metallic_map"), 32);
				renderer::bind(pbr_texs[28], 33);
				glUniform1i(pbr_eff.get_uniform_location("roughness_map"), 33);
				renderer::bind(pbr_texs[29], 34);
				glUniform1i(pbr_eff.get_uniform_location("ao_map"), 34);
			}

			// Render mesh
			renderer::render(m);
		}
	}
}