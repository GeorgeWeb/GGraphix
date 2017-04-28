#ifndef MAIN_CAMERA_H
#define MAIN_CAMERA_H
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
	class CameraFramework
	{
	public:
		CameraFramework();
		~CameraFramework() = default;

		free_camera &get_free_camera();
		chase_camera &get_chase_camera();

		void set_free_camera();
		void rotate_free_camera(double cursor_x, double cursor_y);
		void move_free_camera(vec3 translate);
		void update_free_camera(float delta_time);

		void set_chase_camera(mesh chaseObj);
		void rotate_chase_camera(double cursor_x, double cursor_y);
		void update_chase_camera(float delta_time);

	private:
		free_camera freeCam;
		chase_camera chaseCam;

		float aspectRatio;
	};
}

#endif !MAIN_CAMERA_H