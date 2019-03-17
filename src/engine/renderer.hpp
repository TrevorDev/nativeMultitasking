#pragma once
#include "../j.hpp"
#include "vulkanInc.hpp"
#include "../engine/instance.hpp"
#include "../engine/device.hpp"
#include "../engine/swapchain.hpp"
#include "../engine/pipeline.hpp"

#include "../engine/glmInc.h"
#include "../object3d/camera.hpp"
#include "../object3d/mesh.hpp"
#include "../object3d/pointLight.hpp"
#include "../object3d/scene.hpp"

class Renderer {
    public:
    Instance _instance;
    Device _device;

    Renderer(){
        
    }

    void initInstance(std::vector<std::string>& instanceExtensions){
        _instance.init(instanceExtensions);
    }

    void initDevice(VkSurfaceKHR surface){
        std::vector<vk::PhysicalDevice> x =  _instance._instance.enumeratePhysicalDevices();
        _device.init(surface, x);
    }

    void getNextImage(Swapchain& swapchain){
        _device._device.waitForFences(_device._inFlightFences[swapchain._currentFrame], VK_TRUE, std::numeric_limits<uint64_t>::max());
        swapchain.getNextImage(_device);
    }

    void presentFrame(Swapchain& swapchain, vk::Semaphore* signalSemaphores){
        // present frame
        vk::PresentInfoKHR presentInfo = {};
        presentInfo.setPWaitSemaphores(signalSemaphores);
        vk::SwapchainKHR swapChains[] = {swapchain._swapChain};
        presentInfo.swapchainCount = 1;
        presentInfo.setPSwapchains(swapChains);
        presentInfo.pImageIndices = &swapchain._currentImageIndex;
        vk::Result result;
        
        try{
            result = _device._presentQueue.presentKHR(presentInfo);
        }catch(const std::exception& e){
            result = vk::Result::eErrorOutOfDateKHR;
        }
        

        // if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || _framebufferResized) {
        //     framebufferResized = false;
        //     //recreateSwapChain();
        // } else 
        if (result != vk::Result::eSuccess) {
            throw std::runtime_error("failed to present swap chain image!");
        }
        swapchain._currentFrame = (swapchain._currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
    }
    
    private:
    
};