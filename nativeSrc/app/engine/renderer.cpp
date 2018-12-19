#pragma once
#include "j.h"
#include "vulkanInc.h"
#include "instance.cpp"
#include "device.cpp"
#include "../engine/swapchain.cpp"
#include "../engine/pipeline.cpp"
#include "../engine/material.cpp"

#include "../engine/glmInc.h"
#include "object3d/camera.cpp"
#include "object3d/mesh.cpp"

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
        _device.init(surface, _instance._instance.enumeratePhysicalDevices());
    }

    void updateUniformBuffer(uint32_t currentImage, Swapchain& swapchain, std::vector<vk::DeviceMemory>& _uniformBuffersMemory) {
        static auto startTime = std::chrono::high_resolution_clock::now();

        auto currentTime = std::chrono::high_resolution_clock::now();
        float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

        Camera c;
        c.position.z = -3;
        c.computeWorldMatrix();
        
        float x[] = {
        1,0,0,0,
        0,1,0,0,
        0,0,1,0,
        0,0,-3,1
        };


        UniformBufferObject ubo = {};
        
        ubo.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        ubo.view = glm::make_mat4((float*)(c._worldMatrix.m));  //glm::lookAt(glm::vec3(0.0f, 0.0, 3.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        ubo.proj = glm::perspective(glm::radians(45.0f), swapchain._swapChainExtent.width / (float) swapchain._swapChainExtent.height, 0.1f, 10.0f);
        ubo.proj[1][1] *= -1;

        void* data;
        vkMapMemory(_device._device, _uniformBuffersMemory[currentImage], 0, sizeof(ubo), 0, &data);
            memcpy(data, &ubo, sizeof(ubo));
        vkUnmapMemory(_device._device, _uniformBuffersMemory[currentImage]);
    }
    
    uint32_t _currentFrame = 0;
    void drawFrame(Swapchain& swapchain, Mesh& onlyMesh) {
        _device._device.waitForFences(_device._inFlightFences[_currentFrame], VK_TRUE, std::numeric_limits<uint64_t>::max());

        uint32_t imageIndex;
        VkResult result = vkAcquireNextImageKHR(_device._device, swapchain._swapChain, std::numeric_limits<uint64_t>::max(), _device._imageAvailableSemaphores[_currentFrame], VK_NULL_HANDLE, &imageIndex);

        if (result == VK_ERROR_OUT_OF_DATE_KHR) {
            //recreateSwapChain();
            return;
        } else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
            throw std::runtime_error("failed to acquire swap chain image!");
        }

        updateUniformBuffer(imageIndex, swapchain, onlyMesh._materialRef->_uniformBuffersMemory);

        vk::SubmitInfo submitInfo = {};

        vk::Semaphore waitSemaphores[] = {_device._imageAvailableSemaphores[_currentFrame]};
        vk::PipelineStageFlags waitStages[] = {vk::PipelineStageFlagBits::eColorAttachmentOutput};
        submitInfo.waitSemaphoreCount = 1;
        submitInfo.setPWaitSemaphores(waitSemaphores);
        submitInfo.commandBufferCount = 1;
        submitInfo.setPWaitDstStageMask(waitStages);
        submitInfo.setPCommandBuffers(&onlyMesh._commandBuffers[imageIndex]);

        vk::Semaphore signalSemaphores[] = {_device._renderFinishedSemaphores[_currentFrame]};
        submitInfo.setPSignalSemaphores(signalSemaphores);

        _device._device.resetFences(_device._inFlightFences[_currentFrame]);

        _device._graphicsQueue.submit(submitInfo, _device._inFlightFences[_currentFrame]);

        vk::PresentInfoKHR presentInfo = {};
        presentInfo.setPWaitSemaphores(signalSemaphores);

        vk::SwapchainKHR swapChains[] = {swapchain._swapChain};
        presentInfo.swapchainCount = 1;
        presentInfo.setPSwapchains(swapChains);

        presentInfo.pImageIndices = &imageIndex;

        vk::Result r = _device._presentQueue.presentKHR(presentInfo);

        // if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || _framebufferResized) {
        //     framebufferResized = false;
        //     //recreateSwapChain();
        // } else 
        if (result != VK_SUCCESS) {
            throw std::runtime_error("failed to present swap chain image!");
        }

        _currentFrame = (_currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
    }
    
    private:
    
};