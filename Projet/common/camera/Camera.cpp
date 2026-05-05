#include "Camera.hpp"
#include "Camera_Helper.hpp"
	


// Include GLM
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <GLFW/glfw3.h>
#include <glm/gtx/norm.hpp>




void Camera::init()
{
	m_fovDegree = 45.0f;
	m_position = glm::vec3(0.f, 40.f, 0.f);
	m_eulerAngle = glm::vec3(0.f, 0.f, 0.f); //0:pitch 1:yaw 2:roll
	m_rotation = glm::quat{};
	//m_showImguiDemo = false;
	m_translationSpeed = 0.7f;
	m_rotationSpeed = 0.2f;
	m_input = false;
	m_thirdView = true;
	m_thirdViewEulerAngle = glm::vec3(20.f, 0.f, 0.f);
	m_targetDistance = 10.f;

}

static void updateAngle(glm::vec3 &_eulerAngles, int _index, float _rotationSpeed, glm::quat &_rotation)
{
	_eulerAngles[_index] += _rotationSpeed;
	if (_index == 0) //pitch
	{
		_eulerAngles[_index] = glm::clamp(_eulerAngles[_index], -89.9f, 89.9f);
	}
	else //yaw
	{
		_eulerAngles[_index] = Camera_Helper::clipAngle180(_eulerAngles[_index]);
	}
	_rotation = glm::quat(glm::radians(_eulerAngles));
}

void Camera::updateInterface(float _deltaTime)
{
	// ImGUI window creation
	if (ImGui::Begin("Interface"))
	{
		ImGui::Separator();
		ImGui::Text("Welcome to this TP about Cameras! Press escape to close the exe");
		ImGui::Text("Long live to the cameras - With actor included");
		ImGui::Separator();
		//ImGui::Checkbox("show Imgui Demo", &m_showImguiDemo);
		ImGui::DragFloat3("Position", &m_position[0],1.f, -80.0f, 80.0f);
		if (ImGui::DragFloat2("Euler Angle", &m_eulerAngle[0], 1.f, -180.0f, 180.0f)){
			m_rotation = glm::quat(glm::radians(m_eulerAngle));
		}
		ImGui::DragFloat("FOV Degree", &m_fovDegree, 1.f,1.0f, 179.0f);
		ImGui::DragFloat("Translation Speed", &m_translationSpeed, 0.005f, 0.01f, 1.0f);
		ImGui::DragFloat("Rotation Speed", &m_rotationSpeed, 0.05f, 0.1f, 10.0f);
		ImGui::Checkbox("Input Mode", &m_input);
		if(ImGui::Checkbox("Third View", &m_thirdView)){
			m_rotation = glm::quat(glm::radians(m_thirdViewEulerAngle));
		}
		

		

		
		

		if(ImGui::Button("Reset Camera"))
		{
			Camera::init();
		}
	}

	if(ImGui::IsKeyPressed(ImGuiKey_Z, false)){
			m_input = !m_input;
		}



	if(m_input == true && !m_thirdView){ //mode 1
			
		if(ImGui::IsKeyDown(ImGuiKey_W))
		{
			m_position += m_translationSpeed * Camera_Helper::projectVectorOnPlan(m_rotation * VEC_FRONT, m_rotation* VEC_UP);
		}
		if(ImGui::IsKeyDown(ImGuiKey_D))
		{
			m_position += m_translationSpeed * Camera_Helper::projectVectorOnPlan(m_rotation * -VEC_RIGHT, m_rotation* VEC_UP);
		}
		if(ImGui::IsKeyDown(ImGuiKey_A))
		{
			m_position += m_translationSpeed * Camera_Helper::projectVectorOnPlan(m_rotation * VEC_RIGHT, m_rotation* VEC_UP);
		}
		if(ImGui::IsKeyDown(ImGuiKey_S))
		{
			m_position += m_translationSpeed * Camera_Helper::projectVectorOnPlan(m_rotation * -VEC_FRONT, m_rotation* VEC_UP);
		}
		ImGuiIO& io = ImGui::GetIO();
		if(ImGui::IsMouseDown(ImGuiMouseButton_Left))
		{
			
			
			if(!io.WantCaptureMouse)
			{
				ImVec2 mousePos = ImGui::GetMouseDragDelta(ImGuiMouseButton_Left);
				updateAngle(m_eulerAngle, 1, m_rotationSpeed * static_cast<float>(mousePos.x), m_rotation);
				updateAngle(m_eulerAngle, 0, m_rotationSpeed * static_cast<float>(-mousePos.y), m_rotation);
				ImGui::ResetMouseDragDelta(ImGuiMouseButton_Left);
			}
			
		}	

	}else if(m_input == false && !m_thirdView){ //mode 2
		if(ImGui::IsKeyDown(ImGuiKey_W))
		{
			m_position += m_translationSpeed * Camera_Helper::projectVectorOnPlan(m_rotation * VEC_FRONT, VEC_UP);
		}
		if(ImGui::IsKeyDown(ImGuiKey_D))
		{
			m_position += m_translationSpeed * Camera_Helper::projectVectorOnPlan(m_rotation * -VEC_RIGHT, VEC_UP);
		}
		if(ImGui::IsKeyDown(ImGuiKey_A))
		{
			m_position += m_translationSpeed * Camera_Helper::projectVectorOnPlan(m_rotation * VEC_RIGHT, VEC_UP);
		}
		if(ImGui::IsKeyDown(ImGuiKey_S))
		{
			m_position += m_translationSpeed * Camera_Helper::projectVectorOnPlan(m_rotation * -VEC_FRONT, VEC_UP);
		}


		if(ImGui::IsKeyDown(ImGuiKey_UpArrow))
		{
			updateAngle(m_eulerAngle, 0, -m_rotationSpeed * 10.f, m_rotation);
		}
		if(ImGui::IsKeyDown(ImGuiKey_RightArrow))
		{
			updateAngle(m_eulerAngle, 1, -m_rotationSpeed * 10.f, m_rotation);
		}
		if(ImGui::IsKeyDown(ImGuiKey_LeftArrow))
		{
			updateAngle(m_eulerAngle, 1, m_rotationSpeed * 10.f, m_rotation);
		}
		if(ImGui::IsKeyDown(ImGuiKey_DownArrow))
		{
			updateAngle(m_eulerAngle, 0, m_rotationSpeed * 10.f, m_rotation);
		}
		if(ImGui::IsKeyDown(ImGuiKey_Q))
		{
			m_position += m_translationSpeed * Camera_Helper::projectVectorOnPlan(m_rotation * VEC_UP, VEC_RIGHT);
		}
		if(ImGui::IsKeyDown(ImGuiKey_E))
		{
			m_position -= m_translationSpeed * Camera_Helper::projectVectorOnPlan(m_rotation * VEC_UP, VEC_RIGHT);
		}


	}

	ImGui::End();
}

void Camera::updateFreeInput(float _deltaTime, GLFWwindow* _window)
{	
	if (m_thirdView && target)
	{	
		glm::vec3 carPos = target->getCarCenter(1.);
		glm::vec3 vitesse = target->getVitesse();
		float speed = glm::length(vitesse);

		glm::vec3 backwardDir;

		if (speed > 0.1f) {
			backwardDir = glm::normalize(-vitesse);
		} else {
			glm::mat4 carRotation = target->getTransformation().getRotationMatrix();
			backwardDir = glm::normalize(glm::vec3(carRotation * glm::vec4(-1.0f, 0.0f, 0.0f, 0.0f)));
		}

		float distance = 6.0f;
		float height = 2.0f;

		glm::vec3 desiredPos = carPos + (backwardDir * distance) + glm::vec3(0.0f, height, 0.0f);

		float smooth = 6.0f;
		m_position = glm::mix(m_position, desiredPos, _deltaTime * smooth);

		glm::mat4 m = glm::lookAt(m_position, carPos, VEC_UP);
		m_rotation = glm::quat_cast(glm::inverse(m));	
	}
}


void Camera::update(float _deltaTime, GLFWwindow* _window)
{
	if (target)
	{
		target->getCarCenter(1.);
	}
	//updateInterface(_deltaTime);
	updateFreeInput(_deltaTime, _window);

	Camera_Helper::computeFinalView(m_projectionMatrix, m_viewMatrix, m_position, m_rotation, m_fovDegree);
}