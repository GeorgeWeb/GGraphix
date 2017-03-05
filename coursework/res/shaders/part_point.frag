vec4 calculate_point(in point_light point, in material mat, in vec3 position, in vec3 normal, in vec3 view_dir, in vec4 tex_colour, in bool gamma) {
// *********************************
  // Get distance between point light and vertex
  float distance = distance(point.position, position);
  // Calculate attenuation factor
  float attenuation = 1.0f / (point.constant + point.linear * distance + point.quadratic * (gamma ? distance * distance : distance));
  // Calculate light colour
  vec4 light_colour = point.light_colour * attenuation;
  // Calculate light dir
  vec3 light_dir = normalize(point.position - vertex_position);

  // Now use standard phong shading but using calculated light colour and direction
  // - note no ambient
  vec3 half_vector = normalize(light_dir + view_dir);
  // *********************************
  vec4 diffuse = max(dot(normal, light_dir), 0.0) * mat.diffuse_reflection * light_colour;
  vec4 specular = pow(max(dot(normal, half_vector), 0.0), mat.shininess) * mat.specular_reflection * light_colour;
  diffuse.a = 1.0;
  specular.a = 1.0;
  
  vec4 primary = mat.emissive + diffuse;

  return (primary * tex_colour) + specular;
}