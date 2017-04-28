#ifndef WORLD_H
#define WORLD_H
#ifdef PRAGMA_ONCE
#pragma once
#endif

#include "WorldCreator.hpp"
#include "WorldRenderer.hpp"
#include "CameraFramework.hpp"
#include "LightingFramework.hpp"
#include "PostProcessingFramework.hpp"
#include "InputFramework.hpp"

namespace SpookyTown
{
	enum class Scenes : char
	{
		BPHONG = 1,
		PBR = 2
	};

	enum class PostEffects : char
	{
		NOEFFECT = 0,
		INVERSE = 1,
		GRAYSCALE = 2,
		SEPIA = 3,
		SOBEL = 4,
		BLUR = 5
	};

	class World
	{
		public:
			// set a default destructor
			World();
			~World();

			// public function members
			void init();

			inline double get_cursor_x() { return cursor_x; }
			inline double get_cursor_y() { return cursor_y; }

			// Capture initial mouse position + Set cursor mode
			bool init_cursor();

			// Update cursor/mouse position
			inline void set_cursor_pos() { glfwSetCursorPos(renderer::get_window(), cursor_x, cursor_y); }

		private:
			// used in the `init()` function
			bool load_content();
			bool update(float delta_time);
			bool render();
			// helper functions
			void render_default_scene();
			void render_default_reordered_scene();
			void move_tractor(vector<spot_light> &spots);
			void update_mainCam(float delta_time);

			inline static void prev_eff();
			inline static void next_eff();
			inline static void parallax();
			inline static void blinn_phong_scene();
			inline static void pbr_scene();

		private:
			WorldCreator *worldCreator;
			WorldRenderer *worldRenderer;
		
			ActorFramework *actorFramework;

			LightingFramework *lightingFramework;

			CameraFramework *mainCam;

			InputFramework input;

			map<string, bool> cams;

			shadow_map shadow;

			GLdouble cursor_x = 0;
			GLdouble cursor_y = 0;

			GLfloat origTractorPosX = 0.0f;
			GLfloat origTractorPosZ = 0.0f;
			GLfloat moveSpeed = 0.3f;
			GLfloat elapsedTime = 0.0f;
			vec3 tractorMove = vec3(0.0f, 0.0f, moveSpeed);

			PostProcessingFramework *postProcessor;
			static PostEffects postEffect;
			static unsigned int post_count;

			static Scenes scene;

			static GLint s_parallax;
	};
}

#endif !WORLD_H