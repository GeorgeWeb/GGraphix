#include "LightingFramework.hpp"

namespace SpookyTown
{
	LightingFramework::LightingFramework()
	{
		load_lights();
	}

	LightingFramework::~LightingFramework()
	{ }
	
	void LightingFramework::load_lights()
	{
		load_directonal_light();
		load_spot_lights();
		load_point_lights();
	}

	void LightingFramework::load_directonal_light()
	{
		// Set directional properties
		light.set_ambient_intensity(vec4(0.00001f, 0.00001f, 0.00001f, 1.0f));
		light.set_light_colour(vec4(0.8f, 0.8f, 0.8f, 1.0f));
		light.set_direction(vec3(1.0f, 1.0f, -1.0f)); // -moon_pos
	}

	void LightingFramework::load_point_lights()
	{
		// Set point properties
		point.set_position(vec3(400.0f, 400.0f, 200.0f) + vec3(-40.0f, 20.0f, 0.0f));
		point.set_light_colour(vec4(1.0, 1.0f, 1.0f, 1.0f));
		point.set_range(50.0f);
	}

	void LightingFramework::load_spot_lights()
	{
		// Set spot properties
		spots.resize(2);
		spots[0].set_position(vec3(30.0f, 25.0f, 0.0f));
		spots[0].set_light_colour(vec4(1.0f, 1.0f, 0.8f, 1.0f));
		spots[0].set_direction(-spots[0].get_position());
		spots[0].set_range(80.0f);
		spots[0].set_power(0.0001f);

		spots[1].set_position(vec3(0.0f, 32.5f, -45.0f));
		spots[1].set_light_colour(vec4(1.0f, 1.0f, 0.0f, 1.0f));
		spots[1].set_direction(vec3(-2.0f, 2.0f, 5.0f));
		spots[1].set_range(10.0f);
		spots[1].set_power(1.0f);
	}
}