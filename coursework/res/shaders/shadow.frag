#version 440

// Directional light data structure
#ifndef DIRECTIONAL_LIGHT
#define DIRECTIONAL_LIGHT
struct directional_light {
	vec4 ambient_intensity;
	vec4 light_colour;
	vec3 light_dir;
};
#endif

// Point light data structure
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

// Spot light data structure
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
vec4 calculate_point(in point_light point, in material mat, in vec3 position, in vec3 normal, in vec3 view_dir, in vec4 tex_colour);
vec4 calculate_spot(in spot_light spot, in material mat, in vec3 position, in vec3 normal, in vec3 view_dir, in vec4 tex_colour);
vec3 calc_normal(in vec3 normal, in vec3 tangent, in vec3 binormal, in sampler2D normal_map, in vec2 tex_coord);
float calculate_shadow(in sampler2D shadow_map, in vec4 light_space_pos);

// Directional light being used in the scene
uniform directional_light light;
// Point light(s) being used in the scene
uniform point_light point;
// Spot light being used in the scene
uniform spot_light spots[2];
// Material of the object being rendered
uniform material mat;
// Position of the eye
uniform vec3 eye_pos;
// Texture to sample from
uniform sampler2D tex;
// Normal map to sample from
uniform sampler2D normal_map;
// Shadow map to sample from
uniform sampler2D shadow_map;

// PARALLAX MAPPING
uniform sampler2D depth_map;
uniform int parallax;
uniform float height_scale;

// Incoming position
layout(location = 0) in vec3 vertex_position;
// Incoming normal
layout(location = 1) in vec3 transformed_normal;
// Incoming texture coordinate
layout(location = 2) in vec2 tex_coord;
// Incoming light space position
layout(location = 3) in vec4 vertex_light;
// Incoming tangent
layout(location = 4) in vec3 tangent;
// Incoming binormal
layout(location = 5) in vec3 binormal;
// Incoming tex_weight
layout(location = 6) in vec4 vertex_tex_weight;

// Outgoing colour
layout(location = 0) out vec4 colour;

// PARALLAX MAPPING
vec2 ParallaxMapping(vec2 tex_coord, vec3 view_dir)
{
	// number of depth layers
	const float minLayers = 10;
	const float maxLayers = 20;
	float numLayers = mix(maxLayers, minLayers, abs(dot(vec3(0.0, 0.0, 1.0), view_dir)));
	// calculate the size of each layer
	float layerDepth = 1.0 / numLayers;
	// depth of current layer
	float currentLayerDepth = 0.0;
	// the amount to shift the texture coordinates per layer (from vector P)
	vec2 P = view_dir.xy / view_dir.z * height_scale;
	vec2 deltaTexCoords = P / numLayers;

	// get initial values
	vec2  currentTexCoords = tex_coord;
	float currentDepthMapValue = texture(depth_map, currentTexCoords).r;

	while (currentLayerDepth < currentDepthMapValue)
	{
		// shift texture coordinates along direction of P
		currentTexCoords -= deltaTexCoords;
		// get depthmap value at current texture coordinates
		currentDepthMapValue = texture(depth_map, currentTexCoords).r;
		// get depth of next layer
		currentLayerDepth += layerDepth;
	}

	// -- parallax occlusion mapping interpolation from here on
	// get texture coordinates before collision (reverse operations)
	vec2 prevTexCoords = currentTexCoords + deltaTexCoords;

	// get depth after and before collision for linear interpolation
	float afterDepth = currentDepthMapValue - currentLayerDepth;
	float beforeDepth = texture(depth_map, prevTexCoords).r - currentLayerDepth + layerDepth;

	// interpolation of texture coordinates
	float weight = afterDepth / (afterDepth - beforeDepth);
	vec2 finalTexCoords = prevTexCoords * weight + currentTexCoords * (1.0 - weight);

	return currentTexCoords;
}

void main()
{
  // Calculate shade factor
  float shade_factor = calculate_shadow(shadow_map, vertex_light);
  
  // Calculate view direction
  vec3 view_dir = normalize(eye_pos - vertex_position);  

  vec2 tex_coords;
  if (parallax == 1)
	  tex_coords = ParallaxMapping(tex_coord, view_dir);
  else if (parallax == 0)
	  tex_coords = tex_coord;
  
  // Sample texture
  vec4 tex_colour = texture(tex, tex_coords);
  vec4 final_tex_colour = tex_colour;	  

  // Calculate normal from normal map
  vec3 normal_map_result = calc_normal(transformed_normal, tangent, binormal, normal_map, tex_coords);

  // Calculate directional light colour
  vec4 directional_light_colour = calculate_direction(light, mat, normal_map_result, view_dir, final_tex_colour);

  // Calculate point light colour
  vec4 point_light_colour = calculate_point(point, mat, vertex_position, normal_map_result, view_dir, final_tex_colour);

  // Calculate spot light colour
  vec4 spot_light_colour = vec4(0.0, 0.0, 0.0, 1.0);
  for (int i = 0; i < spots.length(); i++)
  {
	 spot_light_colour += calculate_spot(spots[i], mat, vertex_position, normal_map_result, view_dir, final_tex_colour);
  }

  directional_light_colour.a = 1.0;
  point_light_colour.a = 1.0;
  spot_light_colour.a = 1.0;

  // Scale colour by shade
  colour = directional_light_colour / 3.0 + point_light_colour + spot_light_colour * shade_factor;
}