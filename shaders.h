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

float rand(glm:: vec3 co) {
    return glm::fract(sin(glm::dot(co, glm::vec3(12.9898, 78.233, 54.53))) * 43758.5453);
}

Fragment fragmentShader(Fragment& fragment) {
     Color color;

    glm::vec3 baseColor = glm::vec3(0.3f, 0.5f, 0.8f); // Color base del planeta
    glm::vec2 uv = glm::vec2(fragment.original.x, fragment.original.y);

    FastNoiseLite noiseGenerator;
    noiseGenerator.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);

    // Capa de textura rojiza para áreas elevadas
    float ox1 = 1200.0f;
    float oy1 = 3000.0f;
    float zoom1 = 200.0f;
    float noiseValue1 = noiseGenerator.GetNoise((uv.x + ox1) * zoom1, (uv.y + oy1) * zoom1);
    glm::vec3 redLayer = glm::vec3(1.0f, 0.3f, 0.1f) * noiseValue1;

    // Capa de textura amarilla para áreas con vegetación
    float ox2 = 800.0f;
    float oy2 = 500.0f;
    float zoom2 = 150.0f;
    float noiseValue2 = noiseGenerator.GetNoise((uv.x + ox2) * zoom2, (uv.y + oy2) * zoom2);
    glm::vec3 yellowLayer = glm::vec3(1.0f, 1.0f, 0.0f) * noiseValue2;

    // Capa de textura verde para áreas con agua
    float ox3 = 2000.0f;
    float oy3 = 1000.0f;
    float zoom3 = 100.0f;
    float noiseValue3 = noiseGenerator.GetNoise((uv.x + ox3) * zoom3, (uv.y + oy3) * zoom3);
    glm::vec3 greenLayer = glm::vec3(0.0f, 0.8f, 0.2f) * noiseValue3;

    // Capa de textura blanca para áreas de nieve o hielo
    float ox4 = 3000.0f;
    float oy4 = 2500.0f;
    float zoom4 = 120.0f;
    float noiseValue4 = noiseGenerator.GetNoise((uv.x + ox4) * zoom4, (uv.y + oy4) * zoom4);
    glm::vec3 whiteLayer = glm::vec3(0.5f, 0.5f, 0.5f) * noiseValue4;

    // Añade todas las capas de textura y variación de color
    glm::vec3 finalColor = baseColor + redLayer + yellowLayer + greenLayer + whiteLayer;

    // Añade un efecto de atmósfera
    float atmosphereFactor = 0.3f; // Controla la intensidad de la atmósfera
    finalColor = glm::mix(finalColor, glm::vec3(0.1f, 0.1f, 0.1f), fragment.intensity * atmosphereFactor);

    color = Color(static_cast<int>(finalColor.x * 255), static_cast<int>(finalColor.y * 255), static_cast<int>(finalColor.z * 255));

    fragment.color = color;

    return fragment;
}