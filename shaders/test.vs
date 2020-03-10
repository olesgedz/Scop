#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
out vec3 Normal;
uniform vec3 lightPos; // we now define the uniform in the vertex shader and pass the 'view space' lightpos to the fragment shader. lightPos is currently in world space.
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
void main()
{
   Normal = mat3(transpose(inverse(view * model))) * aNormal;
   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);
}