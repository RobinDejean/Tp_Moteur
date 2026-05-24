#pragma once

#include "../Node.hpp"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/norm.hpp>

struct GLFWwindow;

class Camera
{
public: 

	void init();
	void update(float _deltaTime, GLFWwindow* _window);
	void updateInterface(float _deltaTime);
	void updateFreeInput(float _deltaTime, GLFWwindow* _window);
	void computeFinalView();

	glm::quat getRotation() const {return m_rotation;}
	glm::mat4 getViewMatrix() const {return m_viewMatrix;}
	glm::mat4 getProjectionMatrix() const {return m_projectionMatrix;}
    void setTarget(Node* _target) { target = _target; }
	glm::vec3 getPosition() const { return m_position; }
	glm::vec3 getEulerAngle() const { return m_eulerAngle; }


private:

	//Camera parameters 
	float		m_fovDegree{ 45.0f };
	glm::vec3	m_position{ glm::vec3(0.f, 40.f, 0.f) };
	glm::vec3	m_eulerAngle{ glm::vec3(0.f, 0.f, 0.f) };
	glm::quat	m_rotation{};
	float m_translationSpeed = {0.7f};
	float m_rotationSpeed = {0.3f};
	bool m_input = {false};
	bool m_thirdView = {false};
	glm::vec3 m_thirdViewEulerAngle{ glm::vec3(20.f, 0.f, 0.f) };
	float m_targetDistance = {10.f};

	//Interface option
	//bool m_showImguiDemo{ false };

	//View
	glm::mat4 m_viewMatrix;
	glm::mat4 m_projectionMatrix;

	Node* target{ nullptr };
};