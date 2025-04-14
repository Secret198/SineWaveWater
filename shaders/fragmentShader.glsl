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

vec3 calculateDiffuse()
{
    float diffuse = max(dot(normalize(Normal), normalize(lightSource.direction)), 0.0);
    return diffuse * lightSource.diffusion;
}

vec3 calculateSpecular()
{
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(lightSource.direction, Normal);
    float specular = pow(max(dot(reflectDir, viewDir), 0.0), 2);
    return lightSource.specular * specular;
}

void main()
{
    vec3 waterColor = vec3(0.09, 0.17, 0.4);
    fragColor = vec4((calculateDiffuse() + calculateSpecular()) * waterColor, 1.0);
};