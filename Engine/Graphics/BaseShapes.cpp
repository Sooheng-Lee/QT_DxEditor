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

         // Right (+X)
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

    // Match the outward normals and clockwise front faces (FrontCounterClockwise = false).
    indices = {
        // Front (+Z)
        0, 2, 1,
        0, 3, 2,

        // Back (-Z)
        4, 6, 5,
        4, 7, 6,

        // Right (+X)
        8, 10, 9,
        8, 11, 10,

        // Left (-X)
        12, 14, 13,
        12, 15, 14,

        // Top (+Y)
        16, 18, 17,
        16, 19, 18,

        // Bottom (-Y)
        20, 22, 21,
        20, 23, 22
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
            float length = sqrtf(x * x + y * y + z * z);

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

void BaseShapes::CreateCylinder(std::vector<VertexDefault>& vertices, std::vector<UINT>& indices, float radius, float height, UINT sliceCount, XMFLOAT4 color)
{
    if (radius <= 0.0f || height <= 0.0f || sliceCount < 3) return;

    constexpr float PI = XM_PI;
    const float halfHeight = height * 0.5f;
    for (UINT y = 0; y < 2; ++y)
    {
        double bt = halfHeight * pow(-1, y+1);
        for (UINT x = 0; x < sliceCount + 1; ++x)
        {
            float slice = static_cast<float>(x) / sliceCount;
            float theta = slice * XM_2PI;

            float cosTheta = cosf(theta);
            float sinTheta = sinf(theta);

            VertexDefault vertex = {};
            vertex.Position = XMFLOAT3(radius * cosTheta, bt, radius * sinTheta);
            vertex.Normal = XMFLOAT3(cosTheta, 0.0f, sinTheta);
            vertex.UV = XMFLOAT2(slice, 1.0f - y);
            vertex.Color = color;
            vertices.push_back(vertex);
        }
    }

    const UINT ringSize = sliceCount + 1;
    for (UINT idx = 0; idx < sliceCount; ++idx)
    {
        UINT bt0 = idx;
        UINT bt1 = idx + 1;
        UINT top0 = ringSize + bt0;
        UINT top1 = ringSize + bt1;

        indices.push_back(bt0);
        indices.push_back(top0);
        indices.push_back(bt1);
        indices.push_back(bt1);
        indices.push_back(top0);
        indices.push_back(top1);
    }

    for (UINT cap = 0; cap < 2; ++cap)
    {
        float normalY = halfHeight * pow(-1, cap);
        float y = halfHeight * normalY;

        UINT centerIdx = static_cast<UINT>(vertices.size());
        VertexDefault center = {};
        center.Position = XMFLOAT3(0, y, 0);
        center.Normal = XMFLOAT3(0, normalY, 0);
        center.UV = XMFLOAT2(0.5f, 0.5f);
        center.Color = color;

        UINT startIdx = static_cast<UINT>(vertices.size());

        for (UINT i = 0; i <= sliceCount; ++i)
        {
            float theta = XM_2PI * i / sliceCount;
            float x = radius * cosf(theta);
            float z = radius * sinf(theta);

            VertexDefault Vertex = {};
            Vertex.Position = XMFLOAT3(x, y, z);
            Vertex.Normal = XMFLOAT3(0, normalY, 0);
            Vertex.UV = XMFLOAT2(
                0.5f + x / (2.0f * radius),
                0.5f - z / (2.0f * radius)
            );
            Vertex.Color = XMFLOAT4(1, 1, 1, 1);

            vertices.push_back(Vertex);
        }

        for (UINT idx = 0; idx < sliceCount; ++idx)
        {
            if (y > 0)
            {
                indices.push_back(centerIdx);
                indices.push_back(startIdx + idx + 1);
                indices.push_back(startIdx + idx);
            }
            else
            {
                indices.push_back(centerIdx);
                indices.push_back(startIdx + idx);
                indices.push_back(startIdx + idx + 1);
            }
        }
    }
}
