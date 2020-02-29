// Fragment shader:
// ================
#version 400 core
struct Material {
vec3 ambient;
vec3 diffuse;
vec3 specular;
float shininess;
};
uniform Material material;


out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec3 LightPos;   // extra in variable, since we need the light position in view space we calculate this in the vertex shader
in vec2 TextureCoord;
uniform vec3 lightColor;
uniform vec3 objectColor;
uniform sampler2D texture1;
uniform int textureExist;
void main()
{
	
	// ambient
	float ambientStrength = 0.1;
	vec3 ambient = ambientStrength * lightColor;    
	
	 // diffuse 
	vec3 norm = normalize(Normal);
	vec3 lightDir = normalize(LightPos - FragPos);
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = diff * vec3(1,1,1);
	
	// specular
	float specularStrength = 0.5;
	vec3 viewDir = normalize(-FragPos); // the viewer is always at (0,0,0) in view-space, so viewDir is (0,0,0) - Position => -Position
	vec3 reflectDir = reflect(-lightDir, norm);  
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
	vec3 specular = specularStrength * spec * lightColor; 
	vec3 result;
	if (textureExist == 1)
		result = (diffuse + specular + ambient) * vec3(texture(texture1, TextureCoord));// * objectColor;
	else
		result = (diffuse + specular + ambient) * objectColor;
	FragColor = vec4(result, 1.0);
}