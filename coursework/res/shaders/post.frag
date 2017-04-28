#version 430 core

// Incoming frame data
uniform sampler2D tex;

// 1.0f / screen width
uniform float inverse_width;
// 1.0f / screen height
uniform float inverse_height;

// Incoming texture coordinate
layout(location = 0) in vec2 tex_coord;

// Outgoing colour
layout(location = 0) out vec4 colour;

const float sizeH = 900.0;
const float sizeV = 600.0;

uniform int eff_opt;

void main()
{
	// Define temp colour
	vec4 final_colour = vec4(0.0);
	// ***** IVERSION ***** //
	if (eff_opt == 1)
	{
		final_colour = vec4(1.0 - texture(tex, tex_coord).rgb, 1.0);
	}
	// ***** GRAYSCALE ***** //
	if (eff_opt == 2)
	{
		final_colour = texture(tex, tex_coord);
		float avg = (0.2126 * final_colour.r + 0.7152 * final_colour.g + 0.0722 * final_colour.b);
		final_colour = vec4(vec3(avg), 1.0);
	}
	// ***** SEPIA ***** //
	if (eff_opt == 3)
	{
		final_colour = texture(tex, tex_coord);
		float avg = (0.2126 * final_colour.r + 0.7152 * final_colour.g + 0.0722 * final_colour.b);
		final_colour = vec4(vec3(avg) + vec3(0.314, 0.169, -0.090), 1.0);
	}
	// ***** SOBEL - EDGE DETECTION ***** //
	if (eff_opt == 4)
	{
		vec4 top = texture(tex, vec2(tex_coord.x, tex_coord.y + 1.0 / sizeV));
		vec4 bottom = texture(tex, vec2(tex_coord.x, tex_coord.y - 1.0 / sizeV));
		vec4 left = texture(tex, vec2(tex_coord.x - 1.0 / sizeH, tex_coord.y));
		vec4 right = texture(tex, vec2(tex_coord.x + 1.0 / sizeH, tex_coord.y));
		vec4 topLeft = texture(tex, vec2(tex_coord.x - 1.0 / sizeH, tex_coord.y + 1.0 / sizeV));
		vec4 topRight = texture(tex, vec2(tex_coord.x + 1.0 / sizeH, tex_coord.y + 1.0 / sizeV));
		vec4 bottomLeft = texture(tex, vec2(tex_coord.x - 1.0 / sizeH, tex_coord.y - 1.0 / sizeV));
		vec4 bottomRight = texture(tex, vec2(tex_coord.x + 1.0 / sizeH, tex_coord.y - 1.0 / sizeV));
		vec4 sx = -topLeft - 2 * left - bottomLeft + topRight + 2 * right + bottomRight;
		vec4 sy = -topLeft - 2 * top - topRight + bottomLeft + 2 * bottom + bottomRight;
		vec4 sobel = sqrt(sx * sx + sy * sy);
		final_colour = vec4(sobel.rgb, 1.0);
	}
	// ***** BOXED BLUR ***** //
	if (eff_opt == 5)
	{
		vec4 sum = vec4(0.0);
		// sum textured colour values
		for (int x = -4; x <= 4; x++)
			for (int y = -4; y <= 4; y++)
				sum += texture(tex, vec2(tex_coord.x + x * (1.0 / sizeH), tex_coord.y + y * (1.0 / sizeV))) / 81.0;
		// output final(summed) colour
		final_colour = vec4(sum.rgb, 1.0);
	}

	// Output fragment colour
	colour = final_colour;
}