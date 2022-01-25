#include "Camera.h"

#include <glm/gtc/matrix_transform.hpp>

Camera::Camera(glm::vec3 pos, glm::vec3 forward, glm::vec3 up)
{
	LookAt(pos, forward, up);
	//update_view_matrix();
	m_world_model_matrix = glm::mat4(1.0f);
	m_angleX = 0;
	m_angleY = 0;
	m_target = glm::vec4(0.0f);
	m_distance = 3.0f;
	m_minDistance = 0.1f;
	m_maxDistance = 1000.0f;
}

glm::mat4 Camera::ComputeProjectionMatrix(float n, float f, float l, float r, float t, float b) const
{
	glm::mat4 proj(0.0f) ;
	proj[0][0] = 2.0f * n / (r - l);
	proj[1][1] = 2.0f * n / (t - b);
	proj[2][2] = (f + n) / (f - n);
	proj[2][0] = (r + l) / (r - l);
	proj[2][1] = (t + b) / (t - b);
	proj[3][2] = 2.0f*f*n / (f - n);
	proj[2][3] = -1.0f;
	return proj;
}

void Camera::SetOrthographic(float left, float right, float top, float bottom, float near, float far)
{
	glm::mat4 persp = glm::mat4(1.0f);
	persp[0][0] = 2 / (right - left);
	persp[1][1] = 2 / (top - bottom);
	persp[2][2] = 2 / (far - near);
	persp[3][0] = -((right + left) / (right - left));
	persp[3][1] = -((top + bottom) / (top - bottom));
	persp[3][2] = -((far + near) / (far - near));
	persp[3][3] = -1;
	m_projection_matrix = persp;
}

void Camera::SetPerspective(float fov_y, float aspect, float near, float far)
{
	glm::mat4 persp = glm::mat4(1.0f);
	float e = 1 / tan(fov_y / 2);
	persp[1][1] = e;
	persp[0][0] = 1 / (aspect* tan(fov_y / 2));
	persp[2][2] = -((far + near) / (far - near));
	persp[3][2] = -((far * near * 2) / (far - near));
	persp[3][3] = 0;
	persp[2][3] = -1;
	m_projection_matrix = glm::orthoRH(-1.0f * aspect, 1.0f*aspect, -1.0f, 1.0f, 10.0f, -10.0f);;

}

void Camera::LookAt(glm::vec3 pos, glm::vec3 front, glm::vec3 up)
{
	this->pos = pos;
	this->front = front;
	this->up = up;
	right = glm::normalize(glm::cross(front, up));
	up_real = glm::normalize(glm::cross(right, front));

	glm::vec3 const f(glm::normalize(front));
	glm::vec3 const s(glm::normalize(cross(f, up)));
	glm::vec3 const u(glm::cross(s, f));

	glm::mat4 Result(1.0f);
	Result[0][0] = s.x;
	Result[1][0] = s.y;
	Result[2][0] = s.z;
	Result[0][1] = u.x;
	Result[1][1] = u.y;
	Result[2][1] = u.z;
	Result[0][2] = -f.x;
	Result[1][2] = -f.y;
	Result[2][2] = -f.z;
	Result[3][0] = -glm::dot(s, pos);
	Result[3][1] = -glm::dot(u, pos);
	Result[3][2] = glm::dot(f, pos);
	m_view_matrix = Result;
}



void Camera::SetPosFrontUp(glm::vec3& pos, glm::vec3& front, glm::vec3& up)
{
	pos = this->pos;
	front = this->front;
	up = this->up;
}

void Camera::RotateWorld(glm::vec3 angles)
{
	glm::mat4 x_rotate = glm::mat4(1.0f);
	x_rotate[1][1] = glm::cos(glm::radians(-angles.y));
	x_rotate[2][1] = -glm::sin(glm::radians(-angles.y));
	x_rotate[1][2] = glm::sin(glm::radians(-angles.y));
	x_rotate[2][2] = glm::cos(glm::radians(-angles.y));

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

	m_world_model_matrix = z_rotate * y_rotate * x_rotate * m_world_model_matrix;
}

void Camera::TranslateWorld(glm::vec3 movement)
{
	glm::mat4 translate = glm::mat4(1.0f);
	translate[3][0] = -movement.x;
	translate[3][1] = movement.y;
	translate[3][2] = movement.z;

	m_world_model_matrix = translate * m_world_model_matrix;
}

void Camera::ScaleWorld(glm::vec3 movement)
{
	glm::mat4 scale = glm::mat4(1.0f);
	scale[0][0] = movement.x;
	scale[1][1] = movement.y;
	scale[2][2] = movement.z;

	m_world_model_matrix = scale * m_world_model_matrix;
}

void Camera::MoveTarget(glm::vec4 v)
{
	m_target += v;
	update_view_matrix();
}

void Camera::Rotate(float dx, float dy)
{
	m_angleX += dx;
	m_angleX = m_angleX > 180 ? 180 : m_angleX;
	m_angleX = m_angleX < -180 ? -180 : m_angleX;

	m_angleY += dy;
	m_angleY = m_angleY > 180 ? 180 : m_angleY;
	m_angleY = m_angleY < -180 ? -180 : m_angleY;
	update_view_matrix();
}

void Camera::Zoom(float dd)
{
	m_distance -= dd;
	m_distance = m_distance > m_maxDistance ? m_maxDistance : m_distance;
	m_distance = m_distance < m_minDistance ? m_minDistance : m_distance;
	update_view_matrix();
}

void Camera::update_view_matrix()
{
	glm::mat4 x_rotate = glm::mat4(1.0f);
	x_rotate[1][1] = glm::cos(glm::radians(-m_angleY));
	x_rotate[2][1] = -glm::sin(glm::radians(-m_angleY));
	x_rotate[1][2] = glm::sin(glm::radians(-m_angleY));
	x_rotate[2][2] = glm::cos(glm::radians(-m_angleY));

	glm::mat4 y_rotate = glm::mat4(1.0f);
	y_rotate[0][0] = glm::cos(glm::radians(-m_angleX));
	y_rotate[2][0] = -glm::sin(glm::radians(-m_angleX));
	y_rotate[0][2] = glm::sin(glm::radians(-m_angleX));
	y_rotate[2][2] = glm::cos(glm::radians(-m_angleX));

	glm::mat4 translate_target = glm::mat4(1.0f);
	translate_target[3][0] = -m_target.x;
	translate_target[3][1] = -m_target.y;
	translate_target[3][2] = -m_target.z;

	glm::mat4 translate_dist = glm::mat4(1.0f);
	translate_dist[3][0] = 0;
	translate_dist[3][1] = 0;
	translate_dist[3][2] = -m_distance;

	m_view_matrix = translate_dist * x_rotate * y_rotate * translate_target;
}

