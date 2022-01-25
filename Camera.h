#pragma once
#include <glm/glm.hpp>

class Camera
{
public:
	Camera(glm::vec3 pos, glm::vec3 forward, glm::vec3 up);
	~Camera() = default;
	Camera() = default;

	glm::mat4 GetViewMatrix() const { return m_view_matrix; }
	glm::mat4 GetWorldModelMatrix() const { return m_world_model_matrix; }
	glm::mat4 GetProjectionMatrix() const { return m_projection_matrix; };
	glm::vec3 GetRightVector() const { return glm::normalize(right); };
	glm::vec3 GetUpVector() const { return glm::normalize(up_real); };

	glm::mat4 ComputeProjectionMatrix(float n, float f, float l, float r, float t, float b) const;

	void SetOrthographic(float left, float right, float top, float bottom, float near, float far);
	void SetPerspective(float fov, float aspect, float near, float far);

	void LookAt(glm::vec3 pos, glm::vec3 front, glm::vec3 up);
	void SetPosFrontUp(glm::vec3& pos, glm::vec3& front, glm::vec3& up);

	void RotateWorld(glm::vec3 angles);
	void TranslateWorld(glm::vec3 movement);
	void ScaleWorld(glm::vec3 movement);

	float getXAngle() const { return m_angleX; }
	float getYAngle() const { return m_angleY; }
	float getDistance() const { return m_distance; }
	glm::vec4 getTarget() const { return m_target; }
	glm::vec3 getPos() const { return pos; }

	void MoveTarget(glm::vec4 v);
	void Rotate(float dx, float dy);
	void Zoom(float dd);

private:
	void update_view_matrix();

	glm::vec3 m_position;
	glm::vec3 m_up_vector;
	glm::vec3 m_forward_vector;

	glm::vec3 front;
	glm::vec3 up;
	glm::vec3 pos;
	glm::vec3 right;
	glm::vec3 up_real;

	float m_angleX, m_angleY;
	float m_distance, m_minDistance, m_maxDistance;
	glm::vec4 m_target;

	glm::mat4 m_view_matrix;
	glm::mat4 m_world_model_matrix;
	glm::mat4 m_projection_matrix;
};
