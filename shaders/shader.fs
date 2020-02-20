#version 330 core

out vec4 FragColor;
in  vec3 Normal;
in  vec3 FragPos;
uniform vec3 objectColor;
uniform vec3 lightColor;
uniform vec3 lightPos;
uniform vec3 viewPos;

void main()
{
    float specularStrength = 0.5;
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * lightColor;
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(normalize(Normal), lightDir), 0.0);
    vec3 diffuse = diff * lightColor;
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, normalize(Normal));
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * spec * lightColor;
    vec3 result = (ambient + diffuse + specular) * objectColor;
    FragColor = vec4(result, 1.0);
}