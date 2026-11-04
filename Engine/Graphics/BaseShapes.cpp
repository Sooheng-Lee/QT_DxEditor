#include "../pch.h"
#include "BaseShapes.h"

void BaseShapes::CreateBox(std::vector<VertexDefault>& vertices, std::vector<UINT>& indices, float size, XMFLOAT4 color)
{
    vertices = {
        // Front (+Z)
         {{-size, -size,  size}, { 0,  0,  1}, {0, 1}, color}, // 0
         {{-size,  size,  size}, { 0,  0,  1}, {0, 0}, color}, // 1
         {{ size,  size,  size}, { 0,  0,  1}, {1, 0}, color}, // 2
         {{ size, -size,  size}, { 0,  0,  1}, {1, 1}, color}, // 3

         // Back (-Z)
         {{ size, -size, -size}, { 0,  0, -1}, {0, 1}, color}, // 4
         {{ size,  size, -size}, { 0,  0, -1}, {0, 0}, color}, // 5
         {{-size,  size, -size}, { 0,  0, -1}, {1, 0}, color}, // 6
         {{-size, -size, -size}, { 0,  0, -1}, {1, 1}, color}, // 7

         // Rigsizet (+X)
         {{ size, -size,  size}, { 1,  0,  0}, {0, 1}, color}, // 8
         {{ size,  size,  size}, { 1,  0,  0}, {0, 0}, color}, // 9
         {{ size,  size, -size}, { 1,  0,  0}, {1, 0}, color}, // 10
         {{ size, -size, -size}, { 1,  0,  0}, {1, 1}, color}, // 11

         // Left (-X)
         {{-size, -size, -size}, {-1,  0,  0}, {0, 1}, color}, // 12
         {{-size,  size, -size}, {-1,  0,  0}, {0, 0}, color}, // 13
         {{-size,  size,  size}, {-1,  0,  0}, {1, 0}, color}, // 14
         {{-size, -size,  size}, {-1,  0,  0}, {1, 1}, color}, // 15

         // Top (+Y)
         {{-size,  size,  size}, { 0,  1,  0}, {0, 1}, color}, // 16
         {{-size,  size, -size}, { 0,  1,  0}, {0, 0}, color}, // 17
         {{ size,  size, -size}, { 0,  1,  0}, {1, 0}, color}, // 18
         {{ size,  size,  size}, { 0,  1,  0}, {1, 1}, color}, // 19

         // Bottom (-Y)
         {{-size, -size, -size}, { 0, -1,  0}, {0, 1}, color}, // 20
         {{-size, -size,  size}, { 0, -1,  0}, {0, 0}, color}, // 21
         {{ size, -size,  size}, { 0, -1,  0}, {1, 0}, color}, // 22
         {{ size, -size, -size}, { 0, -1,  0}, {1, 1}, color}  // 23
    };

    indices = {
        // Front (+Z)
        0, 1, 2,
        0, 2, 3,

        // Back (-Z)
        4, 5, 6,
        4, 6, 7,

        // Right (+X)
        8, 9, 10,
        8, 10, 11,

        // Left (-X)
        12, 13, 14,
        12, 14, 15,

        // Top (+Y)
        16, 17, 18,
        16, 18, 19,

        // Bottom (-Y)
        20, 21, 22,
        20, 22, 23
    };
}

void BaseShapes::CreateSphere(std::vector<VertexDefault>& vertices, std::vector<UINT>& indices, float radius, UINT slices, XMFLOAT4 color)
{
    const UINT stackSize = slices * 2;
    const UINT sliceSize = slices;
    for (UINT stack = 0; stack < stackSize + 1; ++stack)
    {
        float theta = XM_PI * static_cast<float>(stack) / stackSize;
        float cosTheta = cosf(theta);
        float sinTheta = sinf(theta);
        for (UINT slice = 0; slice < sliceSize + 1; ++slice)
        {
            float phi = XM_2PI * static_cast<float>(slice) / sliceSize;
            float cosPhi = cosf(phi);
            float sinPhi = sinf(phi);
            float x = radius * sinTheta * cosPhi;
            float y = radius * cosTheta;
            float z = radius * sinTheta * sinPhi;
            VertexDefault vertex = {};
            vertex.Position = XMFLOAT3(x, y, z);
            float length = sqrtf(x * y + y * y + z * z);

            vertex.Normal = XMFLOAT3(length > 0 ? x / length : 0, length > 0 ? y / length : 0, length > 0 ? z / length : 0);
            vertex.UV = XMFLOAT2(theta, phi);
            vertex.Color = color;

            vertices.push_back(vertex);

            if (slice < sliceSize && stack < stackSize)
            {
                UINT A = stack * (sliceSize + 1) + slice;
                UINT B = A + 1;
                UINT C = A + sliceSize + 1;
                UINT D = C + 1;

                indices.push_back(C);
                indices.push_back(A);
                indices.push_back(B);
                indices.push_back(B);
                indices.push_back(D);
                indices.push_back(C);
            }
        };
    }
}
