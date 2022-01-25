#pragma once
#include <stdio.h>
#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <math.h>
#include <vector>
#include <algorithm>
#include <memory>
#include<functional>

#include "Shader.h"
#include "Camera.h"
#include "imgui.h"

class Object
	
{
public:
	Object(Shader& shader_, int number);
	Object() = default;
	~Object();
	virtual void DrawObject(glm::mat4 mvp);
	virtual void CreateMenu() {};
	void MoveObject(glm::vec3 movement);
	void MoveObjectTo(glm::vec3 movement);
	void RotateObject(glm::vec3 movement);
	void RotateObject(glm::quat rotation);
	void ResizeObject(glm::vec3 movement);
	virtual void Select();
	virtual void UnSelect();
	void SetName(std::string name_);
	bool CompareName(std::string name_);
	virtual void Update() {};
	virtual std::vector<std::shared_ptr<Object>> GetVirtualObjects() { return std::vector<std::shared_ptr<Object>>(); };
	virtual std::vector<std::function<glm::vec3(double, double)>> GetParametrisations() { return {}; };
	virtual std::vector<std::function<glm::vec3(double, double)>> GetUParametrisations() { return {}; };
	virtual std::vector<std::function<glm::vec3(double, double)>> GetVParametrisations() { return {}; };
	virtual void UpdateMyPointer(std::string constname_, std::shared_ptr<Object> new_point) {};

	static glm::quat RotationBetweenVectors(glm::vec3 start, glm::vec3 dest);
	void InformOwners() { inform_owner_of_change(); }
	glm::vec3 GetPosition();

	unsigned int* screen_width;
	unsigned int* screen_height;

	bool selected;
	char name[512];

	std::string constname;

	unsigned int textureID = 0;

protected:
	virtual void update_object() {};
	virtual void inform_owner_of_change() {};
	unsigned int VBO;
	unsigned int VAO;
	unsigned int EBO;
	

	bool was_selected_in_last_frame{false};
	bool need_update{ false };
	unsigned int description_number;
	 
	glm::mat4 translate;
	glm::mat4 rotate;
	glm::mat4 resize;
	
	glm::mat4 mvp;

	glm::vec3 position;
	glm::vec3 scale;
	glm::vec3 angle;
	glm::quat quaternion_rotation;


	glm::vec4 color;

	Shader shader;
	bool moved{ false };
};

