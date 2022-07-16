#include "Perlin.h"

namespace EngineCore {

    unsigned int table[1024];

    float interpolation(float a, float b, float t) {
        return a + (b - a) * t;
    }

    float qunticCurve(float t)
    {
        return t * t * t * (t * (t * 6 - 15) + 10);
    }

    // ”становка сида дл€ функций шумов
    void perlinNoiseSeed(unsigned int _seed) {
        srand(_seed);
        for (size_t i = 0; i < 1024; i++)
            table[i] = rand() & 3;
    }

    // ѕолучение псевдослучайного вектора
    glm::vec2 GetPseudoRandomGradientVector(int x, int y) {
        int v = (int)(((x * 1836311903) ^ (y * 2971215073) + 4807526976) & 1023);
        v = table[v];

        switch (v)
        {
        case 0:  return { 1,  0 };
        case 1:  return { -1,  0 };
        case 2:  return { 0,  1 };
        default: return { 0, -1 };
        }
    }

    // Ўум ѕерлина [-0.56;0.56]
    float perlinNoise(float fx, float fy) {
        // находим координаты левой верхней вершины квадрата
        int left = (int)std::floor(fx);
        int top = (int)std::floor(fy);

        // а теперь локальные координаты точки внутри квадрата
        float pointInQuadX = fx - left;
        float pointInQuadY = fy - top;

        // извлекаем градиентные векторы дл€ всех вершин квадрата:
        glm::vec2 topLeftGradient = GetPseudoRandomGradientVector(left, top);
        glm::vec2 topRightGradient = GetPseudoRandomGradientVector(left + 1, top);
        glm::vec2 bottomLeftGradient = GetPseudoRandomGradientVector(left, top + 1);
        glm::vec2 bottomRightGradient = GetPseudoRandomGradientVector(left + 1, top + 1);

        // вектора от вершин квадрата до точки внутри квадрата:
        glm::vec2 distanceToTopLeft = { pointInQuadX, pointInQuadY };
        glm::vec2 distanceToTopRight = { pointInQuadX - 1, pointInQuadY };
        glm::vec2 distanceToBottomLeft = { pointInQuadX, pointInQuadY - 1 };
        glm::vec2 distanceToBottomRight = { pointInQuadX - 1, pointInQuadY - 1 };

        // считаем скал€рные произведени€ между которыми будем интерполировать

        float tx1 = glm::dot(distanceToTopLeft, topLeftGradient);
        float tx2 = glm::dot(distanceToTopRight, topRightGradient);
        float bx1 = glm::dot(distanceToBottomLeft, bottomLeftGradient);
        float bx2 = glm::dot(distanceToBottomRight, bottomRightGradient);

        // готовим параметры интерпол€ции, чтобы она не была линейной:
        pointInQuadX = qunticCurve(pointInQuadX);
        pointInQuadY = qunticCurve(pointInQuadY);

        // собственно, интерпол€ци€:
        float tx = interpolation(tx1, tx2, pointInQuadX);
        float bx = interpolation(bx1, bx2, pointInQuadX);
        float tb = interpolation(tx, bx, pointInQuadY);

        // возвращаем результат:
        return tb;
    }

    // ћультипликативний шум перлина [-0.56;0.56]
    float multiPerlinNoise(float fx, float fy, int octaves, float persistence) {
        float amplitude = 1;
        float max = 0;
        float result = 0;

        while (octaves-- > 0)
        {
            max += amplitude;
            result += perlinNoise(fx, fy) * amplitude;
            amplitude *= persistence;
            fx *= 2;
            fy *= 2;
        }

        return result / max;
    }
}