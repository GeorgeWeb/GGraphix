#ifndef WORLD_H
#define WORLD_H
#pragma once

#include "MainCamera.h"
#include <vector>

class World
{
	public:
		// set a default destructor
		World();
		~World() = default;
		
		// public function members
		void init();
		
		inline double get_cursor_x() { return cursor_x; }
		inline double get_cursor_y() { return cursor_y; }

		// Capture initial mouse position + Set cursor mode
		bool init_cursor();

		// Update cursor/mouse position
		inline void set_cursor_pos() { glfwSetCursorPos(renderer::get_window(), cursor_x, cursor_y); }

	private:
		// function members
		void create_terrain();
		void create_skybox();
		void create_base_colours();
		void load_shaders();
		void load_textures();

		bool load_content();
		bool update(float delta_time);
		bool render();
		void render_skybox();

		// variable members
		map<string, vec4> basic_colours;
		map<string, mesh> meshes;
		map<string, mesh> shadow_meshes;
		mesh skybox;

		cubemap cube_map;

		effect eff;
		effect base_eff;
		effect shadow_eff;
		effect sky_eff;

		// Temporary
		effect main_eff;
		spot_light spot;
		texture tex;
		target_camera cam;
		
		shadow_map shadow;
		frame_buffer frame;

		directional_light light;
		vector<point_light> points;
		vector<spot_light> spots;

		MainCamera mainCam;

		array<texture, 10> texs;
		array<texture, 10> normal_maps;
		texture blend_map;

		double cursor_x = 0;
		double cursor_y = 0;
		
		float origTractorPosX = 0.0f;
		float origTractorPosZ = 0.0f;
		float moveSpeed = 0.3f;
		vec3 tractorMove = vec3(0.0f, 0.0f, moveSpeed);

		float elapsedTime = 0.0f;

		bool gamma = true;

		map<string, bool> cams;
};

#endif !WORLD_H