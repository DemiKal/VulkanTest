#pragma once
#include "vulkan.hpp"

struct SwapchainDimensions
{
	/// Width of the swapchain.
	uint32_t width = 0;

	/// Height of the swapchain.
	uint32_t height = 0;

	/// Pixel format of the swapchain.
	vk::Format format = vk::Format::eUndefined;
};

struct PerFrame
{
	vk::Device device;

	vk::Fence queue_submit_fence;

	vk::CommandPool primary_command_pool;

	vk::CommandBuffer primary_command_buffer;

	vk::Semaphore swapchain_acquire_semaphore;

	vk::Semaphore swapchain_release_semaphore;

	int32_t queue_index;
};

struct Context
{
	/// The Vulkan instance.
	vk::Instance instance;

	/// The Vulkan physical device.
	vk::PhysicalDevice gpu;

	/// The Vulkan device.
	vk::Device device;

	/// The Vulkan device queue.
	vk::Queue queue;

	/// The swapchain.
	vk::SwapchainKHR swapchain;

	/// The swapchain dimensions.
	SwapchainDimensions swapchain_dimensions;

	/// The surface we will render to.
	vk::SurfaceKHR surface;

	/// The queue family index where graphics work will be submitted.
	int32_t graphics_queue_index = -1;

	/// The image view for each swapchain image.
	std::vector<vk::ImageView> swapchain_image_views;

	/// The framebuffer for each swapchain image view.
	std::vector<vk::Framebuffer> swapchain_framebuffers;

	/// The renderpass description.
	vk::RenderPass render_pass;

	/// The graphics pipeline.
	vk::Pipeline pipeline;

	/**
	 * The pipeline layout for resources.
	 * Not used in this sample, but we still need to provide a dummy one.
	 */
	vk::PipelineLayout pipeline_layout;

	/// The debug report callback.
	vk::DebugReportCallbackEXT debug_callback;

	/// A set of semaphores that can be reused.
	std::vector<vk::Semaphore> recycled_semaphores;

	/// A set of per-frame data.
	std::vector<PerFrame> per_frame;
};