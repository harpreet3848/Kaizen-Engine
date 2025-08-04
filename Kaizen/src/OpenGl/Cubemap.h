#pragma once

#include <vector>
#include <string>

class Cubemap
{
public:
    // Constructor takes a vector of 6 texture file paths
    Cubemap(const std::vector<std::string>& faces, bool gamma = false);
    ~Cubemap();

    void BindTexture(uint32_t slot = 0) const;
    void Unbind() const;

private:
    uint32_t m_RendererID;
};