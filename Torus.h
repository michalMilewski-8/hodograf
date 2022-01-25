#pragma once
#include "Object.h"



class Torus :
    public Object
{
public:
    Torus(float R, float r, int vertical, int horizontal,glm::vec4 color, Shader sh);

    void DrawObject(glm::mat4 mvp) override;
    void CreateMenu() override;

    float GetR() const { return R; }
    float Getr() const { return r; }
    float GetVertical() const { return vertical_points_number; }
    float GetHorizontal() const { return horizontal_points_number; }
    glm::vec4 GetColor() const { return color; }

    std::vector<std::function<glm::vec3(double, double)>> GetParametrisations() override;
    std::vector<std::function<glm::vec3(double, double)>> GetUParametrisations() override;
    std::vector<std::function<glm::vec3(double, double)>> GetVParametrisations() override;

    void SetR(float _R);
    void Setr(float _r);
    void SetVertical(int _v);
    void SetHorizontal(float _h);
    void SetColor(glm::vec4 _c);

    static unsigned int counter;

private:
    void create_torus_points();
    glm::vec3 torus_point(float alfa_r, float beta_r);
    void update_object() override;

    float R;
    float r;
    int vertical_points_number;
    int horizontal_points_number;

    std::vector<float> points;
    std::vector<unsigned int> triangles;
};

