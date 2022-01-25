#pragma once
#include "Object.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <math.h>
#define _USE_MATH_DEFINES
#include <cmath>
#include "implot.h"

namespace ImPlot {
	struct ScrollingBuffer {
		int MaxSize;
		int Offset;
		ImVector<ImVec2> Data;
		ScrollingBuffer(int max_size = 2000) {
			MaxSize = max_size;
			Offset = 0;
			Data.reserve(MaxSize);
		}
		void AddPoint(float x, float y) {
			if (Data.size() < MaxSize)
				Data.push_back(ImVec2(x, y));
			else {
				Data[Offset] = ImVec2(x, y);
				Offset = (Offset + 1) % MaxSize;
			}
		}
		void Erase() {
			if (Data.size() > 0) {
				Data.shrink(0);
				Offset = 0;
			}
		}
	};
}

class Block :
	public Object
{
public:
	static int counter;

	Block(glm::vec2 lup, Shader sh);

	void DrawObject(glm::mat4 mvp) override;

	void Update() override;
	void SetCenter(glm::vec2 lr) { center = lr; Update(); }
	void CreateMenu();

	std::vector<glm::vec2> corners;

private:
	void create_block_points();
	void update_object() override;

	glm::vec2 center = { 0,0 };

	bool blocks_need_creation = true;

	std::vector<float> points;
	std::vector<unsigned int> quads;
};

class Line :
	public Object
{
public:
	Line(Shader sh);

	void DrawObject(glm::mat4 mvp) override;

	void Update() override { need_update = true; };
	void SetStEnd(glm::vec2 start_, glm::vec2 end_) { start = start_; end = end_; Update(); };

private:
	void create_block_points();
	void update_object() override;

	glm::vec2 start;
	glm::vec2 end;

	std::vector<float> points;
	std::vector<unsigned int> quads;
};



class Arm :
	public Object
{
public:
	Arm(Shader sh);

	void DrawObject(glm::mat4 mvp) override;

	void Update() override { need_update = true; };
	void Menu();
	void SetT(float T) { current_angle += w * T; dt = T; t += dt; Update(); }
	void Reset() { current_angle = 0.0f; _x.Erase(); _dx.Erase(); _ddx.Erase(); _state.Erase(); t = 0.0f; Update(); }

private:
	void create_block_points();
	void update_object() override;

	double dt;
	double t = 0.0f;
	float r;
	float w;
	float L;
	double x;
	double dx;
	double last_x;
	double last_dx;

	float current_angle = 0.0f;

	bool blocks_need_creation = true;

	std::unique_ptr<Line> line;
	std::unique_ptr<Block> block;

	std::vector<float> points;
	std::vector<unsigned int> quads;

	ImPlot::ScrollingBuffer _x = {20000};
	ImPlot::ScrollingBuffer _dx = { 20000 };
	ImPlot::ScrollingBuffer _ddx = { 20000 };
	ImPlot::ScrollingBuffer _state = { 20000 };
};
