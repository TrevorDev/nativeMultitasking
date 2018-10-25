#pragma once

const int MAX_FRAMES_IN_FLIGHT = 2;

const std::vector<const char*> validationLayers = {
	"VK_LAYER_LUNARG_standard_validation"
};

const std::vector<const char*> deviceExtensions = {
	VK_KHR_SWAPCHAIN_EXTENSION_NAME
};


struct QueueFamilyIndices {
	std::optional<uint32_t> graphicsFamily;
	std::optional<uint32_t> presentFamily;

	bool isComplete() {
		return graphicsFamily.has_value() && presentFamily.has_value();
	}
};

struct SwapChainSupportDetails {
	vk::SurfaceCapabilitiesKHR capabilities;
	std::vector<vk::SurfaceFormatKHR> formats;
	std::vector<vk::PresentModeKHR> presentModes;
};

#ifdef NDEBUG
const bool enableValidationLayers = false;
#else
const bool enableValidationLayers = true;
#endif


class VulkanRenderer {
public:
	VulkanRenderer() {
	}

	void initializeInstance(bool enableValidationLayers) {
		if (enableValidationLayers && !this->checkValidationLayerSupport()) {
			throw std::runtime_error("validation layers requested, but not available!");
		}

		vk::ApplicationInfo appInfo;
		appInfo.pApplicationName = "Hello Triangle";
		appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
		appInfo.pEngineName = "No Engine";
		appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
		appInfo.apiVersion = VK_API_VERSION_1_0;

		vk::InstanceCreateInfo createInfo;
		createInfo.pApplicationInfo = &appInfo;

		auto extensions = this->getRequiredExtensions();
		createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
		createInfo.ppEnabledExtensionNames = extensions.data();

		if (enableValidationLayers) {
			createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
			createInfo.ppEnabledLayerNames = validationLayers.data();
		}
		else {
			createInfo.enabledLayerCount = 0;
		}

		this->instance = vk::createInstance(createInfo);

		// Setup debug layer
		if (!enableValidationLayers) return;

		VkDebugUtilsMessengerCreateInfoEXT createInfoExt = {};
		createInfoExt.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
		createInfoExt.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
		createInfoExt.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
		
		// TODO
		//createInfoExt.pfnUserCallback = debugCallback;

		/*if (CreateDebugUtilsMessengerEXT(instance, &createInfoExt, nullptr, &callback) != VK_SUCCESS) {
		throw std::runtime_error("failed to set up debug callback!");
		}*/
	}

	void initializePhysicalAndLogicalDevice(vk::SurfaceKHR& surface) {
		// Pick physical device
		auto devices = this->instance.enumeratePhysicalDevices();

		vk::Device device;
		for (const auto& device : devices) {
			if (this->isDeviceSuitable(device, surface)) {
				log("Found device");
				this->physicalDevice = device;
				break;
			}
		}

		if (!this->physicalDevice) {
			throw std::runtime_error("failed to find a suitable GPU!");
		}

		// Create logical device
		QueueFamilyIndices indices = findQueueFamilies(physicalDevice, surface);
		
		std::vector<vk::DeviceQueueCreateInfo> queueCreateInfos;
		std::set<uint32_t> uniqueQueueFamilies = { indices.graphicsFamily.value(), indices.presentFamily.value() };

		float queuePriority = 1.0f;
		for (uint32_t queueFamily : uniqueQueueFamilies) {
			vk::DeviceQueueCreateInfo queueCreateInfo = {};
			queueCreateInfo.queueFamilyIndex = queueFamily;
			queueCreateInfo.queueCount = 1;
			queueCreateInfo.pQueuePriorities = &queuePriority;
			queueCreateInfos.push_back(queueCreateInfo);
		}

		vk::PhysicalDeviceFeatures deviceFeatures = {};

		vk::DeviceCreateInfo deviceCreateInfo = {};

		deviceCreateInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
		deviceCreateInfo.pQueueCreateInfos = queueCreateInfos.data();

		deviceCreateInfo.pEnabledFeatures = &deviceFeatures;

		deviceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
		deviceCreateInfo.ppEnabledExtensionNames = deviceExtensions.data();

		if (enableValidationLayers) {
			deviceCreateInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
			deviceCreateInfo.ppEnabledLayerNames = validationLayers.data();
		}
		else {
			deviceCreateInfo.enabledLayerCount = 0;
		}

		this->device = this->physicalDevice.createDevice(deviceCreateInfo, nullptr);
		this->graphicsQueue = this->device.getQueue(indices.graphicsFamily.value(), 0);
		this->presentQueue = this->device.getQueue(indices.presentFamily.value(), 0);
	}

	void createSwapchainAndMemory(vk::SurfaceKHR& surface, int width, int height) {
		// Create swapchain
		SwapChainSupportDetails swapChainSupport = querySwapChainSupport(physicalDevice, surface);
		vk::SurfaceFormatKHR surfaceFormat = this->chooseSwapSurfaceFormat(swapChainSupport.formats);
		vk::PresentModeKHR presentMode = this->chooseSwapPresentMode(swapChainSupport.presentModes);
		vk::Extent2D extent = this->chooseSwapExtent(swapChainSupport.capabilities, width, height);

		uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
		if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount) {
			imageCount = swapChainSupport.capabilities.maxImageCount;
		}

		vk::SwapchainCreateInfoKHR swapchainCreateInfo = {};
		swapchainCreateInfo.surface = surface;

		swapchainCreateInfo.minImageCount = imageCount;
		swapchainCreateInfo.imageFormat = surfaceFormat.format;
		swapchainCreateInfo.imageColorSpace = surfaceFormat.colorSpace;
		swapchainCreateInfo.imageExtent = extent;
		swapchainCreateInfo.imageArrayLayers = 1;
		swapchainCreateInfo.imageUsage = vk::ImageUsageFlags(VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT);

		QueueFamilyIndices indices = findQueueFamilies(physicalDevice, surface);
		uint32_t queueFamilyIndices[] = { indices.graphicsFamily.value(), indices.presentFamily.value() };

		if (indices.graphicsFamily != indices.presentFamily) {
			swapchainCreateInfo.imageSharingMode = vk::SharingMode::eConcurrent;
			swapchainCreateInfo.queueFamilyIndexCount = 2;
			swapchainCreateInfo.pQueueFamilyIndices = queueFamilyIndices;
		}
		else {
			swapchainCreateInfo.imageSharingMode = vk::SharingMode::eExclusive;
		}

		swapchainCreateInfo.preTransform = swapChainSupport.capabilities.currentTransform;
		swapchainCreateInfo.compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque;
		swapchainCreateInfo.presentMode = presentMode;
		swapchainCreateInfo.clipped = VK_TRUE;

		//auto callbacks = vk::AllocationCallbacks();
		// TODO: EXTERNAL MEMORY HERE in allocator
		this->swapChain = device.createSwapchainKHR(swapchainCreateInfo);// , callbacks);
		this->swapChainImages = this->device.getSwapchainImagesKHR(this->swapChain);

		this->swapChainImageFormat = surfaceFormat.format;
		this->swapChainExtent = extent;

		// Create view images

		swapChainImageViews.resize(swapChainImages.size());

		for (size_t i = 0; i < swapChainImages.size(); i++) {
			vk::ImageViewCreateInfo createInfo = {};
			createInfo.image = swapChainImages[i];
			createInfo.viewType = vk::ImageViewType::e2D;
			createInfo.format = swapChainImageFormat;
			createInfo.components.r = vk::ComponentSwizzle::eIdentity;
			createInfo.components.g = vk::ComponentSwizzle::eIdentity;
			createInfo.components.b = vk::ComponentSwizzle::eIdentity;
			createInfo.components.a = vk::ComponentSwizzle::eIdentity;
			createInfo.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
			createInfo.subresourceRange.baseMipLevel = 0;
			createInfo.subresourceRange.levelCount = 1;
			createInfo.subresourceRange.baseArrayLayer = 0;
			createInfo.subresourceRange.layerCount = 1;

			this->swapChainImageViews[i] = device.createImageView(createInfo);
		}
	}

	void createRenderPass() {
		vk::AttachmentDescription colorAttachment = {};
		colorAttachment.format = swapChainImageFormat;
		colorAttachment.samples = vk::SampleCountFlagBits::e1;
		colorAttachment.loadOp = vk::AttachmentLoadOp::eClear;
		colorAttachment.storeOp = vk::AttachmentStoreOp::eStore;
		colorAttachment.stencilLoadOp = vk::AttachmentLoadOp::eDontCare;
		colorAttachment.stencilStoreOp = vk::AttachmentStoreOp::eDontCare;
		colorAttachment.initialLayout = vk::ImageLayout::eUndefined;
		colorAttachment.finalLayout = vk::ImageLayout::ePresentSrcKHR;

		vk::AttachmentReference colorAttachmentRef = {};
		colorAttachmentRef.attachment = 0;
		colorAttachmentRef.layout = vk::ImageLayout::eColorAttachmentOptimal;

		vk::SubpassDescription subpass = {};
		subpass.pipelineBindPoint = vk::PipelineBindPoint::eGraphics;
		subpass.colorAttachmentCount = 1;
		subpass.pColorAttachments = &colorAttachmentRef;

		vk::SubpassDependency dependency = {};
		dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
		dependency.dstSubpass = 0;
		dependency.srcStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput;
		//dependency.srcAccessMask = 0;
		dependency.dstStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput;
		dependency.dstAccessMask = vk::AccessFlagBits::eColorAttachmentRead| vk::AccessFlagBits::eColorAttachmentWrite;

		vk::RenderPassCreateInfo renderPassInfo = {};
		renderPassInfo.attachmentCount = 1;
		renderPassInfo.pAttachments = &colorAttachment;
		renderPassInfo.subpassCount = 1;
		renderPassInfo.pSubpasses = &subpass;
		renderPassInfo.dependencyCount = 1;
		renderPassInfo.pDependencies = &dependency;

		vk::RenderPass renderPass;
		vk::PipelineLayout pipelineLayout;
		vk::Pipeline graphicsPipeline;

		vk::CommandPool commandPool;
		std::vector<vk::CommandBuffer> commandBuffers;

		std::vector<vk::Semaphore> imageAvailableSemaphores;
		std::vector<vk::Semaphore> renderFinishedSemaphores;
		std::vector<vk::Fence> inFlightFences;
		this->renderPass = device.createRenderPass(renderPassInfo);
	}
	void createGraphicsPipeline(vk::SurfaceKHR& surface) {
		// Create graphics pipeline
		auto vertShaderCode = readFile("../nativeMultitasking/vert.spv");
		auto fragShaderCode = readFile("../nativeMultitasking/frag.spv");

		vk::ShaderModule vertShaderModule = createShaderModule(vertShaderCode, device);
		vk::ShaderModule fragShaderModule = createShaderModule(fragShaderCode, device);

		vk::PipelineShaderStageCreateInfo vertShaderStageInfo = {};
		vertShaderStageInfo.stage = vk::ShaderStageFlagBits::eVertex;
		vertShaderStageInfo.module = vertShaderModule;
		vertShaderStageInfo.pName = "main";

		vk::PipelineShaderStageCreateInfo fragShaderStageInfo = {};
		fragShaderStageInfo.stage = vk::ShaderStageFlagBits::eFragment;
		fragShaderStageInfo.module = fragShaderModule;
		fragShaderStageInfo.pName = "main";

		vk::PipelineShaderStageCreateInfo shaderStages[] = { vertShaderStageInfo, fragShaderStageInfo };

		vk::PipelineVertexInputStateCreateInfo vertexInputInfo = {};
		vertexInputInfo.vertexBindingDescriptionCount = 0;
		vertexInputInfo.vertexAttributeDescriptionCount = 0;

		vk::PipelineInputAssemblyStateCreateInfo inputAssembly = {};
		inputAssembly.topology = vk::PrimitiveTopology::eTriangleList;
		inputAssembly.primitiveRestartEnable = VK_FALSE;

		vk::Viewport viewport = {};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = (float)swapChainExtent.width;
		viewport.height = (float)swapChainExtent.height;
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;

		vk::Rect2D scissor = {};
		scissor.offset = { 0, 0 };
		scissor.extent = swapChainExtent;

		vk::PipelineViewportStateCreateInfo viewportState = {};
		viewportState.viewportCount = 1;
		viewportState.pViewports = &viewport;
		viewportState.scissorCount = 1;
		viewportState.pScissors = &scissor;

		vk::PipelineRasterizationStateCreateInfo rasterizer = {};
		rasterizer.depthClampEnable = VK_FALSE;
		rasterizer.rasterizerDiscardEnable = VK_FALSE;
		rasterizer.polygonMode = vk::PolygonMode::eFill;
		rasterizer.lineWidth = 1.0f;
		rasterizer.cullMode = vk::CullModeFlagBits::eBack;
		rasterizer.frontFace = vk::FrontFace::eClockwise;
		rasterizer.depthBiasEnable = VK_FALSE;

		vk::PipelineMultisampleStateCreateInfo multisampling = {};
		multisampling.sampleShadingEnable = VK_FALSE;
		multisampling.rasterizationSamples = vk::SampleCountFlagBits::e1;

		vk::PipelineColorBlendAttachmentState colorBlendAttachment = {};
		colorBlendAttachment.colorWriteMask = vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA;
		colorBlendAttachment.blendEnable = VK_FALSE;

		vk::PipelineColorBlendStateCreateInfo colorBlending = {};
		colorBlending.logicOpEnable = VK_FALSE;
		colorBlending.logicOp = vk::LogicOp::eCopy;
		colorBlending.attachmentCount = 1;
		colorBlending.pAttachments = &colorBlendAttachment;
		colorBlending.blendConstants[0] = 0.0f;
		colorBlending.blendConstants[1] = 0.0f;
		colorBlending.blendConstants[2] = 0.0f;
		colorBlending.blendConstants[3] = 0.0f;

		vk::PipelineLayoutCreateInfo pipelineLayoutInfo = {};
		pipelineLayoutInfo.setLayoutCount = 0;
		pipelineLayoutInfo.pushConstantRangeCount = 0;


		this->pipelineLayout = device.createPipelineLayout(pipelineLayoutInfo);

		vk::GraphicsPipelineCreateInfo pipelineInfo = {};
		pipelineInfo.stageCount = 2;
		pipelineInfo.pStages = shaderStages;
		pipelineInfo.pVertexInputState = &vertexInputInfo;
		pipelineInfo.pInputAssemblyState = &inputAssembly;
		pipelineInfo.pViewportState = &viewportState;
		pipelineInfo.pRasterizationState = &rasterizer;
		pipelineInfo.pMultisampleState = &multisampling;
		pipelineInfo.pColorBlendState = &colorBlending;
		pipelineInfo.layout = pipelineLayout;
		pipelineInfo.renderPass = renderPass;
		pipelineInfo.subpass = 0;
		//pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;

		
	/*	if (vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &graphicsPipeline) != VK_SUCCESS) {
			throw std::runtime_error("failed to create graphics pipeline!");
		}*/
		this->graphicsPipeline = device.createGraphicsPipelines(vk::PipelineCache(), pipelineInfo, nullptr)[0];

		vkDestroyShaderModule(device, fragShaderModule, nullptr);
		vkDestroyShaderModule(device, vertShaderModule, nullptr);

		// Create frame buffers
		this->swapChainFramebuffers.resize(swapChainImageViews.size());

		for (size_t i = 0; i < swapChainImageViews.size(); i++) {
			vk::ImageView attachments[] = {
				swapChainImageViews[i]
			};

			vk::FramebufferCreateInfo framebufferInfo = {};
			framebufferInfo.renderPass = renderPass;
			framebufferInfo.attachmentCount = 1;
			framebufferInfo.pAttachments = attachments;
			framebufferInfo.width = swapChainExtent.width;
			framebufferInfo.height = swapChainExtent.height;
			framebufferInfo.layers = 1;

			swapChainFramebuffers[i] = device.createFramebuffer(framebufferInfo);
		}

		// Create command pool
		QueueFamilyIndices poolQueueFamilyIndices = findQueueFamilies(physicalDevice, surface);

		vk::CommandPoolCreateInfo poolInfo = {};
		poolInfo.queueFamilyIndex = poolQueueFamilyIndices.graphicsFamily.value();

		this->commandPool = device.createCommandPool(poolInfo);

		// Create command buffers
		commandBuffers.resize(swapChainFramebuffers.size());

		vk::CommandBufferAllocateInfo allocInfo = {};
		allocInfo.commandPool = commandPool;
		allocInfo.level = vk::CommandBufferLevel::ePrimary;
		allocInfo.commandBufferCount = (uint32_t)commandBuffers.size();

		this->commandBuffers = device.allocateCommandBuffers(allocInfo);

		for (size_t i = 0; i < commandBuffers.size(); i++) {
			vk::CommandBufferBeginInfo beginInfo = {};
			beginInfo.flags = vk::CommandBufferUsageFlagBits::eSimultaneousUse;

			commandBuffers[i].begin(beginInfo);

			vk::RenderPassBeginInfo renderPassInfo = {};
			renderPassInfo.renderPass = renderPass;
			renderPassInfo.framebuffer = swapChainFramebuffers[i];
			renderPassInfo.renderArea.offset = { 0, 0 };
			renderPassInfo.renderArea.extent = swapChainExtent;

			vk::ClearValue clearColor = vk::ClearValue();
			clearColor.color.setFloat32({ 0.0f, 0.0f, 0.0f, 1.0f });
			renderPassInfo.clearValueCount = 1;
			renderPassInfo.pClearValues = &clearColor;

			commandBuffers[i].beginRenderPass(renderPassInfo, vk::SubpassContents::eInline);

			vkCmdBindPipeline(commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline);

			vkCmdDraw(commandBuffers[i], 3, 1, 0, 0);

			vkCmdEndRenderPass(commandBuffers[i]);

			if (vkEndCommandBuffer(commandBuffers[i]) != VK_SUCCESS) {
				throw std::runtime_error("failed to record command buffer!");
			}
		}

		// Create sync objects
		imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
		renderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
		inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);

		vk::SemaphoreCreateInfo semaphoreInfo = {};

		vk::FenceCreateInfo fenceInfo = {};
		fenceInfo.flags = vk::FenceCreateFlagBits::eSignaled;

		for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
			imageAvailableSemaphores[i] = device.createSemaphore(semaphoreInfo, nullptr);
			renderFinishedSemaphores[i] = device.createSemaphore(semaphoreInfo, nullptr);
			inFlightFences[i] = device.createFence(fenceInfo);
		}
	}
	void render() {
		
		// Draw frame
		device.waitForFences(inFlightFences[currentFrame], VK_TRUE, std::numeric_limits<uint64_t>::max());

		uint32_t imageIndex;
		VkResult result = vkAcquireNextImageKHR(device, swapChain, std::numeric_limits<uint64_t>::max(), imageAvailableSemaphores[currentFrame], VK_NULL_HANDLE, &imageIndex);
		//device.acquireNextImageKHR(swapChain, std::numeric_limits<uint64_t>::max(), imageAvailableSemaphores[currentFrame], vk::Fence(), &imageIndex);
		if (result == VK_ERROR_OUT_OF_DATE_KHR) {

			log("needs recreate");
			//recreateSwapChain();
			return;
		}
		else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
			throw std::runtime_error("failed to acquire swap chain image!");
		}

		vk::SubmitInfo submitInfo = {};

		vk::Semaphore waitSemaphores[] = { imageAvailableSemaphores[currentFrame] };
		vk::PipelineStageFlags waitStages[] = { vk::PipelineStageFlagBits::eColorAttachmentOutput };
		submitInfo.waitSemaphoreCount = 1;
		submitInfo.pWaitSemaphores = waitSemaphores;
		submitInfo.pWaitDstStageMask = waitStages;

		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &commandBuffers[imageIndex];

		vk::Semaphore signalSemaphores[] = { renderFinishedSemaphores[currentFrame] };
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = signalSemaphores;

		device.resetFences(inFlightFences[currentFrame]);

		graphicsQueue.submit(submitInfo, inFlightFences[currentFrame]);

		vk::PresentInfoKHR presentInfo = {};

		presentInfo.waitSemaphoreCount = 1;
		presentInfo.pWaitSemaphores = signalSemaphores;

		vk::SwapchainKHR swapChains[] = { swapChain };
		presentInfo.swapchainCount = 1;
		presentInfo.pSwapchains = swapChains;

		presentInfo.pImageIndices = &imageIndex;

		//result = vkQueuePresentKHR(presentQueue, &presentInfo);
		try {
			presentQueue.presentKHR(presentInfo);
		}
		catch (vk::OutOfDateKHRError err) {
			log(err.what());
			log("needs recreate");
			throw std::runtime_error("failed to present swap chain image!");
			//e.
			//if (err.code == vk::OutOfDateKHRError::Error){// || err.code == VK_SUBOPTIMAL_KHR) {// || framebufferResized) {
			//	
			//	/*framebufferResized = false;
			//	recreateSwapChain();*/
			//	throw std::runtime_error("failed to present swap chain image!");
			//}
			//else { // if (err.code != VK_SUCCESS) {
			//	throw std::runtime_error("failed to present swap chain image!");
			//}
			
		}
		catch (vk::SystemError err) {
			throw std::runtime_error("failed to present swap chain image!");
		}
		

		
		

		currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
		// Draw frame end
		
	}

	void finish() {
		device.waitIdle();
	}

	void createImage(int width, int height) {
		vk::ExternalMemoryImageCreateInfo extMemory = {};
		extMemory.handleTypes = vk::ExternalMemoryHandleTypeFlagBits::eOpaqueFd | vk::ExternalMemoryHandleTypeFlagBits::eOpaqueWin32;

		vk::ImageCreateInfo imageCreateInfo;
		imageCreateInfo.pNext = &extMemory;
		imageCreateInfo.imageType = vk::ImageType::e2D;
		imageCreateInfo.format = vk::Format::eB8G8R8A8Unorm;
		imageCreateInfo.extent = vk::Extent3D(width, height, 1);
		imageCreateInfo.mipLevels = 1;
		imageCreateInfo.arrayLayers = 1;
		imageCreateInfo.samples = vk::SampleCountFlagBits::e1;
		imageCreateInfo.usage = vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eSampled;
		//imageCreateInfo.initialLayout = vk::ImageLayout::eColorAttachmentOptimal;
		this->device.createImage(imageCreateInfo);

	}

	vk::Instance instance;
private:
	vk::DebugUtilsMessengerEXT callback;

	vk::PhysicalDevice physicalDevice;;
	vk::Device device;

	vk::Queue graphicsQueue;
	vk::Queue presentQueue;

	vk::SwapchainKHR swapChain;
	std::vector<vk::Image> swapChainImages;
	vk::Format swapChainImageFormat;
	vk::Extent2D swapChainExtent;
	std::vector<vk::ImageView> swapChainImageViews;
	std::vector<vk::Framebuffer> swapChainFramebuffers;

	vk::RenderPass renderPass;
	vk::PipelineLayout pipelineLayout;
	vk::Pipeline graphicsPipeline;

	vk::CommandPool commandPool;
	std::vector<vk::CommandBuffer> commandBuffers;

	std::vector<vk::Semaphore> imageAvailableSemaphores;
	std::vector<vk::Semaphore> renderFinishedSemaphores;
	std::vector<vk::Fence> inFlightFences;
	size_t currentFrame = 0;

	bool framebufferResized = false;


	// Random functions
	bool checkValidationLayerSupport() {
		uint32_t layerCount;
		vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

		std::vector<VkLayerProperties> availableLayers(layerCount);
		vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

		for (const char* layerName : validationLayers) {
			bool layerFound = false;

			for (const auto& layerProperties : availableLayers) {
				if (strcmp(layerName, layerProperties.layerName) == 0) {
					layerFound = true;
					break;
				}
			}

			if (!layerFound) {
				return false;
			}
		}

		return true;
	}

	std::vector<const char*> getRequiredExtensions() {
		uint32_t glfwExtensionCount = 0;
		const char** glfwExtensions;
		glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

		std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

		// TODO: EXTERNAM MEMORY
		// extensions.push_back("VK_KHR_get_physical_device_properties2");
		// extensions.push_back("VK_KHR_external_semaphore_capabilities");
		// extensions.push_back("VK_KHR_external_memory_capabilities");
		if (enableValidationLayers) {
			extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
		}

		return extensions;
	}

	QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device, VkSurfaceKHR surface) {
		QueueFamilyIndices indices;

		uint32_t queueFamilyCount = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

		std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
		vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

		int i = 0;
		for (const auto& queueFamily : queueFamilies) {
			if (queueFamily.queueCount > 0 && queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
				indices.graphicsFamily = i;
			}

			VkBool32 presentSupport = false;
			vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);

			if (queueFamily.queueCount > 0 && presentSupport) {
				indices.presentFamily = i;
			}

			if (indices.isComplete()) {
				break;
			}

			i++;
		}

		return indices;
	}

	bool checkDeviceExtensionSupport(VkPhysicalDevice device) {
		uint32_t extensionCount;
		vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

		std::vector<VkExtensionProperties> availableExtensions(extensionCount);
		vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

		std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());

		for (const auto& extension : availableExtensions) {
			requiredExtensions.erase(extension.extensionName);
		}

		return requiredExtensions.empty();
	}

	SwapChainSupportDetails querySwapChainSupport(vk::PhysicalDevice& device, vk::SurfaceKHR& surface) {
		SwapChainSupportDetails details;

		details.capabilities = device.getSurfaceCapabilitiesKHR(surface);
		details.formats = device.getSurfaceFormatsKHR(surface);
		details.presentModes = device.getSurfacePresentModesKHR(surface);

		return details;
	}

	bool isDeviceSuitable(vk::PhysicalDevice device, vk::SurfaceKHR surface) {
		QueueFamilyIndices indices = this->findQueueFamilies(device, surface);

		bool extensionsSupported = this->checkDeviceExtensionSupport(device);

		bool swapChainAdequate = false;
		if (extensionsSupported) {
			SwapChainSupportDetails swapChainSupport = this->querySwapChainSupport(device, surface);
			swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
		}

		return indices.isComplete() && extensionsSupported && swapChainAdequate;
	}

	vk::SurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& availableFormats) {
		if (availableFormats.size() == 1 && availableFormats[0].format == vk::Format::eUndefined) {
			return { vk::Format::eB8G8R8A8Unorm, vk::ColorSpaceKHR::eVkColorspaceSrgbNonlinear };
		}
		
		for (const auto& availableFormat : availableFormats) {
			if (availableFormat.format == vk::Format::eB8G8R8A8Unorm && availableFormat.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear) {
				return availableFormat;
			}
		}

		return vk::SurfaceFormatKHR(availableFormats[0]);
	}

	vk::PresentModeKHR chooseSwapPresentMode(const std::vector<vk::PresentModeKHR> availablePresentModes) {
		vk::PresentModeKHR bestMode = vk::PresentModeKHR::eFifo;

		for (const auto& availablePresentMode : availablePresentModes) {
			if (availablePresentMode == vk::PresentModeKHR::eMailbox) {
				return availablePresentMode;
			}
			else if (availablePresentMode == vk::PresentModeKHR::eImmediate) {
				bestMode = availablePresentMode;
			}
		}

		return bestMode;
	}

	vk::Extent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities, int width, int height) {
		if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
			return capabilities.currentExtent;
		}
		else {
			VkExtent2D actualExtent = {
				static_cast<uint32_t>(width),
				static_cast<uint32_t>(height)
			};

			actualExtent.width = std::max(capabilities.minImageExtent.width, std::min(capabilities.maxImageExtent.width, actualExtent.width));
			actualExtent.height = std::max(capabilities.minImageExtent.height, std::min(capabilities.maxImageExtent.height, actualExtent.height));

			return actualExtent;
		}
	}

	VkShaderModule createShaderModule(const std::vector<char>& code,VkDevice device) {
		VkShaderModuleCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		createInfo.codeSize = code.size();
		createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());
	
		VkShaderModule shaderModule;
		if (vkCreateShaderModule(device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS) {
			throw std::runtime_error("failed to create shader module!");
		}
	
		return shaderModule;
	}
};