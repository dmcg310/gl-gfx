#version 330 core

out vec4 FragColor;

uniform vec3 color;
uniform sampler2D mainTexture;
uniform bool useTexture;

in vec2 TexCoords;

void main() {
    if (useTexture) {
        FragColor = texture(mainTexture, TexCoords) * vec4(color, 1.0);
    } else {
        FragColor = vec4(color, 1.0);
    }
}
