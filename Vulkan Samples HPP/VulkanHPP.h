#pragma once
#include "vulkanStructs.h"
#include "VertexBuffer.h"


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
	void SetupRenderPassBeginInfo(Context& context)
	{
		m_ClearValues.clear();
		m_ClearValues.push_back(vk::ClearValue(std::array<float, 4>{0.1f, 0.1f, 0.1f, 1.0f}));
		m_ClearValues.push_back(vk::ClearDepthStencilValue{ 1.0f, 0 });

		m_RenderPassBeginInfo = vk::RenderPassBeginInfo();
		m_RenderPassBeginInfo.renderPass = context.render_pass;
		m_RenderPassBeginInfo.renderArea.extent = vk::Extent2D{ context.swapchain_dimensions.width,context.swapchain_dimensions.height };
		m_RenderPassBeginInfo.clearValueCount = (uint32_t)m_ClearValues.size();
		m_RenderPassBeginInfo.pClearValues = m_ClearValues.data();
	}

	void SetupDepthStencil(Context& context)
	{
		auto depthFormat = vk::Format::eD32SfloatS8Uint; //TODO: QUERY AND CHOOSE ONE

		vk::ImageAspectFlags aspect = vk::ImageAspectFlagBits::eDepth | vk::ImageAspectFlagBits::eStencil;
		vk::ImageCreateInfo depthStencilCreateInfo;
		depthStencilCreateInfo.imageType = vk::ImageType::e2D;
		depthStencilCreateInfo.extent = vk::Extent3D{ (uint32_t)m_Width, (uint32_t)m_Height, 1u };
		depthStencilCreateInfo.format = depthFormat;
		depthStencilCreateInfo.mipLevels = 1;
		depthStencilCreateInfo.arrayLayers = 1;
		depthStencilCreateInfo.usage = vk::ImageUsageFlagBits::eDepthStencilAttachment | vk::ImageUsageFlagBits::eTransferSrc;
		m_DepthStencil = CreateImage(depthStencilCreateInfo);

		SetImageLayout(m_DepthStencil.image, aspect, vk::ImageLayout::eUndefined, vk::ImageLayout::eDepthStencilAttachmentOptimal);

		vk::ImageViewCreateInfo depthStencilView;
		depthStencilView.viewType = vk::ImageViewType::e2D;
		depthStencilView.format = depthFormat;
		depthStencilView.subresourceRange.aspectMask = aspect;
		depthStencilView.subresourceRange.levelCount = 1;
		depthStencilView.subresourceRange.layerCount = 1;
		depthStencilView.image = m_DepthStencil.image;
		m_DepthStencil.view = m_Context.device.createImageView(depthStencilView);
	}


	void SetImageLayout(vk::Image image, vk::ImageAspectFlags aspectMask, vk::ImageLayout oldImageLayout, vk::ImageLayout newImageLayout) const {
		WithPrimaryCommandBuffer([&](const auto& commandBuffer) {
			SetImageLayout(commandBuffer, image, aspectMask, oldImageLayout, newImageLayout);
			});
	}

	void SetImageLayout(vk::CommandBuffer cmdbuffer,
		vk::Image image,
		vk::ImageAspectFlags aspectMask,
		vk::ImageLayout oldImageLayout,
		vk::ImageLayout newImageLayout) const 
	{
		vk::ImageSubresourceRange subresourceRange;
		subresourceRange.aspectMask = aspectMask;
		subresourceRange.levelCount = 1;
		subresourceRange.layerCount = 1;
		setImageLayout(cmdbuffer, image, oldImageLayout, newImageLayout, subresourceRange);
	}

	void setImageLayout(vk::CommandBuffer cmdbuffer,
		vk::Image image,
		vk::ImageLayout oldImageLayout,
		vk::ImageLayout newImageLayout,
		vk::ImageSubresourceRange subresourceRange) const {
		// Create an image barrier object
		vk::ImageMemoryBarrier imageMemoryBarrier;
		imageMemoryBarrier.oldLayout = oldImageLayout;
		imageMemoryBarrier.newLayout = newImageLayout;
		imageMemoryBarrier.image = image;
		imageMemoryBarrier.subresourceRange = subresourceRange;
		imageMemoryBarrier.srcAccessMask = AccessFlagsForLayout(oldImageLayout);
		imageMemoryBarrier.dstAccessMask = AccessFlagsForLayout(newImageLayout);
		vk::PipelineStageFlags srcStageMask = PipelineStageForLayout(oldImageLayout);
		vk::PipelineStageFlags destStageMask = PipelineStageForLayout(newImageLayout);
		// Put barrier on top
		// Put barrier inside setup command buffer
		cmdbuffer.pipelineBarrier(srcStageMask, destStageMask, vk::DependencyFlags(), nullptr, nullptr, imageMemoryBarrier);
	}

	void WithPrimaryCommandBuffer(const std::function<void(const vk::CommandBuffer& commandBuffer)>& f) const {
		vk::CommandBuffer commandBuffer = createCommandBuffer(vk::CommandBufferLevel::ePrimary);
		commandBuffer.begin(vk::CommandBufferBeginInfo{ vk::CommandBufferUsageFlagBits::eOneTimeSubmit });
		f(commandBuffer);
		commandBuffer.end();
		FlushCommandBuffer(commandBuffer);
		m_Context.device.freeCommandBuffers(m_Context.GetCommandPool(), commandBuffer);
	}
	void FlushCommandBuffer(vk::CommandBuffer& commandBuffer) const {
		if (!commandBuffer) {
			return;
		}
		m_Context.queue.submit(vk::SubmitInfo{ 0, nullptr, nullptr, 1, &commandBuffer }, vk::Fence());
		m_Context.queue.waitIdle();
		m_Context.device.waitIdle();
	}

	Image CreateImage(const vk::ImageCreateInfo& imageCreateInfo, //todo: add context?
		const vk::MemoryPropertyFlags& memoryPropertyFlags = vk::MemoryPropertyFlagBits::eDeviceLocal)   
	{
		auto& device = m_Context.device;
		
		Image result;
		result.device = device;
		result.image =   device.createImage(imageCreateInfo);
		result.format = imageCreateInfo.format;
		result.extent = imageCreateInfo.extent;
		m_DeviceMemoryProperties = m_Context.gpu.getMemoryProperties();

		vk::MemoryRequirements memReqs = device.getImageMemoryRequirements(result.image);
		vk::MemoryAllocateInfo memAllocInfo;
		
		memAllocInfo.allocationSize = result.allocSize = memReqs.size;
		memAllocInfo.memoryTypeIndex = GetMemoryType(memReqs.memoryTypeBits, memoryPropertyFlags);
		result.memory = device.allocateMemory(memAllocInfo);
		device.bindImageMemory(result.image, result.memory, 0);
		return result;
	}

	vk::CommandBuffer createCommandBuffer(vk::CommandBufferLevel level = vk::CommandBufferLevel::ePrimary) const {
		vk::CommandBuffer cmdBuffer;
		vk::CommandBufferAllocateInfo cmdBufAllocateInfo;
		cmdBufAllocateInfo.commandPool = m_Context.GetCommandPool();
		cmdBufAllocateInfo.level = level;
		cmdBufAllocateInfo.commandBufferCount = 1;
		cmdBuffer = m_Context.device.allocateCommandBuffers(cmdBufAllocateInfo)[0];
		return cmdBuffer;
	}
	vk::Bool32 GetMemoryType(uint32_t typeBits, const vk::MemoryPropertyFlags& properties, uint32_t* typeIndex) const {
		for (uint32_t i = 0; i < 32; i++) {
			if ((typeBits & 1) == 1) {
				if ((m_DeviceMemoryProperties.memoryTypes[i].propertyFlags & properties) == properties) {
					*typeIndex = i;
					return VK_TRUE;
				}
			}
			typeBits >>= 1;
		}
		return VK_FALSE;
	}

	uint32_t GetMemoryType(uint32_t typeBits, const vk::MemoryPropertyFlags& properties) const {
		uint32_t result = 0;
		if (VK_FALSE == GetMemoryType(typeBits, properties, &result)) {
			throw std::runtime_error("Unable to find memory type " + vk::to_string(properties));
			// todo : throw error
		}
		return result;
	}
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

	vk::AccessFlags AccessFlagsForLayout(vk::ImageLayout layout) const
	{
		switch (layout) {
		case vk::ImageLayout::ePreinitialized:
			return vk::AccessFlagBits::eHostWrite;
		case vk::ImageLayout::eTransferDstOptimal:
			return vk::AccessFlagBits::eTransferWrite;
		case vk::ImageLayout::eTransferSrcOptimal:
			return vk::AccessFlagBits::eTransferRead;
		case vk::ImageLayout::eColorAttachmentOptimal:
			return vk::AccessFlagBits::eColorAttachmentWrite;
		case vk::ImageLayout::eDepthStencilAttachmentOptimal:
			return vk::AccessFlagBits::eDepthStencilAttachmentWrite;
		case vk::ImageLayout::eShaderReadOnlyOptimal:
			return vk::AccessFlagBits::eShaderRead;
		default:
			return vk::AccessFlags();
		}
	}
	vk::PipelineStageFlags PipelineStageForLayout(vk::ImageLayout layout) const
	{
		switch (layout)
		{
		case vk::ImageLayout::eTransferDstOptimal:
		case vk::ImageLayout::eTransferSrcOptimal:
			return vk::PipelineStageFlagBits::eTransfer;

		case vk::ImageLayout::eColorAttachmentOptimal:
			return vk::PipelineStageFlagBits::eColorAttachmentOutput;

		case vk::ImageLayout::eDepthStencilAttachmentOptimal:
			return vk::PipelineStageFlagBits::eEarlyFragmentTests;

		case vk::ImageLayout::eShaderReadOnlyOptimal:
			return vk::PipelineStageFlagBits::eFragmentShader;

		case vk::ImageLayout::ePreinitialized:
			return vk::PipelineStageFlagBits::eHost;

		case vk::ImageLayout::eUndefined:
			return vk::PipelineStageFlagBits::eTopOfPipe;

		default:
			return vk::PipelineStageFlagBits::eBottomOfPipe;
		}
	};
	
	VmaAllocator m_Allocator;
	UniformBuffer* m_Uniform;
	GLFWwindow* m_GLFWwindow;
	Image m_DepthStencil;
	vk::PhysicalDeviceMemoryProperties m_DeviceMemoryProperties;
	Context m_Context;
	glm::vec2 m_MousePos;
	std::unique_ptr<Camera> m_Camera;
	std::vector<vk::ClearValue> m_ClearValues;
	vk::RenderPassBeginInfo m_RenderPassBeginInfo;
	int m_Width = 1024;
	int m_Height = 768;
	bool m_MouseLeft = false;
	bool m_MouseRight = false;
};

bool ValidateLayers(const std::vector<const char*>& required, const std::vector<vk::LayerProperties>& available);

#define VK_CHECK(x)                                                 \
	do                                                              \
	{                                                               \
		VkResult err = x;                                           \
		if (err)                                                    \
		{                                                           \
			std::cout << "Detected Vulkan error: " << err << std::endl; \
			 											\
			abort();                                                \
		}                                                           \
	} while (0)