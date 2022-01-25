#include "Object.h"

glm::quat Object::RotationBetweenVectors(glm::vec3 start, glm::vec3 dest) {
	start = normalize(start);
	dest = normalize(dest);

	float cosTheta = dot(start, dest);
	glm::vec3 rotationAxis;

	rotationAxis = cross(start, dest);

	float s = sqrt((1 + cosTheta) * 2);
	float invs = 1 / s;

	return glm::quat(
		s * 0.5f,
		rotationAxis.x * invs,
		rotationAxis.y * invs,
		rotationAxis.z * invs
	);

}

Object::Object(Shader& shader_, int number) :
	shader(shader_),
	description_number(number),
	translate(glm::mat4(1.0f)),
	resize(glm::mat4(1.0f)),
	rotate(glm::mat4(1.0f)),
	mvp(glm::mat4(1.0f)),
	selected(false),
	position(glm::vec3(0.0f)),
	scale(glm::vec3(1.0f)),
	angle(glm::vec3(0.0f)),
	quaternion_rotation(glm::quat(1,0,0,0))
{
	glGenBuffers(1, &EBO);
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
}

Object::~Object()
{
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
}

void Object::DrawObject(glm::mat4 mvp_)
{	
	moved = false;
	mvp = mvp_ * translate * rotate * resize;
	shader.use();
	int projectionLoc = glGetUniformLocation(shader.ID, "mvp");
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(mvp));

	glBindVertexArray(VAO);
}

void Object::MoveObject(glm::vec3 movement)
{
	position += movement;
	if (moved) return;
	glm::mat4 translate_ = glm::mat4(1.0f);
	translate_[3][0] = position.x;
	translate_[3][1] = position.y;
	translate_[3][2] = position.z;

	translate = translate_ ;
	inform_owner_of_change();
	moved = true;
}

void Object::MoveObjectTo(glm::vec3 movement)
{
	position = movement;
	if (moved) return;
	glm::mat4 translate_ = glm::mat4(1.0f);
	translate_[3][0] = movement.x;
	translate_[3][1] = movement.y;
	translate_[3][2] = movement.z;

	translate = translate_;
	inform_owner_of_change();
	moved = true;
}

void Object::RotateObject(glm::vec3 angles)
{
	if (moved) return;
	angle += angles;
	glm::mat4 x_rotate = glm::mat4(1.0f);
	x_rotate[1][1] = glm::cos(glm::radians(angles.y));
	x_rotate[2][1] = glm::sin(glm::radians(angles.y));
	x_rotate[1][2] = -glm::sin(glm::radians(angles.y));
	x_rotate[2][2] = glm::cos(glm::radians(angles.y));

	glm::mat4 y_rotate = glm::mat4(1.0f);
	y_rotate[0][0] = glm::cos(glm::radians(angles.x));
	y_rotate[2][0] = -glm::sin(glm::radians(angles.x));
	y_rotate[0][2] = glm::sin(glm::radians(angles.x));
	y_rotate[2][2] = glm::cos(glm::radians(angles.x));

	glm::mat4 z_rotate = glm::mat4(1.0f);
	z_rotate[0][0] = glm::cos(glm::radians(angles.z));
	z_rotate[1][0] = -glm::sin(glm::radians(angles.z));
	z_rotate[0][1] = glm::sin(glm::radians(angles.z));
	z_rotate[1][1] = glm::cos(glm::radians(angles.z));

	rotate = z_rotate * y_rotate * x_rotate * rotate;
	inform_owner_of_change();
	moved = true;
}

void Object::RotateObject(glm::quat rotation)
{
	quaternion_rotation = rotation;

	rotate = glm::mat4(1.0f);

	rotate[0][0] = 1 - 2 * quaternion_rotation.y * quaternion_rotation.y - 2 * quaternion_rotation.z * quaternion_rotation.z;
	rotate[0][1] = 2 * quaternion_rotation.x * quaternion_rotation.y + 2 * quaternion_rotation.w * quaternion_rotation.z;
	rotate[0][2] = 2 * quaternion_rotation.x * quaternion_rotation.z - 2 * quaternion_rotation.w * quaternion_rotation.y;

	rotate[1][0] = 2 * quaternion_rotation.x * quaternion_rotation.y - 2 * quaternion_rotation.w * quaternion_rotation.z;
	rotate[1][1] = 1 - 2 * quaternion_rotation.x * quaternion_rotation.x - 2 * quaternion_rotation.z * quaternion_rotation.z;
	rotate[1][2] = 2 * quaternion_rotation.y * quaternion_rotation.z + 2 * quaternion_rotation.w * quaternion_rotation.x;

	rotate[2][0] = 2 * quaternion_rotation.x * quaternion_rotation.z + 2 * quaternion_rotation.w * quaternion_rotation.y;
	rotate[2][1] = 2 * quaternion_rotation.z * quaternion_rotation.y - 2 * quaternion_rotation.w * quaternion_rotation.x;
	rotate[2][2] = 1 - 2 * quaternion_rotation.x * quaternion_rotation.x - 2 * quaternion_rotation.y * quaternion_rotation.y;
}

void Object::ResizeObject(glm::vec3 movement)
{
	if (moved) return;
	scale *= movement;
	glm::mat4 scale2 = glm::mat4(1.0f);
	scale2[0][0] = scale.x;
	scale2[1][1] = scale.y;
	scale2[2][2] = scale.z;

	resize = scale2 ;
	inform_owner_of_change();
	moved = true;
}

void Object::Select()
{
	selected = true;
	if (selected != was_selected_in_last_frame) {
		update_object();
		was_selected_in_last_frame = selected;
	}
}

void Object::UnSelect()
{
	selected = false;
	if (selected != was_selected_in_last_frame) {
		update_object();
		was_selected_in_last_frame = selected;
	}
}

void Object::SetName(std::string name_)
{
	constname = name_;
	sprintf_s(name, 512, constname.c_str());
}

bool Object::CompareName(std::string name_)
{
	if (constname.compare(name_) == 0 ) return true;
	return false;
}

glm::vec3 Object::GetPosition()
{
	return position;
}
