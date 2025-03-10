#version 330 core

struct DirLight
{
    vec3 direction;

    vec3 ambient;
    vec3 diffusion;
    vec3 specular;
};

in vec3 Normal;
in vec3 FragPos;
in vec2 TextureCoords;

out vec4 fragColor;

void main()
{
    fragColor = vec4(0.09, 0.17, 0.4, 1.0);
};