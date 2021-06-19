#version 120                  // GLSL 1.20

varying vec3 v_position;
varying vec2 v_texcoord;
varying vec3 v_normal;

uniform sampler2D u_diffuse_texture;

uniform	vec3 u_light_position; 
uniform vec3 u_light_color; 

uniform float u_obj_shininess; 

uniform vec3 u_camera_position; 

uniform mat4 u_model_matrix; 
uniform mat3 u_normal_matrix; 

void main()
{
	// world coordinate
	vec3 position_wc = (u_model_matrix * vec4(v_position, 1.0f)).xyz;
	vec3 normal_wc	 = normalize(u_normal_matrix * v_normal);
	
	// set ambient
	float ambientStrength = 0.2;
	vec3 ambient = ambientStrength * u_light_color;

	// set diffuse
	vec3 light_dir = normalize(u_light_position - position_wc);
	float diff = max(dot(normal_wc, light_dir), 0.0);
	vec3 diffuse = diff * u_light_color;

	// set specular
	vec3 reflect_dir = reflect(-light_dir, normal_wc);
	vec3 view_dir = normalize(u_camera_position - position_wc);
	float rdotv = pow(max(dot(view_dir, reflect_dir), 0.0), u_obj_shininess);
	vec3 specular = rdotv * u_light_color;

	vec3 color = (ambient + (1/pow(length(u_light_position - position_wc), 4)) * (diffuse + specular)) * texture2D(u_diffuse_texture, v_texcoord).xyz;

	gl_FragColor = vec4(color, 1.0f);

	// gl_FragColor = vec4(v_color, 1.0f);
}