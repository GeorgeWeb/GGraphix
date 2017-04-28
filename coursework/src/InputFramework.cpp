#include "InputFramework.hpp"

namespace SpookyTown
{
	function<void()> InputFramework::prev_eff;
	function<void()> InputFramework::next_eff;
	function<void()> InputFramework::parallax;
	function<void()> InputFramework::blinn_phong_scene;
	function<void()> InputFramework::pbr_scene;

	InputFramework::InputFramework() 
	{  /* Empty */  }
	InputFramework::~InputFramework() 
	{ /* Empty */ }

	void InputFramework::key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
	{
		if (key == GLFW_KEY_Z && action == GLFW_RELEASE)
			prev_eff();
		if (key == GLFW_KEY_X && action == GLFW_RELEASE)
			next_eff();
		if (key == GLFW_KEY_P && action == GLFW_RELEASE)
			parallax();
		if (key == GLFW_KEY_B && action == GLFW_RELEASE)
			blinn_phong_scene();
		if (key == GLFW_KEY_N && action == GLFW_RELEASE)
			pbr_scene();
	}
}