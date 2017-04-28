#ifndef WORLD_RENDERER_H
#define WORLD_RENDERER_H
#ifdef PRAGMA_ONCE
#pragma once
#endif

#include "WorldFramework.hpp"
#include "CameraFramework.hpp"
#include "ActorFramework.hpp"

namespace SpookyTown
{
	class WorldRenderer : public WorldFramework
	{
		public:
			WorldRenderer();
			~WorldRenderer() {}

			void render_skybox(map<string, mesh> meshes, mesh skybox, cubemap cube_map, map<string, bool> cams, CameraFramework *mainCam);
			void render_shadows(map<string, mesh> meshes, shadow_map shadow);
			void render_terrain(map<string, mesh> meshes, map<string, bool> cams, CameraFramework *mainCam, directional_light light);
			void render_actors(ActorFramework *actor, CameraFramework *mainCam, map<string, bool> cams);
			void render_world(map<string, mesh> meshes, map<string, bool> cams, CameraFramework *mainCam, directional_light light, 
				point_light point, vector<spot_light> spots, shadow_map shadow, GLint parallax);
			void render_worldPBR(map<string, mesh> meshes, map<string, bool> cams, CameraFramework *mainCam, directional_light light, 
				point_light point, vector<spot_light> spots, shadow_map shadow, GLint parallax);

		public:
			GLfloat height_scale = 0.05f;

		private:
			mat4 lightProjectionMat;
	};
}

#endif !WORLD_RENDERER_H