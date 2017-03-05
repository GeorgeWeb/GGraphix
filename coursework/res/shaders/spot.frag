#version 440

// Spot light data
struct spot_light {
  vec4 light_colour;
  vec3 position;
  vec3 direction;
  float constant;
  float linear;
  float quadratic;
  float power;
};

// Material data
struct material {
  vec4 emissive;
  vec4 diffuse_reflection;
  vec4 specular_reflection;
  float shininess;
};

// Spot light being used in the scene
uniform spot_light spot;
// Material of the object being rendered
uniform material mat;
// Position of the eye (camera)
uniform vec3 eye_pos;
// Texture to sample from
uniform sampler2D tex;

// Incoming position
layout(location = 0) in vec3 vertex_position;
// Incoming normal
layout(location = 1) in vec3 transformed_normal;
// Incoming texture coordinate
layout(location = 2) in vec2 tex_coord;

// Outgoing colour
layout(location = 0) out vec4 colour;

void main() {
  // *********************************
  // Calculate direction to the light
  vec3 light_dir = normalize(spot.position - vertex_position);

  // Calculate distance to light
  float distance = distance(spot.position, vertex_position);

  // Calculate attenuation value
  float attenuation = 1.0 / (spot.constant + spot.linear * distance + spot.quadratic * (distance * distance));

  // Calculate spot light intensity
  float intensity = pow(max(dot(-(spot.direction), light_dir), 0.0), spot.power);

  // Calculate light colour
  vec4 light_colour = spot.light_colour * attenuation * intensity;

  // Calculate view direction
  // *********************************
  vec3 view_dir = normalize(eye_pos - vertex_position);
  vec3 half_vector = normalize(light_dir + view_dir);

  // Now use standard phong shading but using calculated light colour and direction
  // - note no ambient
  // *********************************
  vec4 diffuse = max(dot(transformed_normal, light_dir), 0.0) * mat.diffuse_reflection * light_colour;
  vec4 specular = pow(max(dot(transformed_normal, half_vector), 0.0), mat.shininess) * mat.specular_reflection * light_colour;
  diffuse.a = 1.0;
  specular.a = 1.0;
  // *********************************
  vec4 tex_colour = texture(tex, tex_coord);
  vec4 primary = mat.emissive + diffuse;
  // *********************************
  colour = (primary * tex_colour) + specular;
  // *********************************
}