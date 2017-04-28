#include "ActorFramework.hpp"
#include "CameraFramework.hpp"

namespace SpookyTown
{
	ActorFramework::ActorFramework()
	{
		init();
	}

	ActorFramework::~ActorFramework()
	{ }
	
	void ActorFramework::init()
	{
		spawnNPC();
	}

	void ActorFramework::spawnNPC()
	{
		// Vector we will use to store randomly generated points
		vector<vec3> positions;

		// Allows creation of random points.  Note range
		default_random_engine e;
		uniform_real_distribution<float> dist(0, 200);

		// Randomly generate points
		for (auto i = 0; i < 200; ++i)
			positions.push_back(vec3(dist(e), 12.5f, dist(e)));

		// Create geometry using these points
		npc.add_buffer(positions, BUFFER_INDEXES::POSITION_BUFFER);
		// Set geometry type to points
		npc.set_type(GL_POINTS);
	}

	void ActorFramework::renderNPC(effect billboard_eff, CameraFramework *mainCam, map<string, bool> cams, texture tex)
	{
		renderer::bind(billboard_eff);
		
		auto V = cams["freeCam"] ? mainCam->get_free_camera().get_view() : mainCam->get_chase_camera().get_view();
		auto P = cams["freeCam"] ? mainCam->get_free_camera().get_projection() : mainCam->get_chase_camera().get_projection();
		auto MVP = P * V;

		glUniformMatrix4fv(billboard_eff.get_uniform_location("MV"), 1, GL_FALSE, value_ptr(V));
		glUniformMatrix4fv(billboard_eff.get_uniform_location("P"), 1, GL_FALSE, value_ptr(P));
		glUniform1f(billboard_eff.get_uniform_location("point_size"), 5.0f);

		renderer::bind(tex, 0);
		glUniform1i(billboard_eff.get_uniform_location("tex"), 0);

		renderer::render(npc);
	}
}