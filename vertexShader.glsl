#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 normalVector;
layout(location = 2) in vec2 aTextureCoords;

const int numberOfWaves = 5;

out vec3 Normal;
out vec3 FragPos;
out vec2 TextureCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform float time;
uniform float deltaTime;
uniform float randomValues[numberOfWaves * 5];

float rand(vec2 co)
{
    return fract(sin(dot(co, vec2(12.9898, 78.233))) * 43758.5453);
}

float GetPhase(float waveLength, int i)
{
    return (deltaTime + randomValues[i + 4]) * (2.0 / waveLength);
};

float GetWave(vec2 position, int i)
{
    float amplitude = randomValues[i];
    float waveLength = randomValues[i + 1];
    vec2 directionVector = vec2(randomValues[i + 2], randomValues[i + 3]);
    float frequency = 2.0 / waveLength;
    float phase = GetPhase(waveLength, i);

    return amplitude * sin(dot(directionVector, position) * frequency + time * phase);
};

float AddedWaves(vec2 position)
{
    float waves = 0;
    for(int i = 0; i < numberOfWaves; i++)
    {
        waves += GetWave(position, i);
    }
    return waves;
};

void main()
{
    vec4 vertexPosition = model * vec4(aPos, 1.0);
    vertexPosition.y = AddedWaves(vec2(vertexPosition.x, vertexPosition.z));
    // vertexPosition.y = 2.0 * sin((dot(vec2(0.1, 0.5), vec2(vertexPosition.x, vertexPosition.z)) * (2.0 / 1.0) + time * ((deltaTime) * (2.0 / 1.0)))) + 0.5 * sin((dot(vec2(0.3, 0.5), vec2(vertexPosition.x, vertexPosition.z)) * (2.0 / 2.0) + time * ((deltaTime + 1) * (2.0 / 2.0))));
    // vertexPosition.y = rand(vec2(vertexPosition.x, vertexPosition.z)) * sin((dot(vec2(rand(vec2(vertexPosition.x, vertexPosition.z)), rand(vec2(vertexPosition.x, vertexPosition.z))), vec2(vertexPosition.x, vertexPosition.z)) * (2.0 / rand(vec2(vertexPosition.x, vertexPosition.z))) + time * ((deltaTime + rand(vec2(vertexPosition.x, vertexPosition.z))) * (2.0 / 1.0)))) + 0.5 * sin((dot(vec2(rand(vec2(vertexPosition.x, vertexPosition.z)), rand(vec2(vertexPosition.x, vertexPosition.z))), vec2(vertexPosition.x, vertexPosition.z)) * (2.0 / rand(vec2(vertexPosition.x, vertexPosition.z))) + time * ((deltaTime + rand(vec2(vertexPosition.x, vertexPosition.z))) * (2.0 / rand(vec2(vertexPosition.x, vertexPosition.z))))));
    gl_Position = projection * view * vertexPosition;
    FragPos = vec3(vertexPosition);
    Normal = mat3(transpose(inverse(model))) * normalVector;
    TextureCoords = aTextureCoords;
};