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

    glm::vec3 baseColor = glm::vec3(232, 174, 104);

    float stripePattern = glm::abs(glm::cos(fragment.original.y * 15.0f)) * 70.0f;
    float stripePattern2 = glm::abs(glm::sin(fragment.original.y * 25.0f)) * 70.0f;
    

    glm::vec3 tmpColor = baseColor + stripePattern + stripePattern2;

    
    float noise = rand(fragment.original);
    tmpColor += noise * 0.1f;

    glm::vec3 gray = glm::vec3(255, 255, 255);

    glm::vec2 uv = glm::vec2(fragment.original.x, fragment.original.y);

    FastNoiseLite noiseGenerator;
    noiseGenerator.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);

    float ox = 900.0f;
    float oy = 10.0f;
    float zoom = 300.0f;

    float noiseValue = noiseGenerator.GetNoise((uv.x + ox) * zoom, (uv.y + oy) * zoom);

    // Define el centro y el radio de la Gran Mancha Roja
    glm::vec2 redSpotCenter = (glm::vec2(-0.50f, -0.4f));
    float redSpotRadius= 0.1f;

    // Comprueba si el fragmento está dentro de la Gran Mancha Roja
    if (glm::distance(uv, redSpotCenter) < redSpotRadius){
        tmpColor = glm::vec3(231, 173, 103);  // Asigna color rojo
    }
    else if (noiseValue > 0.9f) {
        tmpColor = gray;
    }

    color = Color(static_cast<int>(tmpColor.x), static_cast<int>(tmpColor.y), static_cast<int>(tmpColor.z));

    fragment.color = color * fragment.intensity;

    return fragment;
};
