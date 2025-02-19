#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 normalVector;
layout(location = 2) in vec2 aTextureCoords;

out vec3 Normal;
out vec3 FragPos;
out vec2 TextureCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform float time;
uniform float deltaTime;

int numberOfWaves = 10;

float GetWaveDirection(float x, float z, vec2 direction)
{
    return x * direction.x + z * direction.y;
};

float GetFrequency(float waveLength)
{
    return 2 / waveLength;
};

float GetPhase(float waveLength)
{
    return deltaTime * (2 / waveLength);
};

float GetWave(float x, float z, float t)
{

};

float AddedWaves(float x, float z, float t)
{
    float waves = 0;
    for(int i = 0; i < numberOfWaves; i++)
    {
        waves += GetWave(x, z, t);
    }
    return waves;
};

void main()
{
    vec4 vertexPosition = model * vec4(aPos, 1.0);
    vertexPosition.y = AddedWaves(vertexPosition.x, vertexPosition.z, time);
    gl_Position = projection * view * vertexPosition;
    FragPos = vec3(vertexPosition);
    Normal = mat3(transpose(inverse(model))) * normalVector;
    TextureCoords = aTextureCoords;
};