#include "World.hpp"
#include <glm/gtx/rotate_vector.hpp>
#include <vector>
#include <string>

namespace SpookyTown
{
	unsigned int World::post_count = 0;
	PostEffects World::postEffect = PostEffects::NOEFFECT;
	Scenes World::scene = Scenes::BPHONG;
	GLint World::s_parallax = 0;

	World::World()
	{
		// Empty constructor
		/** The init() fuction will be called for initialization instead of constructor! **/
	}

	World::~World()
	{
		if (worldCreator != nullptr)
		{
			delete worldCreator;
			worldCreator = nullptr;
		}

		if (worldRenderer != nullptr)
		{
			delete worldRenderer;
			worldRenderer = nullptr;
		}

		if (lightingFramework != nullptr)
		{
			delete lightingFramework;
			lightingFramework = nullptr;
		}

		if (actorFramework != nullptr)
		{
			delete actorFramework;
			actorFramework = nullptr;
		}

		if (postProcessor != nullptr)
		{
			delete postProcessor;
			postProcessor = nullptr;
		}

		if (mainCam != nullptr)
		{
			delete mainCam;
			mainCam = nullptr;
		}
	}

	#pragma region Helper functions: init_cursor(), move_tractor(), update_mainCam(), render_default_scene(), render_default_reordered_scene()

	bool World::init_cursor()
	{
		glfwSetInputMode(renderer::get_window(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		glfwGetCursorPos(renderer::get_window(), &cursor_x, &cursor_y);

		return true;
	}

	void World::move_tractor(vector<spot_light> &spots)
	{
		worldCreator->meshes["tractor"].get_transform().translate(tractorMove);
		// Forward
		if (worldCreator->meshes["tractor"].get_transform().position.z >= origTractorPosZ + 200.0f)
		{
			worldCreator->meshes["tractor"].get_transform().rotate(vec3(0.0f, -half_pi<float>(), 0.0f));
			origTractorPosZ = worldCreator->meshes["tractor"].get_transform().position.z;
			tractorMove = vec3(-moveSpeed, 0.0f, 0.0f);
			spots[1].set_direction(vec3(0.0f, 0.0f, 0.0f));
		}
		// Right
		if (worldCreator->meshes["tractor"].get_transform().position.x <= origTractorPosX - 100.0f)
		{
			worldCreator->meshes["tractor"].get_transform().rotate(vec3(0.0f, -half_pi<float>(), 0.0f));
			origTractorPosX = worldCreator->meshes["tractor"].get_transform().position.x;
			tractorMove = vec3(0.0f, 0.0f, -moveSpeed);
			spots[1].set_direction(vec3(0.0f, 0.0f, 0.0f));
		}
		// Backward
		if (worldCreator->meshes["tractor"].get_transform().position.z <= origTractorPosZ - 200.0f)
		{
			worldCreator->meshes["tractor"].get_transform().rotate(vec3(0.0f, -half_pi<float>(), 0.0f));
			origTractorPosZ = worldCreator->meshes["tractor"].get_transform().position.z;
			tractorMove = vec3(moveSpeed, 0.0f, 0.0f);
			spots[1].set_direction(vec3(0.0f, 0.0f, 0.0f));
		}
		// Left
		if (worldCreator->meshes["tractor"].get_transform().position.x >= origTractorPosX + 100.0f)
		{
			worldCreator->meshes["tractor"].get_transform().rotate(vec3(0.0f, -half_pi<float>(), 0.0f));
			origTractorPosX = worldCreator->meshes["tractor"].get_transform().position.x;
			tractorMove = vec3(0.0f, 0.0f, moveSpeed);
			spots[1].set_direction(vec3(-2.0f, 2.0f, 5.0f));
		}

		spots[1].rotate(vec3(0.0f, -half_pi<float>(), 0.0f));
		spots[1].move(tractorMove);
	}

	void World::update_mainCam(float delta_time)
	{
		static mesh chaser = worldCreator->meshes["eagle3"];

		if (glfwGetKey(renderer::get_window(), GLFW_KEY_1))
		{
			cams["chaseCam"] = false;
			cams["freeCam"] = true;
			mainCam->set_free_camera();
		}

		if (glfwGetKey(renderer::get_window(), GLFW_KEY_2))
		{
			cams["chaseCam"] = true;
			cams["freeCam"] = false;
			mainCam->set_chase_camera(chaser);
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

			mainCam->rotate_free_camera(get_cursor_x(), get_cursor_y());
			mainCam->move_free_camera(transVec);
			mainCam->update_free_camera(delta_time);
		}
		else if (cams["chaseCam"])
		{
			mainCam->rotate_chase_camera(get_cursor_x(), get_cursor_y());
			// Use keyboard to rotate target_mesh - QE rotate on y-axis
			if (glfwGetKey(renderer::get_window(), GLFW_KEY_Q)) {
				worldCreator->meshes["eagle3"].get_transform().rotate(vec3(0.0f, 0.05f, 0.0f));
			}

			if (glfwGetKey(renderer::get_window(), GLFW_KEY_E)) {
				worldCreator->meshes["eagle3"].get_transform().rotate(vec3(0.0f, -0.05f, 0.0f));
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
			worldCreator->meshes["eagle3"].get_transform().translate(transVec);

			vec3 distVec = worldCreator->meshes["eagle3"].get_transform().position - mainCam->get_chase_camera().get_position();
			mainCam->get_chase_camera().move(worldCreator->meshes["eagle3"].get_transform().position,
				eulerAngles(worldCreator->meshes["eagle3"].get_transform().orientation));

			mainCam->update_chase_camera(delta_time);
		}
	}

	void World::render_default_scene()
	{
		switch (scene)
		{
			case Scenes::BPHONG:
			default:
				#pragma region Blinn-Phong - Default Order
				//*** EXPERIMENTAL VALUES ***//
				lightingFramework->light.set_ambient_intensity(vec4(0.00000001f, 0.00000001f, 0.00000001f, 1.0f));
				lightingFramework->spots[0].set_power(0.0001f);
				lightingFramework->spots[0].set_range(80.0f);
				lightingFramework->spots[1].set_power(1.0f);
				lightingFramework->spots[1].set_range(10.0f);
				//*** ------------------- ***//
				// Render skybox
				worldRenderer->render_skybox(worldCreator->meshes, worldCreator->skybox, worldCreator->cube_map, cams, mainCam);
				// Render shadows
				worldRenderer->render_shadows(worldCreator->meshes, shadow);
				// Render terrain
				worldRenderer->render_terrain(worldCreator->meshes, cams, mainCam, lightingFramework->light);
				// Render world
				worldRenderer->render_world(worldCreator->meshes, cams, mainCam, lightingFramework->light,
					lightingFramework->point, lightingFramework->spots, shadow, s_parallax);
				// Render actors(NPCs)
				worldRenderer->render_actors(actorFramework, mainCam, cams);
				#pragma endregion
				break;
			case Scenes::PBR:
				#pragma region PBR - Default Order
				//*** EXPERIMENTAL VALUES ***//
				lightingFramework->light.set_ambient_intensity(vec4(0.1f, 0.1f, 0.1f, 1.0f));
				lightingFramework->spots[0].set_direction(normalize(-lightingFramework->spots[0].get_position()));
				lightingFramework->spots[0].set_power(10.0f);
				lightingFramework->spots[0].set_range(200.0f);
				lightingFramework->spots[1].set_power(0.8f);
				lightingFramework->spots[1].set_range(8.0f);
				//*** ------------------- ***//
				// Render skybox
				worldRenderer->render_skybox(worldCreator->meshes, worldCreator->skybox, worldCreator->cube_map, cams, mainCam);
				// Render shadows
				worldRenderer->render_shadows(worldCreator->meshes, shadow);
				// Render terrain
				worldRenderer->render_terrain(worldCreator->meshes, cams, mainCam, lightingFramework->light);
				// Render PBR world
				worldRenderer->render_worldPBR(worldCreator->meshes, cams, mainCam, lightingFramework->light,
					lightingFramework->point, lightingFramework->spots, shadow, s_parallax);
				// Render actors(NPCs)
				worldRenderer->render_actors(actorFramework, mainCam, cams);
				#pragma endregion
				break;
		}
		
	}

	void World::render_default_reordered_scene()
	{
		switch (scene)
		{
			case Scenes::BPHONG:
			default:
				#pragma region Blinn-Phong - Reordered Rendering
				//*** EXPERIMENTAL VALUES ***//
				lightingFramework->light.set_ambient_intensity(vec4(0.00000001f, 0.00000001f, 0.00000001f, 1.0f));
				lightingFramework->spots[0].set_power(0.0001f);
				lightingFramework->spots[0].set_range(80.0f);
				lightingFramework->spots[1].set_power(1.0f);
				lightingFramework->spots[1].set_range(10.0f);
				//*** ------------------- ***//
				// Render shadows
				worldRenderer->render_shadows(worldCreator->meshes, shadow);
				// Set frame buffer
				postProcessor->set_target_frame();
				// Render skybox
				worldRenderer->render_skybox(worldCreator->meshes, worldCreator->skybox, worldCreator->cube_map, cams, mainCam);
				// Render terrain
				worldRenderer->render_terrain(worldCreator->meshes, cams, mainCam, lightingFramework->light);
				// Render world
				worldRenderer->render_world(worldCreator->meshes, cams, mainCam, lightingFramework->light,
					lightingFramework->point, lightingFramework->spots, shadow, s_parallax);
				// Render actors(NPCs)
				worldRenderer->render_actors(actorFramework, mainCam, cams);
				#pragma endregion
			break;
			case Scenes::PBR:
				#pragma region PBR - Reordered Rendering
				//*** EXPERIMENTAL VALUES ***//
				lightingFramework->light.set_ambient_intensity(vec4(0.1f, 0.1f, 0.1f, 1.0f));
				lightingFramework->spots[0].set_direction(normalize(-lightingFramework->spots[0].get_position()));
				lightingFramework->spots[0].set_power(10.0f);
				lightingFramework->spots[0].set_range(200.0f);
				lightingFramework->spots[1].set_power(0.8f);
				lightingFramework->spots[1].set_range(8.0f);
				//*** ------------------- ***//
				// Render shadows
				worldRenderer->render_shadows(worldCreator->meshes, shadow);
				// Set frame buffer
				postProcessor->set_target_frame();
				// Render skybox
				worldRenderer->render_skybox(worldCreator->meshes, worldCreator->skybox, worldCreator->cube_map, cams, mainCam);
				// Render terrain
				worldRenderer->render_terrain(worldCreator->meshes, cams, mainCam, lightingFramework->light);
				// Render PBR world
				worldRenderer->render_worldPBR(worldCreator->meshes, cams, mainCam, lightingFramework->light,
					lightingFramework->point, lightingFramework->spots, shadow, s_parallax);
				// Render actors(NPCs)
				worldRenderer->render_actors(actorFramework, mainCam, cams);
				#pragma endregion
			break;
		}
	}

	#pragma endregion

	bool World::load_content()
	{	
		postProcessor = new PostProcessingFramework();

		// Create shadow map - use screen size
		shadow = shadow_map(renderer::get_screen_width(), renderer::get_screen_height());

		// Create world meshes
		worldCreator = new WorldCreator();
		// Create skybox
		worldCreator->create_skybox();
		// Create terrain
		worldCreator->create_terrain();
		// Create world mesh objects
		worldCreator->create_world_meshes(origTractorPosX, origTractorPosZ);

		// Create actors
		actorFramework = new ActorFramework();

		// Load & Setup lights
		lightingFramework = new LightingFramework();

		// Setup Renderer + Load textures + Load&Build shaders
		worldRenderer = new WorldRenderer();

		// Set camera properties
		cams["freeCam"] = true;
		cams["chaseCam"] = false;
		mainCam = new CameraFramework();

		input.prev_eff = []() { prev_eff(); };
		input.next_eff = []() { next_eff(); };
		input.parallax = []() { parallax(); };
		input.blinn_phong_scene = []() { blinn_phong_scene(); };
		input.pbr_scene = []() { pbr_scene(); };

		return true;
	}

	void World::prev_eff()
	{
		post_count = (post_count > 0) ? --post_count : 5;
		postEffect = static_cast<PostEffects>(post_count);
	}

	void World::next_eff()
	{
		post_count = (post_count < 5) ? ++post_count : 0;
		postEffect = static_cast<PostEffects>(post_count);
	}

	void World::parallax()
	{
		if (s_parallax == 0)
			s_parallax = 1;
		else if (s_parallax == 1)
			s_parallax = 0;
	}

	void World::blinn_phong_scene()
	{
		scene = Scenes::BPHONG;
	}

	void World::pbr_scene()
	{
		scene = Scenes::PBR;
	}

	bool World::update(float delta_time)
	{
		// Rotate gate's balls
		worldCreator->meshes["GateTop1"].get_transform().rotate(vec3(0.0f, -half_pi<float>(), 0.0f) * delta_time * 0.5f);
		worldCreator->meshes["GateTop2"].get_transform().rotate(vec3(0.0f, -half_pi<float>(), 0.0f) * delta_time * 0.5f);
		
		// Fly eagle in a circle
		elapsedTime += delta_time;
		worldCreator->meshes["eagle2"].get_transform().rotate(vec3(0.0f, 0.0f, quarter_pi<float>() * delta_time * 0.5f));
		worldCreator->meshes["eagle2"].get_transform().translate(vec3(sin(elapsedTime), 0.0, cos(elapsedTime)));

		// Update tractor mesh transform
		move_tractor(lightingFramework->spots);

		// Update the camera
		update_mainCam(delta_time);

		// update cursor position
		set_cursor_pos();

		// Update the shadow map properties from the spot light
		shadow.light_position = lightingFramework->spots[0].get_position();
		shadow.light_dir = lightingFramework->spots[0].get_direction();

		return true;
	}

	bool World::render()
	{
		switch (postEffect)
		{
			case PostEffects::NOEFFECT:
			default:
				render_default_scene();
				break;

			case PostEffects::INVERSE:
				render_default_reordered_scene();
				postProcessor->render_inverse(worldRenderer->post_eff, static_cast<int>(postEffect));
				break;

			case PostEffects::GRAYSCALE:
				render_default_reordered_scene();
				postProcessor->render_gray(worldRenderer->post_eff, static_cast<int>(postEffect));
				break;

			case PostEffects::SEPIA:
				render_default_reordered_scene();
				postProcessor->render_sepia(worldRenderer->post_eff, static_cast<int>(postEffect));
				break;

			case PostEffects::SOBEL:
				render_default_reordered_scene();
				postProcessor->render_sobel(worldRenderer->post_eff, static_cast<int>(postEffect));
				break;

			case PostEffects::BLUR:
				render_default_reordered_scene();
				postProcessor->render_blur(worldRenderer->post_eff, static_cast<int>(postEffect));
				break;
		}

		return true;
	}

	void World::init()
	{
		// Create application | Fullscreen | HD resolution
		app application("Graphics Coursework | Spooky Town", renderer::borderless, 1366, 768);

		// Set load content, update and render methods
		application.set_load_content([&]() { return load_content(); });
		application.set_initialise([&]() { return init_cursor(); });
		application.set_update([&](float delta_time) { return update(delta_time); });
		application.set_render([&]() { return render(); });
		
		// Input handling (keyboard & mouse)
		application.set_keyboard_callback(input.key_callback);
		
		// Run application
		application.run();
	}
}