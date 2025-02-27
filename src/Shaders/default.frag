#version 460 core

in vec3 Normal;
in vec2 TexCoords;
in vec4 Color;

uniform sampler2D mainTexture;
uniform vec3 color;
uniform int useTexture;
uniform int useInstanceColor;

out vec4 FragColor;

void main() {
    vec4 texColor = texture(mainTexture, TexCoords);
    vec4 baseColor = useTexture > 0 ? texColor : vec4(color, 1.0);

    if (useInstanceColor > 0) {
        baseColor *= Color;
    }

    FragColor = baseColor;
}