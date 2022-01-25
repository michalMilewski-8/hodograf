#include "Torus.h"
#define _USE_MATH_DEFINES
#include <math.h>
unsigned int Torus::counter = 1;

Torus::Torus(float R, float r, int vertical, int horizontal, glm::vec4 color, Shader sh) :
	Object(sh, 9)
{
	sprintf_s(name, 512, ("Torus " + std::to_string(counter)).c_str());
	constname = "Torus " + std::to_string(counter);
	counter++;
	this->R = R;
	this->r = r;
	vertical_points_number = vertical;
	horizontal_points_number = horizontal;
	this->color = color;
	shader = Shader("shader_tex.vs","shader_tex.fs");
	update_object();
}

void Torus::DrawObject(glm::mat4 mvp)
{
	Object::DrawObject(mvp);
	auto texLocation = glGetUniformLocation(shader.ID, "trimm_texture");

	glUniform1i(texLocation, 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureID);

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glDrawElements(GL_LINES,triangles.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

void Torus::CreateMenu()
{
	float R_new;
	float r_new;
	int vertical_points_number_new;
	int horizontal_points_number_new;
	float color_new[4];
	char buffer[512];
	sprintf_s(buffer, "%s###%s", name,constname);
	if (ImGui::TreeNode(buffer)) {
		
		if (ImGui::BeginPopupContextItem())
		{
			ImGui::Text("Edit name:");
			ImGui::InputText("##edit", name, IM_ARRAYSIZE(name));
			if (ImGui::Button("Close"))
				ImGui::CloseCurrentPopup();
			ImGui::EndPopup();
		}                                                    
		R_new = R;
		r_new = r;
		vertical_points_number_new = vertical_points_number;
		horizontal_points_number_new = horizontal_points_number;
		for (int i = 0; i < 4; i++)
			color_new[i] = color[i];
		ImGui::Checkbox("Selected",&selected);
		if (selected != was_selected_in_last_frame) {
			update_object();
			was_selected_in_last_frame = selected;
		}
		ImGui::Text("Set number of divistions:");
		ImGui::SliderInt("Vertical", &vertical_points_number_new, 1, 100);
		ImGui::SliderInt("Horizontal", &horizontal_points_number_new, 1, 100);
		ImGui::Text("Set radiuses:");
		ImGui::SliderFloat("R", &R_new, 0.01, 10);
		ImGui::SliderFloat("r", &r_new, 0.01, 10);
		ImGui::Text("Set color:");
		ImGui::ColorPicker4("Color", color_new);
		ImGui::TreePop();
		ImGui::Separator();

		if (R_new != R || r_new != r ||
			vertical_points_number_new != vertical_points_number ||
			horizontal_points_number_new != horizontal_points_number)
		{
			R = R_new > 0 ? R_new : R;
			r = r_new > 0 ? r_new : r;
			vertical_points_number = vertical_points_number_new > 0 ? vertical_points_number_new : vertical_points_number;
			horizontal_points_number = horizontal_points_number_new > 0 ? horizontal_points_number_new : horizontal_points_number;
			update_object();
		}
		bool difference = false;
		for (int i = 0; i < 4; i++)
			if(color_new[i] != color[i])
			{
				difference = true;
				break;
			}
		if (difference) {
			color = { color_new[0],color_new[1],color_new[2],color_new[3] };
			update_object();
		}
	}
	
}

std::vector<std::function<glm::vec3(double, double)>> Torus::GetParametrisations()
{
	auto result = std::vector<std::function<glm::vec3(double, double)>>();
	result.push_back([=](double u, double v) {
		glm::vec3 res = { 
			glm::cos(2 * M_PI * v) * R - glm::sin(2 * M_PI * u) * glm::cos(2 * M_PI * v) * r,
			glm::cos(2 * M_PI * u) * r,
			glm::sin(2 * M_PI * v) * R - glm::sin(2 * M_PI * u) * glm::sin(2 * M_PI * v) * r };
		return (translate * rotate * resize * glm::vec4( res ,1));
		});
	return result;
}

std::vector<std::function<glm::vec3(double, double)>> Torus::GetUParametrisations()
{
	auto result = std::vector<std::function<glm::vec3(double, double)>>();
	result.push_back([=](double u, double v) {
		glm::vec3 res = {
			-2 * M_PI * glm::cos(2 * M_PI * u) * glm::cos(2 * M_PI * v) * r,
			-2 * M_PI * glm::sin(2 * M_PI * u) * r,
			-2 * M_PI * glm::cos(2 * M_PI * u) * glm::sin(2 * M_PI * v) * r };
		return (translate * rotate * resize * glm::vec4(res, 0));
		});
	return result;
}

std::vector<std::function<glm::vec3(double, double)>> Torus::GetVParametrisations()
{
	auto result = std::vector<std::function<glm::vec3(double, double)>>();
	result.push_back([=](double u, double v) {
		glm::vec3 res = {
			-2 * M_PI * glm::sin(2 * M_PI * v) * R + 2 * M_PI * glm::sin(2 * M_PI * u) * glm::sin(2 * M_PI * v) * r,
			0,
			2 * M_PI* glm::cos(2 * M_PI * v) * R - 2 * M_PI * glm::sin(2 * M_PI * u) * glm::cos(2 * M_PI * v) * r };
		return (translate * rotate * resize * glm::vec4(res, 0));
		});
	return result;
}

void Torus::SetR(float _R)
{
	R = _R;
	update_object();
}

void Torus::Setr(float _r)
{
	r = _r;
	update_object();
}

void Torus::SetVertical(int _v)
{
	vertical_points_number = _v;
	update_object();
}

void Torus::SetHorizontal(float _h)
{
	horizontal_points_number = _h;
	update_object();
}

void Torus::SetColor(glm::vec4 _c)
{
	color = _c;
	update_object();
}

void Torus::create_torus_points() {
	float vertical_stride = 360.0f / (vertical_points_number);
	float horizontal_stride = 360.0f / (horizontal_points_number);

	for (int i = 0; i < horizontal_points_number; i++) {
		float beta = i * horizontal_stride;
		float u = beta / 360.0f;
		for (int j = 0; j < vertical_points_number; j++) {
			float alfa = j * vertical_stride;
			float v = alfa / 360.0f;
			glm::vec3 point = torus_point(glm::radians(alfa), glm::radians(beta));
			points.push_back(point.x);
			points.push_back(point.y);
			points.push_back(point.z);
			if (!selected) {
				points.push_back(color.r);
				points.push_back(color.g);
				points.push_back(color.b);
			}
			else {
				points.push_back(1.0f);
				points.push_back(0.0f);
				points.push_back(0.0f);
			}
			points.push_back(1.0f);

			points.push_back(v);
			points.push_back(u);

			triangles.push_back(i * vertical_points_number + j);
			//triangles.push_back(((i + 1) % horizontal_points_number) * vertical_points_number + (vertical_points_number + (j - 1) % vertical_points_number) % vertical_points_number);
			triangles.push_back(((i + 1) % horizontal_points_number) * vertical_points_number + j);
			triangles.push_back(i * vertical_points_number + j);
			//triangles.push_back(((i + 1) % horizontal_points_number) * vertical_points_number + j);
			triangles.push_back(i * vertical_points_number + (j + 1) % vertical_points_number);
		}
	}
}

glm::vec3 Torus::torus_point(float alfa_r, float beta_r) {
	return { glm::cos(beta_r) * R - glm::sin(alfa_r) * glm::cos(beta_r) * r,glm::cos(alfa_r) * r,glm::sin(beta_r) * R - glm::sin(alfa_r) * glm::sin(beta_r) * r, };
}

void Torus::update_object()
{
	triangles.clear();
	points.clear();

	create_torus_points();

	// 1. bind Vertex Array Object
	glBindVertexArray(VAO);
	// 2. copy our vertices array in a vertex buffer for OpenGL to use
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * points.size(), points.data(), GL_DYNAMIC_DRAW);
	// 3. copy our index array in a element buffer for OpenGL to use
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * triangles.size(), triangles.data(), GL_DYNAMIC_DRAW);
	// 4. then set the vertex attributes pointers
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, description_number * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, description_number * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, description_number * sizeof(float), (void*)(7 * sizeof(float)));
	glEnableVertexAttribArray(2);
}
