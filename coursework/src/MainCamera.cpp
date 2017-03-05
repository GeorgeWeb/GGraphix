#include "MainCamera.h"

MainCamera::MainCamera()
{
	// Empty
}

free_camera &MainCamera::get_free_camera() { return freeCam; }

chase_camera &MainCamera::get_chase_camera() { return chaseCam; }

void MainCamera::set_free_camera()
{
	freeCam.set_position(vec3(0.0f, 200.0f, 0.0f));
	freeCam.set_target(vec3(0.0f, 0.0f, 0.0f));
	aspectRatio = static_cast<float>(renderer::get_screen_width()) / static_cast<float>(renderer::get_screen_height());
	freeCam.set_projection(quarter_pi<float>(), aspectRatio, 2.414f, 1000.0f);
}

void MainCamera::set_chase_camera(mesh chaseObj)
{
	chaseCam.set_pos_offset(vec3(30.0f, 50.0f, 30.0f));
	chaseCam.set_springiness(0.5f);
	chaseCam.move(chaseObj.get_transform().position, eulerAngles(chaseObj.get_transform().orientation));
	aspectRatio = static_cast<float>(renderer::get_screen_width()) / static_cast<float>(renderer::get_screen_height());
	chaseCam.set_projection(quarter_pi<float>(), aspectRatio, 2.414f, 1000.0f);
}

void MainCamera::rotate_free_camera(double cursor_x, double cursor_y)
{
	// The ratio of pixels to rotation - remember the fov
	static double ratio_width = quarter_pi<float>() / static_cast<float>(renderer::get_screen_width());
	static double ratio_height = (quarter_pi<float>() * (static_cast<float>(renderer::get_screen_height()) / 
		static_cast<float>(renderer::get_screen_width()))) / static_cast<float>(renderer::get_screen_height());

	double current_x = 0;
	double current_y = 0;

	// Get the current cursor position
	glfwGetCursorPos(renderer::get_window(), &current_x, &current_y);

	// Calculate delta of cursor positions from last frame
	float delta_x = static_cast<float>(current_x) - static_cast<float>(cursor_x);
	float delta_y = static_cast<float>(current_y) - static_cast<float>(cursor_y);

	// Multiply deltas by ratios - gets actual change in orientation
	delta_x *= ratio_width;
	delta_y *= ratio_height;

	// Rotate camera by delta
	freeCam.rotate(delta_x, -delta_y);
}

void MainCamera::rotate_chase_camera(double cursor_x, double cursor_y)
{
	// The ratio of pixels to rotation - remember the fov
	static double ratio_width = quarter_pi<float>() / static_cast<float>(renderer::get_screen_width());
	static double ratio_height = (quarter_pi<float>() * (static_cast<float>(renderer::get_screen_height()) /
		static_cast<float>(renderer::get_screen_width()))) / static_cast<float>(renderer::get_screen_height());

	double current_x = 0;
	double current_y = 0;

	// Get the current cursor position
	glfwGetCursorPos(renderer::get_window(), &current_x, &current_y);

	// Calculate delta of cursor positions from last frame
	float delta_x = static_cast<float>(current_x) - static_cast<float>(cursor_x);
	float delta_y = static_cast<float>(current_y) - static_cast<float>(cursor_y);

	// Multiply deltas by ratios - gets actual change in orientation
	delta_x *= ratio_width;
	delta_y *= ratio_height;

	// Rotate camera by delta
	chaseCam.rotate(vec3(-delta_y, -delta_x, 0.0f));
}

void MainCamera::move_free_camera(vec3 translate)
{
	// Move camera
	freeCam.move(translate);
}

void MainCamera::update_free_camera(float delta_time)
{
	// Update the camera
	freeCam.update(delta_time);
}

void MainCamera::update_chase_camera(float delta_time)
{
	chaseCam.update(delta_time);
}