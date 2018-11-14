#include <fstream>
#include <stdexcept>
#include <algorithm>
#include <chrono>
#include <vector>
#include <cstring>
#include <cstdlib>
#include <array>
#include <optional>
#include <set>

#include <vulkan/vulkan.hpp>
#include <GLFW/glfw3.h>
#include "j.h"

class VulkanRenderer {
    public:
    vk::Instance _instance;
    vk::DebugUtilsMessengerEXT _callback;
    //vk::SurfaceKHR _surface;

    vk::PhysicalDevice _physicalDevice;
    vk::Device _device;

    vk::Queue _graphicsQueue;
    vk::Queue _presentQueue;

    vk::SwapchainKHR _swapChain;
    std::vector<vk::Image> _swapChainImages;
    vk::Format _swapChainImageFormat;
    vk::Extent2D _swapChainExtent;
    std::vector<vk::ImageView> _swapChainImageViews;
    std::vector<vk::Framebuffer> _swapChainFramebuffers;

    vk::RenderPass _renderPass;
    vk::DescriptorSetLayout _descriptorSetLayout;
    vk::PipelineLayout _pipelineLayout;
    vk::Pipeline _graphicsPipeline;

    vk::CommandPool _commandPool;

    vk::Image _depthImage;
    vk::DeviceMemory _depthImageMemory;
    vk::ImageView _depthImageView;

    vk::Image _textureImage;
    vk::DeviceMemory _textureImageMemory;
    vk::ImageView _textureImageView;
    vk::Sampler _textureSampler;

    vk::Image _textureImageExt;
    vk::DeviceMemory _textureImageMemoryExt;
    vk::ImageView _textureImageViewExt;
    vk::Sampler _textureSamplerExt;

    vk::Buffer _vertexBuffer;
    vk::DeviceMemory _vertexBufferMemory;
    vk::Buffer _indexBuffer;
    vk::DeviceMemory _indexBufferMemory;

    std::vector<vk::Buffer> _uniformBuffers;
    std::vector<vk::DeviceMemory> _uniformBuffersMemory;

    vk::DescriptorPool _descriptorPool;
    std::vector<vk::DescriptorSet> _descriptorSets;

    std::vector<vk::CommandBuffer> _commandBuffers;

    std::vector<vk::Semaphore> _imageAvailableSemaphores;
    std::vector<vk::Semaphore> _renderFinishedSemaphores;
    std::vector<vk::Fence> _inFlightFences;

    void constructor(){

    }

    void createInstance(){
        vk::ApplicationInfo appInfo;
        appInfo.pApplicationName = "Hello Triangle";
        appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.pEngineName = "No Engine";
        appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.apiVersion = VK_API_VERSION_1_0;

        vk::InstanceCreateInfo createInfo;
        createInfo.pApplicationInfo = &appInfo;

        // Extensions
        uint32_t glfwExtensionCount = 0;
        const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

        std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

        extensions.push_back(VK_KHR_EXTERNAL_MEMORY_CAPABILITIES_EXTENSION_NAME);
        extensions.push_back(VK_KHR_EXTERNAL_SEMAPHORE_CAPABILITIES_EXTENSION_NAME);
        

        createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
        createInfo.ppEnabledExtensionNames = extensions.data();

        createInfo.enabledLayerCount = 0;

        try {
            _instance = vk::createInstance(createInfo);
        }catch(...){
            jlog("External memory not supported, falling back");
            extensions.pop_back();
            extensions.pop_back();
            createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
            createInfo.ppEnabledExtensionNames = extensions.data();
            _instance = vk::createInstance(createInfo);
        }
        
        // if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS) {
        //     throw std::runtime_error("failed to create instance!");
        // }
    }

    void pickPhysicalDevice(){
        auto devices = _instance.enumeratePhysicalDevices();

        for (const auto& device : devices) {
            // Check extensions supported
            auto availableExtensions = device.enumerateDeviceExtensionProperties();

            std::set<std::string> requiredExtensions;
            requiredExtensions.insert(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
            requiredExtensions.insert(VK_KHR_EXTERNAL_MEMORY_EXTENSION_NAME);
            requiredExtensions.insert(VK_KHR_EXTERNAL_SEMAPHORE_EXTENSION_NAME);
            requiredExtensions.insert(VK_KHR_EXTERNAL_MEMORY_WIN32_EXTENSION_NAME);
            requiredExtensions.insert(VK_KHR_EXTERNAL_SEMAPHORE_WIN32_EXTENSION_NAME);

            for (const auto& extension : availableExtensions) {
                requiredExtensions.erase(extension.extensionName);
            }

            if(requiredExtensions.empty()){
                continue;
            }

            // bool swapChainAdequate = false;
            // SwapChainSupportDetails swapChainSupport = querySwapChainSupport(device);
            // swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
            

            // VkPhysicalDeviceFeatures supportedFeatures;
            // vkGetPhysicalDeviceFeatures(device, &supportedFeatures);

            // return indices.isComp
            
            // if (isDeviceSuitable(device)) {
            //     physicalDevice = device;
            //     break;
            // }
        }

        // if (physicalDevice == VK_NULL_HANDLE) {
        //     throw std::runtime_error("failed to find a suitable GPU!");
        // }
    };
    
    // setupDebugCallback();
    // createSurface();
    // pickPhysicalDevice();
    // createLogicalDevice();
    // createSwapChain();
    // createImageViews();
    // createRenderPass();
    // createDescriptorSetLayout();
    // createGraphicsPipeline();
    // createCommandPool();
    // createDepthResources();
    // createFramebuffers();
    // createTextureImageView();
    // createTextureSampler();
    // createVertexBuffer();
    // createIndexBuffer();
    // createUniformBuffers();
    // createDescriptorPool();
    // createDescriptorSets();
    // createCommandBuffers();
    // createSyncObjects();
};