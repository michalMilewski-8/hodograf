#include "Block.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <fstream>
#include <queue>

int Block::counter = 1;

void Block::Update()
{
	need_update = true;
}

void Block::CreateMenu()
{
	if (ImGui::TreeNode(constname.c_str()))
	{
		glm::vec2 movement = { 0,0 };
		if (ImGui::SliderFloat2("Move", &(movement.x), -0.01f, 0.01f)) {
			Update();
		}
		ImGui::TreePop();
		ImGui::Separator();
	}
}

void Block::create_block_points()
{
	corners.clear();
	corners.push_back({ -0.0,-0.05 });
	corners.push_back({ -0.0,0.05 });
	corners.push_back({ 0.1,0.05 });
	corners.push_back({ 0.1,-0.05 });

	for (auto& corner : corners) {
		points.push_back(corner.x);
		points.push_back(corner.y);
		points.push_back(0.0f);

		points.push_back(color.x);
		points.push_back(color.y);
		points.push_back(color.z);
		points.push_back(color.w);
	}

	quads.push_back(0);
	quads.push_back(1);
	quads.push_back(1);
	quads.push_back(2);
	quads.push_back(2);
	quads.push_back(3);
	quads.push_back(3);
	quads.push_back(0);

	blocks_need_creation = false;
}

void Block::update_object()
{
	if (blocks_need_creation) {
		quads.clear();
		points.clear();

		create_block_points();

		shader.use();
		// 1. bind Vertex Array Object
		glBindVertexArray(VAO);
		// 2. copy our vertices array in a vertex buffer for OpenGL to use
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * points.size(), points.data(), GL_DYNAMIC_DRAW);
		// 3. copy our index array in a element buffer for OpenGL to use
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * quads.size(), quads.data(), GL_DYNAMIC_DRAW);
		// 4. then set the vertex attributes pointers
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, description_number * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, description_number * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);
	}

	MoveObjectTo({ center,0 });
}

Block::Block(glm::vec2 lup, Shader sh) :
	Object(sh, 7)
{
	center = { 0,0 };
	color = { 0,1.0f ,0 ,1.0f };
	update_object();
}

void Block::DrawObject(glm::mat4 mvp)
{
	if (need_update) {
		update_object();
		need_update = false;
	}

	Object::DrawObject(mvp);

	glDrawElements(GL_LINES, quads.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

Arm::Arm(Shader sh) :
	Object(sh, 7)
{
	r = 0.2f;
	L = 0.5f;
	w = 1.0f;
	dt = 0.1f;
	x = 0.0f;
	dx = 0.0f;
	last_x = 0.0f;
	last_dx = 0.0f;
	color = { 1, 0.05f, 0, 1.0f };
	line = std::make_unique<Line>(sh);
	block = std::make_unique<Block>(glm::vec2{ 0,0 }, sh);
	d = std::normal_distribution<double>(0, 1);
	update_object();
}

void Arm::DrawObject(glm::mat4 mvp)
{
	if (need_update) {
		update_object();
		need_update = false;
	}

	Object::DrawObject(mvp);

	glDrawElements(GL_LINES, quads.size(), GL_UNSIGNED_INT, 0);

	glBindVertexArray(0);

	block->DrawObject(mvp);

	line->DrawObject(mvp);
}

void Arm::Menu()
{
	if (ImGui::InputFloat("r", &r)) {
		need_update = true;
		blocks_need_creation = true;
		if (r <= 0.0f) {
			r = 0.001f;
		}
		if (L < r) {
			L = r;
		}
	}
	if (ImGui::InputDouble("L", &L)) {
		need_update = true;
		if (L < r) {
			L = r;
		}
	}
	if (ImGui::InputFloat("w", &w)) {
		need_update = true;
	}

	ImGui::SliderFloat("epsilon",&epsilon,0.0000001f,0.001f,"%.7f");
	ImGui::Checkbox("enable error", &add_d);


	if (ImPlot::BeginPlot("Wykres 1")) {
		static float history = 10.0f;
		static ImPlotAxisFlags flags = ImPlotAxisFlags_NoTickLabels;

		ImPlot::SetupAxes(NULL, NULL, flags, flags);
		ImPlot::SetupAxisLimits(ImAxis_X1, _x.Data[_x.Offset == 0? _x.Data.size() < _x.MaxSize ? _x.Data.size() - 1 : _x.MaxSize-1 : _x.Offset-1].x - history, _x.Data[_x.Offset == 0 ? _x.Data.size() < _x.MaxSize ? _x.Data.size()-1 : _x.MaxSize - 1 : _x.Offset - 1].x, ImGuiCond_Always);
		ImPlot::SetupAxisLimits(ImAxis_Y1, -1.2, 1.2);
		ImPlot::SetNextFillStyle(IMPLOT_AUTO_COL, 0.5f);
		ImPlot::PlotLine("x(t)", &_x.Data[0].x, &_x.Data[0].y, _x.Data.size(), _x.Offset, 2 * sizeof(float));
		ImPlot::PlotLine("x'(t)", &_dx.Data[0].x, &_dx.Data[0].y, _dx.Data.size(), _dx.Offset, 2 * sizeof(float));
		ImPlot::PlotLine("x''(t)", &_ddx.Data[0].x, &_ddx.Data[0].y, _ddx.Data.size(), _ddx.Offset, 2 * sizeof(float));
		ImPlot::EndPlot();
	}
	if (ImPlot::BeginPlot("Wykresy 2")) {
		static float history = 10.0f;
		static ImPlotAxisFlags flags = ImPlotAxisFlags_NoTickLabels;

		ImPlot::SetupAxes(NULL, NULL, flags, flags);
		ImPlot::SetupAxisLimits(ImAxis_X1, -1.2, 1.2);
		ImPlot::SetupAxisLimits(ImAxis_Y1, -1.2, 1.2);
		ImPlot::SetNextFillStyle(IMPLOT_AUTO_COL, 0.5f);
		ImPlot::PlotLine("x'(x)", &_state.Data[0].x, &_state.Data[0].y, _state.Data.size(), _state.Offset, 2 * sizeof(float));
		ImPlot::EndPlot();
	}
}

void Arm::create_block_points()
{
	constexpr int N = 200;
	constexpr float delta = (2.0f * M_PI) / N;
	for (int i = 0; i < N; i++) {
		points.push_back(r * glm::sin(i * delta));
		points.push_back(r * glm::cos(i * delta));
		points.push_back(0.0f);

		points.push_back(color.x);
		points.push_back(color.y);
		points.push_back(color.z);
		points.push_back(color.w);


		quads.push_back(i);
		quads.push_back((i + 1) % N);
	}

	blocks_need_creation = false;
}

void Arm::update_object()
{
	if (blocks_need_creation) {
		quads.clear();
		points.clear();

		create_block_points();

		shader.use();
		// 1. bind Vertex Array Object
		glBindVertexArray(VAO);
		// 2. copy our vertices array in a vertex buffer for OpenGL to use
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * points.size(), points.data(), GL_DYNAMIC_DRAW);
		// 3. copy our index array in a element buffer for OpenGL to use
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * quads.size(), quads.data(), GL_DYNAMIC_DRAW);
		// 4. then set the vertex attributes pointers
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, description_number * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, description_number * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);
	}

	double Le = L;
	if (add_d) {
		Le += d(gen) * epsilon;
	}

	glm::vec2 point1 = { r * glm::sin(current_angle),r * glm::cos(current_angle) };

	glm::vec2 point2 = { point1.x + std::sqrt(Le * (double)Le - (double)(point1.y * point1.y)) ,0 };

	last_x = x;
	last_dx = dx;
	x = point2.x - Le ;
	float ddx = 0.0f;
	
	dx = 0.0f;
	if (dt > 0.0f) {
		dx = (x - last_x) / dt;
		ddx = (dx - last_dx) / dt;
	}

	_x.AddPoint(t,x);
	_dx.AddPoint(t,dx);
	_ddx.AddPoint(t,ddx);
	_state.AddPoint(x,dx);

	line->SetStEnd(point1, point2);
	block->SetCenter(point2);
}

Line::Line(Shader sh) :
	Object(sh, 7)
{
	start = { 0,0 };
	end = { 0,0 };
	color = { 0,1,1,1 };
	update_object();
}

void Line::DrawObject(glm::mat4 mvp)
{
	if (need_update) {
		update_object();
		need_update = false;
	}

	Object::DrawObject(mvp);

	glDrawElements(GL_LINES, quads.size(), GL_UNSIGNED_INT, 0);

	glBindVertexArray(0);
}

void Line::create_block_points()
{
	points.push_back(start.x);
	points.push_back(start.y);
	points.push_back(0.0f);

	points.push_back(color.x);
	points.push_back(color.y);
	points.push_back(color.z);
	points.push_back(color.w);


	points.push_back(end.x);
	points.push_back(end.y);
	points.push_back(0.0f);

	points.push_back(color.x);
	points.push_back(color.y);
	points.push_back(color.z);
	points.push_back(color.w);

	quads.push_back(0);
	quads.push_back(1);
}

void Line::update_object()
{
	quads.clear();
	points.clear();

	create_block_points();

	shader.use();
	// 1. bind Vertex Array Object
	glBindVertexArray(VAO);
	// 2. copy our vertices array in a vertex buffer for OpenGL to use
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * points.size(), points.data(), GL_DYNAMIC_DRAW);
	// 3. copy our index array in a element buffer for OpenGL to use
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * quads.size(), quads.data(), GL_DYNAMIC_DRAW);
	// 4. then set the vertex attributes pointers
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, description_number * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, description_number * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
}
