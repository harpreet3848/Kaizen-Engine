#pragma once

#include <memory>

#include <glad/glad.h> 
#include <GLFW/glfw3.h>

template<typename T>
using Ref = std::shared_ptr<T>;

template<typename T>
constexpr GLint gl_enum_cast(T value) {
    return static_cast<GLint>(value);
}

