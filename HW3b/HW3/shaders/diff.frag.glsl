#version 130
// ^ Change this to version 130 if you have compatibility issues

//these are the interpolated values out of the rasterizer, so you can't know
//their specific values without knowing the vertices that contributed to them

uniform vec3 u_LightColor; //Add light color here
uniform vec3 u_CameraPos;  //Add camera position here   

in vec4 fs_Normal;
in vec4 fs_LightVector;
in vec4 fs_Color;
in vec3 fs_Position;  // Need position information

out vec4 out_Color;

void main()
{
    // Material base color (before shading)
    vec4 diffuseColor = fs_Color;

    // Calculate the diffuse term
    float diffuseTerm = dot(normalize(fs_Normal), normalize(fs_LightVector));
    // Avoid negative lighting values
    diffuseTerm = clamp(diffuseTerm, 0, 1);

	float n = 100;
	float specularTerm = 1;
	float cos;

	vec4 r = 2*fs_Normal - fs_LightVector;
	vec3 R = vec3(r[0], r[1], r[2]);
	vec3 V = u_CameraPos - fs_Position ;
	cos = dot(normalize(R), normalize(V));
	cos = abs(cos);

	specularTerm = pow(cos, n);
	
    float ambientTerm = 0.2;

    float lightIntensity = diffuseTerm + ambientTerm + specularTerm;

	lightIntensity  = clamp(lightIntensity, 0, 1);

    // Compute final shaded color
	out_Color = vec4( (diffuseTerm + ambientTerm) * diffuseColor.rgb + specularTerm * u_LightColor, diffuseColor.a);
    //out_Color = vec4(u_LightColor*diffuseColor.rgb * lightIntensity, diffuseColor.a);
}
