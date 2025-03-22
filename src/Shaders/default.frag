#version 410 core

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;
in vec4 Color;

uniform sampler2D mainTexture;
uniform vec3 color;
uniform int useTexture;
uniform int useInstanceColor;
uniform int isEmissive;

uniform vec3 viewPos;
uniform vec3 ambientColor;
uniform float ambientStrength;
uniform float diffuseStrength;
uniform float specularStrength;
uniform float shininess;

struct Light {
    int type;// 0 = directional, 1 = point
    vec3 position;
    vec3 direction;
    vec3 color;
    float intensity;
};

#define MAX_LIGHTS 8
uniform Light lights[MAX_LIGHTS];
uniform int numLights;

out vec4 FragColor;

vec3 CalculateLight(Light light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 baseColor) {
    vec3 lightDir;
    float attenuation = 1.0;

    if (light.type == 0) {
        lightDir = normalize(-light.direction);
    } else {
        lightDir = normalize(light.position - fragPos);
        float distance = length(light.position - fragPos);
        attenuation = 1.0 / (1.0 + 0.09 * distance + 0.032 * (distance * distance));
    }

    vec3 ambient = ambientStrength * light.color;

    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = diffuseStrength * diff * light.color;

    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
    vec3 specular = specularStrength * spec * light.color;

    return (ambient + diffuse + specular) * light.intensity * attenuation * baseColor;
}

void main() {
    vec4 texColor = texture(mainTexture, TexCoords);
    vec4 baseColor = useTexture > 0 ? texColor : vec4(color, 1.0);

    if (useInstanceColor > 0) {
        baseColor *= Color;
    }

    if (isEmissive > 0) {
        FragColor = baseColor;
        return;
    }

    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 result = ambientStrength * ambientColor * baseColor.rgb;

    for (int i = 0; i < numLights; i++) {
        if (i >= MAX_LIGHTS) break;
        result += CalculateLight(lights[i], norm, FragPos, viewDir, baseColor.rgb);
    }

    result = min(result, vec3(1.0));

    FragColor = vec4(result, baseColor.a);
}
