#version 460 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

// Instance data
layout (location = 3) in vec4 aModel1;
layout (location = 4) in vec4 aModel2;
layout (location = 5) in vec4 aModel3;
layout (location = 6) in vec4 aModel4;
layout (location = 7) in vec4 aColor;

uniform mat4 view;
uniform mat4 projection;
uniform int useInstanceColor;

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoords;
out vec4 Color;

void main() {
    mat4 instanceModel = mat4(aModel1, aModel2, aModel3, aModel4);

    vec4 worldPos = instanceModel * vec4(aPos, 1.0);
    FragPos = worldPos.xyz;

    Normal = mat3(transpose(inverse(instanceModel))) * aNormal;

    TexCoords = aTexCoords;
    Color = useInstanceColor > 0 ? aColor : vec4(1.0);

    gl_Position = projection * view * worldPos;
}