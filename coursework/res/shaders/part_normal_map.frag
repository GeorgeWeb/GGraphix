vec3 calc_normal(in vec3 normal, in vec3 tangent, in vec3 binormal, in sampler2D normal_map, in vec2 tex_coord)
{
	normal = normalize(normal);
	tangent = normalize(tangent);
	binormal = normalize(binormal);

	vec3 sampled_normal = texture(normal_map, tex_coord).xyz;
	sampled_normal = 2.0 * sampled_normal - vec3(1.0, 1.0, 1.0);
	
	mat3 TBN = mat3(tangent, binormal, normal);
	
	return normalize(TBN * sampled_normal);
}