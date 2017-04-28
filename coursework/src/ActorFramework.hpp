#ifndef ACTOR_FRAMEOWORK_H
#define ACTOR_FRAMEOWORK_H
#ifdef PRAGMA_ONCE
#pragma once
#endif

#include <glm\glm.hpp>
#include <graphics_framework.h>
#include <vector>

using namespace std;
using namespace graphics_framework;
using namespace glm;

namespace SpookyTown
{
	class CameraFramework;	

	class ActorFramework
	{
		friend class WorldRenderer;

		public:
			ActorFramework();
			~ActorFramework();

		private:
			void init();
			void spawnNPC();
			void renderNPC(effect billboard_eff, CameraFramework *mainCam, map<string, bool> cams, texture tex);

		private:
			geometry npc;
	};
}

#endif !ACTOR_FRAMEOWORK_H