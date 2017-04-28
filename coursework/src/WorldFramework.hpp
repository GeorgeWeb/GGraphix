#ifndef WORLD_FRAMEWORK_H
#define WORLD_FRAMEWORK_H
#ifdef PRAGMA_ONCE
#pragma once
#endif

#include <glm\glm.hpp>
#include <graphics_framework.h>

using namespace std;
using namespace graphics_framework;
using namespace glm;

namespace SpookyTown
{
	class WorldFramework
	{
		// FUNCTION MEMBERS
		public:
			WorldFramework(bool loadedTextures, bool loadedShaders);
			virtual ~WorldFramework() = default;
			
		protected:
			void load_shaders();
			void load_textures();
			void create_base_colours();

		// VARIABLE MEMBERS
		public:
			// texture arrays
			array<texture, 4> terrain_texs;
			array<texture, 10> texs;
			array<texture, 10> normal_maps;
			array<texture, 40> pbr_texs;
			// colours container
			map<string, vec4> basic_colours;
			// shading effects
			effect main_eff;
			effect shadow_eff;
			effect sky_eff;
			effect terrain_eff;
			effect billboard_eff;
			effect post_eff;
			effect pbr_eff;
			

		protected:
			bool m_loadedTextures;
			bool m_loadedShaders;
	};
}

#endif !WORLD_FRAMEWORK_H