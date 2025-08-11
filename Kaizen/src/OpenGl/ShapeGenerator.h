#pragma once

#include "VertexArray.h"

class ShapeGenerator
{
public:
    static Ref<VertexArray> GenerateSquare();
    static Ref<VertexArray> GenerateQuad(float offsetX, float offsetY, float sizeX, float sizeY);
    static Ref<VertexArray> GenerateCube();
};