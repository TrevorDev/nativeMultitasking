#pragma once

#include "j.h"
#include "vulkanInc.h"

struct QueueFamilyIndices {
    std::optional<uint32_t> graphicsFamily;
    std::optional<uint32_t> presentFamily;

    bool isComplete() {
        return graphicsFamily.has_value() && presentFamily.has_value();
    }
};

class Device {
    public:
    vk::PhysicalDevice _physicalDevice;
    vk::Device _device;
    vk::Queue _graphicsQueue;
    vk::Queue _presentQueue;
    Device(){
        
    }
    
    void init(VkSurfaceKHR surface, std::vector<vk::PhysicalDevice, std::allocator<vk::PhysicalDevice>>& devices){
        jlog("device count:");
        jlog(devices.size()); 
        auto found = false;
        for (const auto& device : devices) {
            // Check extensions supported
            auto availableExtensions = device.enumerateDeviceExtensionProperties();
            std::set<std::string> requiredExtensions;
            requiredExtensions.insert(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
            // requiredExtensions.insert(VK_KHR_EXTERNAL_MEMORY_EXTENSION_NAME);
            // requiredExtensions.insert(VK_KHR_EXTERNAL_SEMAPHORE_EXTENSION_NAME);
            // requiredExtensions.insert(VK_KHR_EXTERNAL_MEMORY_WIN32_EXTENSION_NAME);
            // requiredExtensions.insert(VK_KHR_EXTERNAL_SEMAPHORE_WIN32_EXTENSION_NAME);
            for (const auto& extension : availableExtensions) {
                requiredExtensions.erase(extension.extensionName);
            }
            if(!requiredExtensions.empty()){
                continue;
            }

            // Check swapchain support (TODO should this use querySwapChainSupport?)
            auto capabilities = device.getSurfaceCapabilitiesKHR(surface);
            auto formats = device.getSurfaceFormatsKHR(surface);
            auto presentModes = device.getSurfacePresentModesKHR(surface);
            
            auto swapChainAdequate = !formats.empty() && !presentModes.empty();
            
            if(!swapChainAdequate){
                continue;
            }

            // ensue device supports samplerAnisotropy
            auto features = device.getFeatures();
            if(!features.samplerAnisotropy){
                continue;
            }

            // Ensure queue families are present
            auto queues = findQueueFamilies(device, surface);
            if(!queues.isComplete()){
                continue;
            }
            
            _physicalDevice = device;
            found = true;
            jlog("device found!");
            break;
        }

        if (!found) {
            throw std::runtime_error("failed to find a suitable GPU!");
        }

        createLogicalDevice(surface);
    };
    
    //private:

    void createLogicalDevice(VkSurfaceKHR surface){
        QueueFamilyIndices indices = findQueueFamilies(_physicalDevice, surface);

        std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
        std::set<uint32_t> uniqueQueueFamilies = {indices.graphicsFamily.value(), indices.presentFamily.value()};

        float queuePriority = 1.0f;
        for (uint32_t queueFamily : uniqueQueueFamilies) {
            VkDeviceQueueCreateInfo queueCreateInfo = {};
            queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            queueCreateInfo.queueFamilyIndex = queueFamily;
            queueCreateInfo.queueCount = 1;
            queueCreateInfo.pQueuePriorities = &queuePriority;
            queueCreateInfos.push_back(queueCreateInfo);
        }

        VkPhysicalDeviceFeatures deviceFeatures = {};
        deviceFeatures.samplerAnisotropy = VK_TRUE;

        VkDeviceCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

        createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
        createInfo.pQueueCreateInfos = queueCreateInfos.data();

        createInfo.pEnabledFeatures = &deviceFeatures;

        std::vector<const char*> deviceExtensions = {
            VK_KHR_SWAPCHAIN_EXTENSION_NAME
        };

        createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
        createInfo.ppEnabledExtensionNames = deviceExtensions.data();

        createInfo.enabledLayerCount = 0;


        _device = _physicalDevice.createDevice(createInfo);
    
 
        _graphicsQueue = _device.getQueue(indices.graphicsFamily.value(),0);
        _presentQueue = _device.getQueue(indices.presentFamily.value(), 0);

        jlog("logical device created!");
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

    VkFormat findSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features) {
        for (VkFormat format : candidates) {
            VkFormatProperties props;
            vkGetPhysicalDeviceFormatProperties(_physicalDevice, format, &props);

            if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features) {
                return format;
            } else if (tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features) {
                return format;
            }
        }

        throw std::runtime_error("failed to find supported format!");
    }

    VkFormat findDepthFormat() {
        return findSupportedFormat(
        {VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT},
            VK_IMAGE_TILING_OPTIMAL,
            VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT
        );
    }
};