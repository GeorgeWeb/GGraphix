#ifndef WORLD_CREATOR_H
#define WORLD_CREATOR_H
#ifdef PRAGMA_ONCE
#pragma once
#endif

#include "WorldFramework.hpp"

namespace SpookyTown
{
	class WorldCreator : public WorldFramework
	{
		public:
			// constructors, destructors
			WorldCreator();
			~WorldCreator();
			// functions
			void create_terrain();
			void create_world_meshes(float &movablePosX, float &movablePosZ);
			void create_skybox();
		
		public:
			map<string, mesh> meshes;
			cubemap cube_map;
			mesh skybox;
			float shininess;
	};
}

#endif !WORLD_CREATOR_H