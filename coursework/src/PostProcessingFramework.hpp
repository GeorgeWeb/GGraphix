#ifndef POST_PROCESSING_FRAMEWORK_H
#define POST_PROCESSING_FRAMEWORK_H
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

	class PostProcessingFramework
	{
		public:
			PostProcessingFramework();
			~PostProcessingFramework();

			void set_target_frame();
			
			void render_inverse(effect post_effect, int eff_opt);
			void render_gray(effect post_effect, int eff_opt);
			void render_sepia(effect post_effect, int eff_opt);
			void render_sobel(effect post_effect, int eff_opt);
			void render_blur(effect post_effect, int eff_opt);

		private:
			void init();

			void render_frame_buffer(effect post_effect, int eff_opt);

		private:
			geometry screen_quad;

			frame_buffer frame;
			frame_buffer temp;
	};
}

#endif !POST_PROCESSING_FRAMEWORK_H