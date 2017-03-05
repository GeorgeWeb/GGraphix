#version 440

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
float calculate_shadow(in sampler2D shadow_map, in vec4 light_space_pos);
	
// Spot light being used in the scene
uniform spot_light spot;
// Material of the object being rendered
uniform material mat;
// Position of the eye
uniform vec3 eye_pos;
// Texture to sample from
uniform sampler2D tex;
// Shadow map to sample from
uniform sampler2D shadow_map;

// Incoming position
layout(location = 0) in vec3 vertex_position;
// Incoming normal
layout(location = 1) in vec3 transformed_normal;
// Incoming texture coordinate
layout(location = 2) in vec2 tex_coord;
// Incoming light space position
layout(location = 3) in vec4 vertex_light;

// Outgoing colour
layout(location = 0) out vec4 colour;

vec4 calculate_spot(in spot_light spot, in material mat, in vec3 position, in vec3 normal, in vec3 view_dir, in vec4 tex_colour) {
	// *********************************
	vec3 light_dir = normalize(spot.position - vertex_position);

	// Calculate distance to light
	float distance = distance(spot.position, vertex_position);

	// Calculate attenuation value
	float attenuation = 1.0 / (spot.constant + spot.linear * distance + spot.quadratic * (distance * distance));

	// Calculate spot light intensity
	float intensity = pow(max(dot(-(spot.direction), light_dir), 0.0), spot.power);

	// Calculate light colour
	vec4 light_colour = spot.light_colour * attenuation * intensity;

	// Now use standard phong shading but using calculated light colour and direction
	// - note no ambient
	vec3 half_vector = normalize(light_dir + view_dir);
	// *********************************
	vec4 diffuse = max(dot(transformed_normal, light_dir), 0.0) * mat.diffuse_reflection * light_colour;
	vec4 specular = pow(max(dot(transformed_normal, half_vector), 0.0), mat.shininess) * mat.specular_reflection * light_colour;
	diffuse.a = 1.0;
	specular.a = 1.0;

	vec4 primary = mat.emissive + diffuse;

	return (primary * tex_colour) + specular;
}

void main() {
  // *********************************
  // Calculate shade factor
  float shade_factor = calculate_shadow(shadow_map, vertex_light);
  
  // Calculate view direction
  vec3 view_dir = normalize(eye_pos - vertex_position);

  // Sample texture
  vec4 tex_colour = texture(tex, tex_coord);

  // Calculate spot light
  vec4 spot_light_colour = calculate_spot(spot, mat, vertex_position, transformed_normal, view_dir, tex_colour);

  spot_light_colour.a = 1.0f;

  // Scale colour by shade
  colour = spot_light_colour * shade_factor;
  // *********************************
}