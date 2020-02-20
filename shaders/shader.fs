#version 330 core

out vec4 FragColor;
in  vec3 Normal;
in  vec3 FragPos;
uniform vec3 objectColor;
uniform vec3 lightColor;
uniform vec3 lightPos;

void main()
{
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * lightColor;
    vec3 dir = normalize(lightPos - FragPos);
    float cosine = dot(dir, normalize(Normal));

    vec3 result = ambient * objectColor + lightColor * cosine;
    FragColor = vec4(result, 1.0);
}