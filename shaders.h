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

    glm::vec3 baseColor = glm::vec3(0, 119, 182); // Color azul característico de Neptuno

    // Añadir patrones de rayas
    float stripePattern1 = glm::abs(glm::cos(fragment.original.y * 4.0f)) * 50.0f;

    glm::vec3 tmpColor = baseColor + glm::vec3(stripePattern1);

    // Agregar ruido para textura
    glm::vec2 uv = glm::vec2(fragment.original.x, fragment.original.y);

    FastNoiseLite noiseGenerator;
    noiseGenerator.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);

    float ox = 900.0f;
    float oy = 10.0f;
    float zoom = 300.0f;

    float noiseValue = noiseGenerator.GetNoise((uv.x + ox) * zoom, (uv.y + oy) * zoom);

    // Añadir ruido para mayor realismo
    tmpColor += glm::vec3(noiseValue * 20.0f, noiseValue * 20.0f, noiseValue * 10.0f);

    // Ajustar la intensidad para obtener un aspecto más suave
    tmpColor /= 255.0f;

    color = Color(static_cast<int>(tmpColor.x * 255), static_cast<int>(tmpColor.y * 255), static_cast<int>(tmpColor.z * 255));

    fragment.color = color * fragment.intensity;

    return fragment;
}

