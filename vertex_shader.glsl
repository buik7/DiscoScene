#version 330 core
layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec3 inNormal;
layout (location = 2) in vec2 inTexCoord;

out vec3 FragPos; // output fragment position to fragment shader
out vec3 Normal; // output normal vector to fragment shader
out vec2 TexCoord; // output texture coordinate vector to fragment shader

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    gl_Position = projection * view * model * vec4(inPosition, 1.0f);
    FragPos = inPosition;
    Normal = inNormal;
    TexCoord = inTexCoord;
}