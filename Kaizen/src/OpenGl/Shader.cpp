#include "Shader.h"

#include <fstream>
#include <sstream>
#include <iostream>
#include <glm/gtc/type_ptr.hpp>

// ---------------------------------------------------------
// Constructor: read, compile, and link shaders (now with optional GS)
// ---------------------------------------------------------
Shader::Shader(const char* vertexPath, const char* fragmentPath, const char* geometryPath)
{
    // 1) Read source code from files
    std::string vertexCode, fragmentCode, geometryCode;
    std::ifstream vShaderFile, fShaderFile, gShaderFile;

    vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    gShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

    try {
        // Vertex & Fragment
        vShaderFile.open(vertexPath);
        fShaderFile.open(fragmentPath);
        std::stringstream vStream, fStream;
        vStream << vShaderFile.rdbuf();
        fStream << fShaderFile.rdbuf();
        vShaderFile.close();
        fShaderFile.close();
        vertexCode = vStream.str();
        fragmentCode = fStream.str();

        // Geometry (optional)
        if (geometryPath && geometryPath[0] != '\0') {
            gShaderFile.open(geometryPath);
            std::stringstream gStream;
            gStream << gShaderFile.rdbuf();
            gShaderFile.close();
            geometryCode = gStream.str();
        }
    }
    catch (std::ifstream::failure& e) {
        std::cerr << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ: " << e.what() << "\n";
    }

    const char* vCode = vertexCode.c_str();
    const char* fCode = fragmentCode.c_str();
    const char* gCode = geometryCode.empty() ? nullptr : geometryCode.c_str();

    // 2) Compile shaders
    GLuint vertex = 0, fragment = 0, geometry = 0;

    // Vertex
    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vCode, nullptr);
    glCompileShader(vertex);
    checkCompileErrors(vertex, "VERTEX");

    // Fragment
    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fCode, nullptr);
    glCompileShader(fragment);
    checkCompileErrors(fragment, "FRAGMENT");

    // Geometry (optional)
    if (gCode) {
        geometry = glCreateShader(GL_GEOMETRY_SHADER);
        glShaderSource(geometry, 1, &gCode, nullptr);
        glCompileShader(geometry);
        checkCompileErrors(geometry, "GEOMETRY");
    }

    // 3) Link program
    ID = glCreateProgram();
    glAttachShader(ID, vertex);
    glAttachShader(ID, fragment);
    if (geometry) glAttachShader(ID, geometry);
    glLinkProgram(ID);
    checkCompileErrors(ID, "PROGRAM");

    // 4) Cleanup shader objects
    glDeleteShader(vertex);
    glDeleteShader(fragment);
    if (geometry) glDeleteShader(geometry);
}

// ---------------------------------------------------------
// activate/use program
// ---------------------------------------------------------
void Shader::use() const
{
    glUseProgram(ID);
}

// ---------------------------------------------------------
// uniform utility methods
// ---------------------------------------------------------
void Shader::setBool(const std::string& name, bool value) const
{
    glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
}

void Shader::setInt(const std::string& name, int value) const
{
    glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
}

void Shader::setFloat(const std::string& name, float value) const
{
    glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
}

void Shader::setVec2(const std::string& name, const glm::vec2& value) const
{
    glUniform2fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
}

void Shader::setVec2(const std::string& name, float x, float y) const
{
    glUniform2f(glGetUniformLocation(ID, name.c_str()), x, y);
}

void Shader::setVec3(const std::string& name, const glm::vec3& value) const
{
    glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
}

void Shader::setVec3(const std::string& name, float x, float y, float z) const
{
    glUniform3f(glGetUniformLocation(ID, name.c_str()), x, y, z);
}

void Shader::setVec4(const std::string& name, const glm::vec4& value) const
{
    glUniform4fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
}

void Shader::setVec4(const std::string& name, float x, float y, float z, float w) const
{
    glUniform4f(glGetUniformLocation(ID, name.c_str()), x, y, z, w);
}

void Shader::setMat2(const std::string& name, const glm::mat2& mat) const
{
    glUniformMatrix2fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, glm::value_ptr(mat));
}

void Shader::setMat3(const std::string& name, const glm::mat3& mat) const
{
    glUniformMatrix3fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, glm::value_ptr(mat));
}

void Shader::setMat4(const std::string& name, const glm::mat4& mat) const
{
    glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, glm::value_ptr(mat));
}

// ---------------------------------------------------------
// error-checking helper
// ---------------------------------------------------------
void Shader::checkCompileErrors(GLuint shader, const std::string& type) const
{
    GLint  success;
    GLchar infoLog[1024];
    if (type != "PROGRAM") {
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(shader, 1024, nullptr, infoLog);
            std::cerr << "ERROR::SHADER_COMPILATION_ERROR (" << type << ")\n"
                << infoLog << "\n --------------------------------------------------- \n";
        }
    }
    else {
        glGetProgramiv(ID, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(ID, 1024, nullptr, infoLog);
            std::cerr << "ERROR::PROGRAM_LINKING_ERROR (" << type << ")\n"
                << infoLog << "\n --------------------------------------------------- \n";
        }
    }
}
