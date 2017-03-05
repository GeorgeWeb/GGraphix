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
  vec4 specular = pow(max(dot(normal, half_vector), 0.0), mat.shininess) * mat.specular_reflection * light.light_colour;

  ambient.a = 1.0;
  diffuse.a = 1.0;
  specular.a = 1.0;

  // Calculate primary colour component
  vec4 primary = mat.emissive + ambient + diffuse;

  // Calculate final colour
  return (primary * tex_colour) + specular;
}