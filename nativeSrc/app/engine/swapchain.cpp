#pragma once

#include "j.h"
#include "vulkanInc.h"
#include "device.cpp"
#include "image.cpp"

struct SwapChainSupportDetails {
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;
};

class Swapchain {
    public:
    vk::SwapchainKHR _swapChain;
    VkFormat _swapChainImageFormat;
    vk::Extent2D _swapChainExtent;
    std::vector<Image> _swapChainImages = {};
    Image _depthImage;

    // TODO why are there 2 variables below? Do they store the same info?
    uint32_t _currentFrame = 0;
    uint32_t _currentImageIndex = 0;
    
    Swapchain(){
        
    }
    void init(VkSurfaceKHR surface, uint32_t width, uint32_t height, Device device){
        // Get information about features the swapchain supports and choose the ideal settings
        SwapChainSupportDetails swapChainSupport = querySwapChainSupport(device._physicalDevice, surface);
        VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);
        VkPresentModeKHR presentMode = chooseSwapPresentMode(swapChainSupport.presentModes); // TODO: this should be an option, i think VK_PRESENT_MODE_IMMEDIATE_KHR is best for vr
        _swapChainExtent = chooseSwapExtent(swapChainSupport.capabilities, width, height);

        // Set the count of images within the swapchain
        // TODO: expose this as an option
        uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
        if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount) {
            imageCount = swapChainSupport.capabilities.maxImageCount;
        }

        // Create the swapchain
        VkSwapchainCreateInfoKHR createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        createInfo.surface = surface;
        createInfo.minImageCount = imageCount;
        createInfo.imageFormat = surfaceFormat.format;
        createInfo.imageColorSpace = surfaceFormat.colorSpace;
        createInfo.imageExtent = _swapChainExtent;
        createInfo.imageArrayLayers = 1;
        createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

        QueueFamilyIndices indices = device.findQueueFamilies(device._physicalDevice, surface);
        uint32_t queueFamilyIndices[] = {indices.graphicsFamily.value(), indices.presentFamily.value()};

        if (indices.graphicsFamily != indices.presentFamily) {
            createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
            createInfo.queueFamilyIndexCount = 2;
            createInfo.pQueueFamilyIndices = queueFamilyIndices;
        } else {
            createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        }

        createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
        createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        createInfo.presentMode = presentMode;
        createInfo.clipped = VK_TRUE;

        _swapChain = device._device.createSwapchainKHR(createInfo);
        _swapChainImageFormat = surfaceFormat.format;

        
        // Convert swapcahin images to image class
        for(auto& image : device._device.getSwapchainImagesKHR(_swapChain)){
            auto newImage = Image();
            newImage._image = image;
            newImage._device = device;
            newImage.createImageView(_swapChainImageFormat, VK_IMAGE_ASPECT_COLOR_BIT);
            _swapChainImages.push_back(newImage);            
        }
        jlog("created swapchain with image count of:");
        jlog(_swapChainImages.size());

        // Create depth image used when drawing
        // TODO not sure if this should be created here, somewhere else or if other buffers should be created like stencil?
        VkFormat depthFormat = device.findDepthFormat();
        _depthImage = Image();
        _depthImage._device = device;
        _depthImage.createImage(_swapChainExtent.width, _swapChainExtent.height, depthFormat, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
        _depthImage.createImageView(depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT);
        _depthImage.transitionImageLayout(depthFormat, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);
    }

    void getNextImage(Device& device){
        VkResult result = vkAcquireNextImageKHR(device._device, _swapChain, std::numeric_limits<uint64_t>::max(), device._imageAvailableSemaphores[_currentFrame], VK_NULL_HANDLE, &_currentImageIndex);

        if (result == VK_ERROR_OUT_OF_DATE_KHR) {
            // TODO recreate swapchain here
            //recreateSwapChain();
            jlog("out of date, need to recreate");
            return;
        } else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
            throw std::runtime_error("failed to acquire swap chain image!");
        }
    }
    
    private:
    SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device, VkSurfaceKHR surface) {
        SwapChainSupportDetails details;

        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.capabilities);

        uint32_t formatCount;
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);

        if (formatCount != 0) {
            details.formats.resize(formatCount);
            vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, details.formats.data());
        }

        uint32_t presentModeCount;
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, nullptr);

        if (presentModeCount != 0) {
            details.presentModes.resize(presentModeCount);
            vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, details.presentModes.data());
        }

        return details;
    }
    VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats) {
        if (availableFormats.size() == 1 && availableFormats[0].format == VK_FORMAT_UNDEFINED) {
            return{VK_FORMAT_B8G8R8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR};
        }

        for (const auto& availableFormat : availableFormats) {
            if (availableFormat.format == VK_FORMAT_B8G8R8A8_UNORM && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
                return availableFormat;
            }
        }

        return availableFormats[0];
    }

    VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR> availablePresentModes) {
        VkPresentModeKHR bestMode = VK_PRESENT_MODE_FIFO_KHR;

        for (const auto& availablePresentMode : availablePresentModes) {
            if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
                return availablePresentMode;
            } else if (availablePresentMode == VK_PRESENT_MODE_IMMEDIATE_KHR) {
                bestMode = availablePresentMode;
            }
        }

        return bestMode;
    }

    VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities, uint32_t width, uint32_t height) {
        if (capabilities.currentExtent.width != (std::numeric_limits<uint32_t>::max)()) {
            return capabilities.currentExtent;
        } else {
            VkExtent2D actualExtent = {
                static_cast<uint32_t>(width),
                static_cast<uint32_t>(height)
            };

            actualExtent.width = (std::max)(capabilities.minImageExtent.width, (std::min)(capabilities.maxImageExtent.width, actualExtent.width));
            actualExtent.height = (std::max)(capabilities.minImageExtent.height, (std::min)(capabilities.maxImageExtent.height, actualExtent.height));

            return actualExtent;
        }
    }
};