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
// Textures to sample from
uniform sampler2D albedo_map;
uniform sampler2D normal_map;
uniform sampler2D metallic_map;
uniform sampler2D roughness_map;
uniform sampler2D ao_map;
// Shadow map to sample from
uniform sampler2D shadow_map;
// tweak this one later...
// uniform float exposure;

// Incoming vertex position
layout(location = 0) in vec3 vertex_position;
// Incoming normal
layout(location = 1) in vec3 transformed_normal;
// Incoming tex_coord
layout(location = 2) in vec2 tex_coord_out;
// Incoming binormal
layout(location = 3) in vec3 binormal_out;
// Incoming tangent
layout(location = 4) in vec3 tangent_out;
// Incoming light space position
layout(location = 5) in vec4 vertex_light;
// PARALLAX MAPPING
uniform sampler2D depth_map;
uniform int parallax;
uniform float height_scale;

// Outgoing colour
layout(location = 0) out vec4 colour;

const float PI = 3.14159265359;
const float EPSILON = 0.00001;

/* Forward declarations of used functions */
// ------------------------------------------------------------------------- //
float DistributionGGX(vec3 N, vec3 H, float roughness)
{
	float a = roughness*roughness;
	float a2 = a*a;
	float NdotH = max(dot(N, H), 0.0);
	float NdotH2 = NdotH*NdotH;

	float nom = a2;
	float denom = (NdotH2 * (a2 - 1.0) + 1.0);
	denom = PI * denom * denom;

	return nom / denom;
}
// ------------------------------------------------------------------------- //
float GeometrySchlickGGX(float NdotV, float roughness)
{
	float r = (roughness + 1.0);
	float k = (r*r) / 8.0;

	float nom = NdotV;
	float denom = NdotV * (1.0 - k) + k;

	return nom / denom;
}
// ------------------------------------------------------------------------- //
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
	float NdotV = max(dot(N, V), 0.0);
	float NdotL = max(dot(N, L), 0.0);
	float ggx2 = GeometrySchlickGGX(NdotV, roughness);
	float ggx1 = GeometrySchlickGGX(NdotL, roughness);

	return ggx1 * ggx2;
}
// ------------------------------------------------------------------------- //
vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
	return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}
// ------------------------------------------------------------------------- //
vec4 calculate_direction(in directional_light light, in material mat, in vec3 normal, in vec3 view_dir, in vec4 tex_colour)
{
	// *********************************
	// Calculate ambient component
	vec4 ambient = mat.diffuse_reflection  * light.ambient_intensity;

	// Calculate diffuse component
	vec4 diffuse = max(dot(normal, light.light_dir), 0.0) * mat.diffuse_reflection * light.light_colour;

	// Calculate half vector
	vec3 half_vector = normalize(light.light_dir + view_dir);

	// Calculate specular component
	vec4 specular = pow(max(dot(normal, half_vector), 0.0), mat.shininess * 4.0) * mat.specular_reflection * light.light_colour;

	ambient.a = 1.0;
	diffuse.a = 1.0;
	specular.a = 1.0;

	// Calculate primary colour component
	vec4 primary = mat.emissive + ambient + diffuse;

	// Calculate final colour
	return (primary * tex_colour) + specular;
}
// ------------------------------------------------------------------------- //
vec3 getNormalFromMap(vec2 tex_coord)
{
	vec3 tangent_normal = texture(normal_map, tex_coord).xyz * 2.0 - 1.0;

	vec3 Q1 = dFdx(vertex_position);
	vec3 Q2 = dFdy(vertex_position);
	vec2 st1 = dFdx(tex_coord);
	vec2 st2 = dFdy(tex_coord);

	vec3 N = normalize(transformed_normal);
	vec3 T = normalize(Q1*st2.t - Q2*st1.t);
	vec3 B = -normalize(cross(N, T));
	mat3 TBN = mat3(T, B, N);

	return normalize(TBN * tangent_normal);
}
// ------------------------------------------------------------------------- //
float calculate_shadow(in sampler2D shadow_map, in vec4 light_space_pos) {
	// Get light screen space coordinate
	vec3 proj_coords = light_space_pos.xyz / light_space_pos.w;
	// Use this to calculate texture coordinates for shadow map
	vec2 shadow_tex_coords;
	// This is a bias calculation to convert to texture space
	shadow_tex_coords.x = 0.5 * proj_coords.x + 0.5;
	shadow_tex_coords.y = 0.5 * proj_coords.y + 0.5;
	// Check shadow coord is in range
	if (shadow_tex_coords.x < 0 || shadow_tex_coords.x > 1 || shadow_tex_coords.y < 0 || shadow_tex_coords.y > 1) {
		return 1.0;
	}
	float z = 0.5 * proj_coords.z + 0.5;
	// *********************************
	// Now sample the shadow map, return only first component (.x/.r)
	float depth = texture(shadow_map, shadow_tex_coords).x;
	// *********************************
	// Check if depth is in range.  Add a slight epsilon for precision
	if (depth == 0.0) {
		return 1.0;
	}
	else if (depth < z + EPSILON) {
		return 0.5;
	}
	else {
		return 1.0;
	}
}
// ------------------------------------------------------------------------- //
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

	return currentTexCoords;
}
// ------------------------------------------------------------------------- //

void main()
{
	// Calculate view direction
	vec3 V = normalize(eye_pos - vertex_position);

	vec2 tex_coords;
	if (parallax == 1)
		tex_coords = ParallaxMapping(tex_coord_out, V);
	else if(parallax == 0)
		tex_coords = tex_coord_out;

	// calculate per-light radiance
	vec3 albedo = pow(texture(albedo_map, tex_coords).rgb, vec3(2.2));
	float metallic = texture(metallic_map, tex_coords).r;
	float roughness = texture(roughness_map, tex_coords).r;
	float ao = texture(ao_map, tex_coords).r;

	// Calculate normal from normal map
	vec3 N = getNormalFromMap(tex_coords);

	// Calculate shade factor
	float shade_factor = calculate_shadow(shadow_map, vertex_light);

	// Calculate directional light colour
	vec3 directional_light_colour = calculate_direction(light, mat, N, V, vec4(albedo, 1.0)).rgb;
		
    // calculate reflectance at normal incidence; if dia-electric (like plastic) use F0 
    // of 0.04 and if it's a metal, use their albedo color as F0 (metallic workflow)    
	vec3 F0 = vec3(0.04);
	F0 = mix(F0, albedo, metallic);
	
	#pragma region Relfectance equation for SPOT LIGHTS
	vec3 spot_Lo = vec3(0.0);
	for (int i = 0; i < spots.length(); ++i)
	{
		// calculate per-light radiance
		vec3 L = normalize(spots[i].position - vertex_position);
		vec3 H = normalize(V + L);
		float distance = length(spots[i].position - vertex_position);
		float attenuation = 1.0 / (spots[i].constant + spots[i].linear * distance + spots[i].quadratic * (distance * distance));
		vec3 radiance = vec3(spots[i].light_colour * attenuation);

		// Cook-Torrance BRDF
		float NDF = DistributionGGX(N, H, roughness);
		float G = GeometrySmith(N, V, L, roughness);
		vec3 F = fresnelSchlick(max(dot(H, V), 0.0), F0);

		vec3 nominator = NDF * G * F;
		float denominator = 4 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.001;
		vec3 brdf = nominator / denominator;

		// kS is equal to Fresnel
		vec3 kS = F;
		vec3 kD = vec3(1.0) - kS;
		kD *= 1.0 - metallic;

		// scale light by NdotL
		float NdotL = max(dot(N, L), 0.0);

		// add to outgoing radiance Lo
		spot_Lo += (kD * albedo / PI + brdf) * radiance * NdotL;
	}
	#pragma endregion

	#pragma region Relfectance equation for POINT LIGHTS
	vec3 point_Lo = vec3(0.0);
	{
		// calculate per-light radiance
		vec3 L = normalize(point.position - vertex_position);
		vec3 H = normalize(V + L);
		float distance = length(point.position - vertex_position);
		float attenuation = 1.0 / (point.constant + point.linear * distance + point.quadratic * (distance * distance));
		vec3 radiance = vec3(point.light_colour * attenuation);

		// Cook-Torrance BRDF
		float NDF = DistributionGGX(N, H, roughness);
		float G = GeometrySmith(N, V, L, roughness);
		vec3 F = fresnelSchlick(max(dot(H, V), 0.0), F0);

		vec3 nominator = NDF * G * F;
		float denominator = 4 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.001;
		vec3 brdf = nominator / denominator;

		// kS is equal to Fresnel
		vec3 kS = F;
		vec3 kD = vec3(1.0) - kS;
		kD *= 1.0 - metallic;

		// scale light by NdotL
		float NdotL = max(dot(N, L), 0.0);

		// add to outgoing radiance Lo
		point_Lo = (kD * albedo / PI + brdf) * radiance * NdotL;
	}
	#pragma endregion
	
	// AMBIENT lighting is the already calculated DIRECTIONAL light's colour.
	vec3 ambient = directional_light_colour * albedo * ao;

	// final colour
	//vec3 color = ambient + point_light_colour.rgb + Lo;
	//color += spot_light_colour.rgb * shade_factor;
	vec3 color = ambient + point_Lo + spot_Lo  * shade_factor;

	// HDR tonemapping
	color = color / (color + vec3(1.0));
	// gamma correction
	color = pow(color, vec3(1.0 / 2.2));

	// output the final colour
	colour = vec4(color, 1.0);
}