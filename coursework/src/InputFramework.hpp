#ifndef INPUT_FRAMEWORK_H
#define INPUT_FRAMEWORK_H
#ifdef PRAGMA_ONCE
#pragma once
#endif

#include <glm\glm.hpp>
#include <graphics_framework.h>
#include <vector>
#include <functional>
#include <iostream>

using namespace std;
using namespace graphics_framework;
using namespace glm;

namespace SpookyTown
{
	class InputFramework
	{
		public:
			InputFramework();
			~InputFramework();

			static function<void()> prev_eff;
			static function<void()> next_eff;
			static function<void()> parallax;
			static function<void()> blinn_phong_scene;
			static function<void()> pbr_scene;

			static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
	};
}

#endif !INPUT_FRAMEWORK_H