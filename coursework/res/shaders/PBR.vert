#version 440

// Model matrix
uniform mat4 MVP;
// View matrix
uniform mat4 M;

// Normal matrix
uniform mat3 N;

// The light transformation matrix
uniform mat4 lightMVP;

// Incoming position
layout(location = 0) in vec3 position;
// Incoming normal
layout(location = 2) in vec3 normal;
// Incoming binormal
layout(location = 3) in vec3 binormal;
// Incoming tangent
layout(location = 4) in vec3 tangent;
// Incoming texture coordinate
layout(location = 10) in vec2 tex_coord_in;

// Outgoing vertex position
layout(location = 0) out vec3 vertex_position;
// Transformed normal
layout(location = 1) out vec3 transformed_normal;
// Outgoing tex_coord
layout(location = 2) out vec2 tex_coord_out;
// Outgoing tangent
layout(location = 3) out vec3 binormal_out;
// Outgoing binormal
layout(location = 4) out vec3 tangent_out;
// Outgoing position in light space
layout(location = 5) out vec4 vertex_light;

void main()
{
	// Transform position into screen space
	gl_Position = MVP * vec4(position, 1.0);
	// Transform position into world space
	vertex_position = (M * vec4(position, 1.0)).xyz;
	// Transform normal
	transformed_normal = N * normal;
	// Pass through texture coordinate
	tex_coord_out = tex_coord_in;
	// Transform position into light space
	vertex_light = lightMVP * vec4(position, 1.0);
}