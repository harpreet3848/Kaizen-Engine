#pragma once

#include "VertexArray.h"

class ShapeGenerator
{
public:
    static Ref<VertexArray> GenerateSquare();
    static Ref<VertexArray> GenerateQuad();
    static Ref<VertexArray> GenerateCube();
};