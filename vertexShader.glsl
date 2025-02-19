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

float rand(vec2 co)
{
    return fract(sin(dot(co, vec2(12.9898, 78.233))) * 43758.5453);
}

float GetFrequency(float waveLength)
{
    return 2 / waveLength;
};

float GetPhase(float waveLength)
{
    return (deltaTime + rand(vec2(waveLength, 1.0))) * (2 / waveLength);
};

float GetWave(vec2 position)
{
    float amplitude = rand(position);
    float waveLength = rand(position);
    vec2 directionVector = vec2(rand(position), rand(position));
    float frequency = GetFrequency(waveLength);
    float phase = GetPhase(waveLength);

    return amplitude * sin(dot(directionVector, position) * frequency + time * phase);
};

float AddedWaves(vec2 position)
{
    float waves = 0;
    for(int i = 0; i < numberOfWaves; i++)
    {
        waves += GetWave(position);
    }
    return waves;
};

void main()
{
    vec4 vertexPosition = model * vec4(aPos, 1.0);
    vertexPosition.y = AddedWaves(vec2(vertexPosition.x, vertexPosition.z));
    // vertexPosition.y = 2.0 * sin((dot(vec2(0.1, 0.5), vec2(vertexPosition.x, vertexPosition.z)) * (2.0 / 1.0) + time * ((deltaTime) * (2.0 / 1.0)))) + 0.5 * sin((dot(vec2(0.3, 0.5), vec2(vertexPosition.x, vertexPosition.z)) * (2.0 / 2.0) + time * ((deltaTime + 1) * (2.0 / 2.0))));
    gl_Position = projection * view * vertexPosition;
    FragPos = vec3(vertexPosition);
    Normal = mat3(transpose(inverse(model))) * normalVector;
    TextureCoords = aTextureCoords;
};