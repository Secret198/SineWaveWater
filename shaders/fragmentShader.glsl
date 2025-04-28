#version 330 core

struct DirLight
{
    vec3 direction;

    vec3 diffusion;
    vec3 specular;
};

in vec3 Normal;
in vec3 FragPos;
in vec2 TextureCoords;

out vec4 fragColor;

uniform DirLight lightSource;
uniform vec3 viewPos;
uniform samplerCube skyBox;
uniform vec3 waterColor;

vec3 calculateDiffuse()
{
    float diffuse = max(dot(normalize(Normal), normalize(lightSource.direction)), 0.0);
    return diffuse * lightSource.diffusion;
}

vec3 calculateSpecular(vec3 viewDir)
{
    vec3 halfway = normalize(viewDir + lightSource.direction);
    float specular = pow(max(dot(halfway, Normal), 0.0), 2);
    return lightSource.specular * specular;
}

void main()
{
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(viewDir, Normal);
    vec3 lightingData = calculateDiffuse() + calculateSpecular(viewDir);
    fragColor = texture(skyBox, reflectDir) * vec4(waterColor * lightingData, 1.0);
};