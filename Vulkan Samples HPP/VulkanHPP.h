#pragma once
#include "vulkanStructs.h"
#include "VertexBuffer.h"
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <GLFW/glfw3.h>

struct Camera
{
	float m_FOV;
	float m_AspectRatio;
	float m_NearPlane;
	float m_FarPlane;
	glm::vec3 m_Position;
	glm::vec3 m_Rotation;
	glm::vec3 m_Forward;
	glm::vec3 m_Up;
	glm::mat4 m_Projection;
	glm::mat4 m_View;
	
	glm::vec3 GetForward() const
	{
		glm::vec3 camFront;
		camFront.x = -cos(glm::radians(m_Rotation.x)) * sin(glm::radians(m_Rotation.y));
		camFront.y = sin(glm::radians(m_Rotation.x));
		camFront.z = cos(glm::radians(m_Rotation.x)) * cos(glm::radians(m_Rotation.y));
		return glm::normalize(camFront);
	}

	void UpdateViewMatrix()
	{
		glm::mat4 rotM = glm::mat4(1.0f);
		glm::mat4 transM;

		rotM = glm::rotate(rotM, glm::radians(m_Rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
		rotM = glm::rotate(rotM, glm::radians(m_Rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
		rotM = glm::rotate(rotM, glm::radians(m_Rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));

		transM = glm::translate(glm::mat4(1.0f), m_Position);

		m_View = transM*rotM  ;
	}

	void Move(glm::vec3 translation)
	{
		m_Position += translation;
	}
	void Rotate(glm::vec3 delta)
	{
		m_Rotation += delta;
		UpdateViewMatrix();
	}
	Camera(const glm::vec3& p_pos,
		const float p_fov,
		const float p_aspect,
		const float p_zNear,
		const float p_zFar)
		:
		m_FOV(p_fov),
		m_AspectRatio(p_aspect),
		m_NearPlane(p_zNear),
		m_FarPlane(p_zFar),
		m_Position(p_pos),
		m_Rotation(glm::vec3(0, 0, 0)),
		m_Forward(glm::vec3(0, 0, 1)),
		m_Up(glm::vec3(0, 1, 0)),
		m_Projection(glm::perspective(glm::radians(p_fov), p_aspect, p_zNear, p_zFar))
	{
		UpdateViewMatrix();
	}
};

class VulkanHPP
{
public:
	void RunLoop();
	void Update(float deltaTime);
	void RenderTriangle(Context& context, uint32_t swapchain_index);

	void Prepare();
	void InitLogger();
	void InitWindow();
	void InitInstance(Context& context, const std::vector<const char*>& required_instance_extensions, const std::vector<const char*>& required_validation_layers);
	void SelectPhysicalDeviceAndInstance(Context& context);
	void InitDevice(Context& context, const std::vector<const char*>& required_device_extensions);
	void InitSwapchain(Context& context);
	void InitRenderPass(Context& context);
	void InitPipeline(Context& context);
	void InitFrameBuffers(Context& context);
	void InitAllocator(Context& context);
	void InitVertices(Context& context);
	void InitUniformBuffer(Context& context);
	void InitDescriptorSetLayout(Context& context);
	vk::SurfaceKHR CreateSurface(vk::Instance instance, vk::PhysicalDevice);
	bool ValidateExtensions(const std::vector<const char*>& required, const std::vector<vk::ExtensionProperties>& available);
	void TeardownPerFrame(Context& context, PerFrame& per_frame);
	void InitPerFrame(Context& context, PerFrame& per_frame);
	void Resize(const uint32_t, const uint32_t);
	void TearDownFramebuffers(Context& context);
	vk::Result PresentImage(Context& context, uint32_t index);
	vk::Result AcquireNextImage(Context& context, uint32_t* image);
	void SetupDescriptorSet(Context& context);
	void InitDescriptorPool(Context& context);
	//void StageBuffer(Context& context, Buffer& allocBuffer, vk::BufferUsageFlagBits usageFlags, VmaMemoryUsage memoryUsage);
	void StageBuffer(Context& context, Buffer& allocBuffer, vk::BufferUsageFlagBits usageFlags, VmaMemoryUsage memoryUsage);
	void UpdateUniformBuffer(float dt);

	void MouseRelease(int button, int mods)
	{
		if (button == GLFW_MOUSE_BUTTON_LEFT) m_MouseLeft = false;
		if (button == GLFW_MOUSE_BUTTON_RIGHT) m_MouseRight = false;
	}

	void MousePress(int button, int mods)
	{
		if (button == GLFW_MOUSE_BUTTON_1) m_MouseLeft |= true;
		if (button == GLFW_MOUSE_BUTTON_2) m_MouseRight |= true;
	}

	void MouseMoved(const float newX, const float newY)
	{
		//int32_t dx = std::clamp((int32_t)(m_MousePos.x - newX), -1, 1);
		//int32_t dy = std::clamp((int32_t)(m_MousePos.y - newY), -1, 1);
		auto dx = m_MousePos.x - newX;
		auto dy = m_MousePos.y - newY;
		m_MousePos = glm::vec2(newX, newY);

		if (m_MouseLeft)
			m_Camera->Rotate(glm::vec3(dy, -dx, 0.0f));


	}

	void HandleInput(float dt)
	{
		auto camForward = m_Camera->GetForward() ;
		glm::vec3 camUp = glm::vec3(0.0f, 1.0f, 0.0f);
		glm::vec3 camLeft = glm::cross(camForward, camUp);

		camForward *= dt;
		camLeft *= dt;

		if (glfwGetKey(m_GLFWwindow, GLFW_KEY_W) == GLFW_PRESS)
			m_Camera->Move(camForward);
		if (glfwGetKey(m_GLFWwindow, GLFW_KEY_S) == GLFW_PRESS)
			m_Camera->Move(-camForward);
		if (glfwGetKey(m_GLFWwindow, GLFW_KEY_A) == GLFW_PRESS)
			m_Camera->Move(-camLeft);
		if (glfwGetKey(m_GLFWwindow, GLFW_KEY_D) == GLFW_PRESS)
			m_Camera->Move(camLeft);


	}


	void KeyReleased(int key) {}

	VmaAllocator m_Allocator;
	UniformBuffer* m_Uniform;//= UniformBuffer
	GLFWwindow* m_GLFWwindow;
	////	ubo_vs.proj = glm::perspective(glm::radians(70.0f), aspect, 0.0f, 100.0f);
	//ubo_vs.view = glm::translate(ubo_vs.view, glm::vec3(0, 0, 0.1f) * dt);
	bool m_MouseLeft = false;
	bool m_MouseRight = false;
	glm::vec2 m_MousePos;
	std::unique_ptr<Camera> m_Camera;
	int m_Width = 1024;
	int m_Height = 768;
	Context m_Context;
};

bool ValidateLayers(const std::vector<const char*>& required, const std::vector<vk::LayerProperties>& available);
