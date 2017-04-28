#include "PostProcessingFramework.hpp"

namespace SpookyTown
{
	PostProcessingFramework::PostProcessingFramework()
	{
		init();
	}

	PostProcessingFramework::~PostProcessingFramework()
	{ /* Empty */ }
	
	void PostProcessingFramework::init()
	{
		// Create frame buffer - use screen width and height
		frame = frame_buffer(renderer::get_screen_width(), renderer::get_screen_height());
		// Create screen quad
		vector<vec3> positions{ vec3(-1.0f, -1.0f, 0.0f), vec3(1.0f, -1.0f, 0.0f), vec3(-1.0f, 1.0f, 0.0f), vec3(1.0f, 1.0f, 0.0f) };
		vector<vec2> tex_coords{ vec2(0.0, 0.0), vec2(1.0f, 0.0f), vec2(0.0f, 1.0f), vec2(1.0f, 1.0f) };
		screen_quad.add_buffer(positions, BUFFER_INDEXES::POSITION_BUFFER);
		screen_quad.add_buffer(tex_coords, BUFFER_INDEXES::TEXTURE_COORDS_0);
		screen_quad.set_type(GL_TRIANGLE_STRIP);
	}

	void PostProcessingFramework::set_target_frame()
	{
		// Set render target to frame buffer
		renderer::set_render_target(frame);
		// Clear frame
		renderer::clear();
	}
	
	void PostProcessingFramework::render_inverse(effect post_effect, int eff_opt)
	{
		render_frame_buffer(post_effect, eff_opt);
	}

	void PostProcessingFramework::render_gray(effect post_effect, int eff_opt)
	{
		render_frame_buffer(post_effect, eff_opt);
	}

	void PostProcessingFramework::render_sepia(effect post_effect, int eff_opt)
	{
		render_frame_buffer(post_effect, eff_opt);
	}

	void PostProcessingFramework::render_sobel(effect post_effect, int eff_opt)
	{
		render_frame_buffer(post_effect, eff_opt);
	}

	void PostProcessingFramework::render_blur(effect post_effect, int eff_opt)
	{
		render_frame_buffer(post_effect, eff_opt);
	}

	#pragma region Helper functions
	
	void PostProcessingFramework::render_frame_buffer(effect post_effect, int eff_opt)
	{
		// Set render target back to the screen
		renderer::set_render_target();
		// Bind Tex effect
		renderer::bind(post_effect);
		// MVP is now the identity matrix
		auto MVP = mat4();
		// Set MVP matrix uniform
		glUniformMatrix4fv(post_effect.get_uniform_location("MVP"), 1, GL_FALSE, value_ptr(MVP));
		// Set effect option uniform
		glUniform1i(post_effect.get_uniform_location("eff_opt"), eff_opt);
		// Bind texture from frame buffer
		renderer::bind(frame.get_frame(), 1);
		// Set the tex uniform
		glUniform1i(post_effect.get_uniform_location("tex"), 1);
		// Render the screen quad
		renderer::render(screen_quad);
	}
	
	#pragma endregion
}