#pragma once
#include "Object.h"
class Cursor :
    public Object
{
public:

    Cursor(Shader& sh);
    Cursor() = default;
    void SetCursorPosition(glm::vec3 position);
    void DrawObject(glm::mat4 mvp) override;

private:
    std::vector<float> points;
    std::vector<unsigned int> lines;
};

