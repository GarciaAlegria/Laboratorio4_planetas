#pragma once
#include "color.h"
#include "uniform.h"
#include "fragment.h"
#include "FastNoiseLite.h"
#include <cmath>
#include <random>

Vertex vertexShader(const Vertex& vertex, const Uniform& uniforms) {

  glm::vec4 clipSpaceVertex = uniforms.projection * uniforms.view * uniforms.model * glm::vec4(vertex.position, 1.0f);

  glm::vec3 ndcVertex = glm::vec3(clipSpaceVertex) / clipSpaceVertex.w;

  glm::vec4 screenVertex = uniforms.viewport * glm::vec4(ndcVertex, 1.0f);

  glm::vec3 transformedNormal = glm::mat3(uniforms.model) * vertex.normal;
  transformedNormal = glm::normalize(transformedNormal);

  return Vertex{
    glm::vec3(screenVertex),
    transformedNormal,
    vertex.position
  };
};

float rand(glm::vec3 co) {
    return glm::fract(sin(glm::dot(co, glm::vec3(12.9898, 78.233, 54.53))) * 43758.5453);
}

Fragment fragmentShader(Fragment& fragment) {
    Color color;

    glm::vec3 groundColor = glm::vec3(0.44f, 0.51f, 0.33f);
    glm::vec3 oceanColor = glm::vec3(0.12f, 0.38f, 0.57f);
    glm::vec3 cloudColor = glm::vec3(1.0f, 1.0f, 1.0f);
    glm::vec3 brownColor = glm::vec3(0.5f, 0.25f, 0.0f); // Color café

    glm::vec2 uv = glm::vec2(fragment.original.x, fragment.original.y);

    FastNoiseLite noiseGenerator;
    noiseGenerator.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);

    float ox = 1200.0f;
    float oy = 3000.0f;
    float zoom = 200.0f;

    float noiseValue = noiseGenerator.GetNoise((uv.x + ox) * zoom, (uv.y + oy) * zoom);

    glm::vec3 tmpColor = (noiseValue < 0.5f) ? oceanColor : groundColor;

    // Agrega una capa de color café en algunas partes
    float oxb = 1200.0f;
    float oyb = 3000.0f;
    float zoomb = 200.0f;

    float noiseValueB = noiseGenerator.GetNoise((uv.x + oxb) * zoomb, (uv.y + oyb) * zoomb);

    float brownFactor = 0.2f; // Controla la intensidad del color café
    if (noiseValueB > 0.6f) {
        tmpColor = glm::mix(tmpColor, tmpColor + (brownColor * brownFactor), (noiseValueB - 0.6f) * 5.0f);
    }

    float oxc = 5500.0f;
    float oyc = 6900.0f;
    float zoomc = 400.0f;

    float noiseValueC = noiseGenerator.GetNoise((uv.x + oxc) * zoomc, (uv.y + oyc) * zoomc);

    if (noiseValueC > 0.5f) {
        tmpColor = glm::mix(tmpColor, cloudColor, (noiseValueC - 0.5f) * 2.0f);
    }

    // Añadir efecto de atenuación atmosférica para simular la atmósfera terrestre
    float atmosphereFactor = 0.2f;
    tmpColor = glm::mix(tmpColor, glm::vec3(0.0f, 0.0f, 0.0f), fragment.intensity * atmosphereFactor);

    color = Color(tmpColor.x, tmpColor.y, tmpColor.z);

    fragment.color = color;

    return fragment;
}


Fragment moonfragmentShader(Fragment& fragment) {
    Color color;

    glm::vec3 baseColor = glm::vec3(0.8, 0.8, 0.8); // Color gris claro para la Luna
    glm::vec3 ambientColor = glm::vec3(0.2, 0.2, 0.2); // Color de luz ambiente más brillante
    glm::vec3 lightDirection = glm::normalize(glm::vec3(1.0, 1.0, 1.0)); // Dirección de la luz
    glm::vec2 uv = glm::vec2(fragment.original.x, fragment.original.y);

    // Calcula la intensidad de la luz difusa y aumenta su intensidad
    float diffuseIntensity = glm::max(0.0f, glm::dot(fragment.original, lightDirection));
    diffuseIntensity = glm::clamp(diffuseIntensity * 1.5f, 0.0f, 1.0f); // Aumenta la intensidad de la luz difusa

    FastNoiseLite noiseGenerator;
    noiseGenerator.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);

    float ox = 1200.0f;
    float oy = 3000.0f;
    float zoom = 200.0f;

    float noiseValue = noiseGenerator.GetNoise((uv.x + ox) * zoom, (uv.y + oy) * zoom);

    // Ajusta la mezcla de blanco y gris en función de noiseValue
    glm::vec3 craterColor = glm::mix(glm::vec3(1.0, 1.0, 1.0), glm::vec3(0.8, 0.8, 0.8), noiseValue);

    // Mezcla el color base con el color de luz ambiente y la luz difusa
    glm::vec3 tmpColor = craterColor * (ambientColor + diffuseIntensity) * 1.0f;

    color = Color(static_cast<int>(tmpColor.x * 255), static_cast<int>(tmpColor.y * 255), static_cast<int>(tmpColor.z * 255));

    fragment.color = color;

    return fragment;
}


