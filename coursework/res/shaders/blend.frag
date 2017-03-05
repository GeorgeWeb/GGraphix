#version 440

// Main textures
uniform sampler2D blend_tex[2];
// Blend map
uniform sampler2D blend_map;

// Incoming texture coordinate
layout(location = 0) in vec2 tex_coord;
// Outgoing fragment colour
layout(location = 0) out vec4 colour;

void main() {
  // *********************************
  // Sample the two main textures
  vec4 colour1 = texture(blend_tex[0], tex_coord);
  vec4 colour2 = texture(blend_tex[1], tex_coord);  
  // Sample the blend texture
  float blend_factor = texture(blend_map, tex_coord).r;

  // Mix the main samples using r component from blend value
  colour = mix(colour1, colour2, blend_factor);
  // *********************************
}