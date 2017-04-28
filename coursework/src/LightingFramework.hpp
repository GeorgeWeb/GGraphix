#ifndef LIGHTING_FRAMEOWORK_H
#define LIGHTING_FRAMEOWORK_H
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
	class LightingFramework
	{
		public:
			LightingFramework();
			~LightingFramework();

		private:
			void load_lights();
			void load_directonal_light();
			void load_point_lights();
			void load_spot_lights();

		public:
			directional_light light;
			point_light point;
			vector<spot_light> spots;
	};
}

#endif !LIGHTING_FRAMEOWORK_H