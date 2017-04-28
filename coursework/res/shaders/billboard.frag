#version 440

// Texture to use on billboards
uniform sampler2D tex;

// Incoming texture coordinate
layout(location = 0) in vec2 tex_coord;

// Outgoing colour
layout(location = 0) out vec4 colour;

void main() {
	// Colour is just texture colour
	colour = texture(tex, tex_coord);
	
	if ((colour.r == 0.0 && colour.g == 0.0 && colour.b == 0.0) ||
		(colour.r == 1.0 && colour.g == 1.0 && colour.b == 1.0)) {
		discard;
	}
}