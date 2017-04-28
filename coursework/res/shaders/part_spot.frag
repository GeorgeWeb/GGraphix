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