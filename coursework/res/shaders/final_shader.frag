#version 440

// This shader requires direction.frag, point.frag and spot.frag

// Directional light structure
#ifndef DIRECTIONAL_LIGHT
#define DIRECTIONAL_LIGHT
struct directional_light {
  vec4 ambient_intensity;
  vec4 light_colour;
  vec3 light_dir;
};
#endif

// Point light information
#ifndef POINT_LIGHT
#define POINT_LIGHT
struct point_light {
  vec4 light_colour;
  vec3 position;
  float constant;
  float linear;
  float quadratic;
};
#endif

// Spot light data
#ifndef SPOT_LIGHT
#define SPOT_LIGHT
struct spot_light {
  vec4 light_colour;
  vec3 position;
  vec3 direction;
  float constant;
  float linear;
  float quadratic;
  float power;
};
#endif

// A material structure
#ifndef MATERIAL
#define MATERIAL
struct material {
  vec4 emissive;
  vec4 diffuse_reflection;
  vec4 specular_reflection;
  float shininess;
};
#endif

// Forward declarations of used functions
vec4 calculate_direction(in directional_light light, in material mat, in vec3 normal, in vec3 view_dir, in vec4 tex_colour);
vec4 calculate_point(in point_light point, in material mat, in vec3 position, in vec3 normal, in vec3 view_dir, in vec4 tex_colour, in bool gamma);
vec4 calculate_spot(in spot_light spot, in material mat, in vec3 position, in vec3 normal, in vec3 view_dir, in vec4 tex_colour, in bool gamma);
vec3 calc_normal(in vec3 normal, in vec3 tangent, in vec3 binormal, in sampler2D normal_map, in vec2 tex_coord);

// Directional light information
uniform directional_light light;
// Point lights being used in the scene
uniform point_light points[1];
// Spot lights being used in the scene
uniform spot_light spots[2];
// Material of the object being rendered
uniform material mat;
// Position of the eye
uniform vec3 eye_pos;
// Texture to sample from
uniform sampler2D tex;
// Normal map to sample from
uniform sampler2D normal_map;
uniform bool gamma;

// Incoming position
layout(location = 0) in vec3 vertex_position;
// Incoming normal
layout(location = 1) in vec3 transformed_normal;
// Incoming texture coordinate
layout(location = 2) in vec2 tex_coord;
// Incoming light space position
layout(location = 3) in vec4 vertex_light;
// Incoming tangent
layout(location = 4) in vec3 tangent_out;
// Incoming binormal
layout(location = 5) in vec3 binormal_out;

// Outgoing colour
layout(location = 0) out vec4 colour;

void main() {
	// *********************************
	// Calculate view direction
	vec3 view_dir = normalize(eye_pos - vertex_position);
	// Calculate normal from normal map
	vec3 normal_map_result = calc_normal(transformed_normal, tangent_out, binormal_out, normal_map, tex_coord);
	// Sample texture
	vec4 tex_colour = texture(tex, tex_coord);

	// *********************************
	// Calculate directional light colour
	vec4 directional_light_colour = calculate_direction(light, mat, normal_map_result, view_dir, tex_colour);

	// Sum point lights
	vec4 point_light_colour = vec4(0.0, 0.0, 0.0, 1.0);
	for (int i = 0; i < 1; i++)
		point_light_colour += calculate_point(points[i], mat, vertex_position, transformed_normal, view_dir, tex_colour, gamma);

	// Sum spot lights
	vec4 spot_light_colour = vec4(0.0, 0.0, 0.0, 1.0);
	for (int i = 0; i < 2; i++)
		spot_light_colour += calculate_spot(spots[i], mat, vertex_position, transformed_normal, view_dir, tex_colour, gamma);

	// *********************************
	// Output colour
	vec3 f_colour = (directional_light_colour + point_light_colour + spot_light_colour).rgb;
	if (gamma) f_colour = pow(f_colour, vec3(1.0 / 2.2));
	colour = vec4(f_colour, 1.0f);
}