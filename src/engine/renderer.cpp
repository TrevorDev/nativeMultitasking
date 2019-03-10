#pragma once
#include "../j.hpp"
#include "vulkanInc.hpp"
#include "../engine/instance.cpp"
#include "../engine/device.cpp"
#include "../engine/swapchain.cpp"
#include "../engine/pipeline.cpp"
#include "../engine/material.cpp"

#include "../engine/glmInc.h"
#include "../object3d/camera.cpp"
#include "../object3d/mesh.cpp"
#include "../object3d/pointLight.cpp"
#include "../object3d/scene.cpp"

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

    void presentFrame(Swapchain& swapchain){
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
    vk::Semaphore signalSemaphores[1];
    void drawFrame(Swapchain& swapchain, Camera& cam, Scene& scene, std::vector<Mesh>& meshes, int drawNum = 0) {
        for(auto &m : meshes){
            m.updateUniformBuffer(_device, swapchain._currentImageIndex);
        }
        scene.updateUniformBuffer(_device, swapchain._currentImageIndex, cam);
        
        //updateUniformBuffer(swapchain._currentImageIndex, swapchain, otherMesh, cam, otherMesh._materialRef->_uniformBuffersMemory, otherMesh._materialRef->_pointLightsUniformBuffersMemory);

        //otherMesh._commandBuffers[swapchain._currentImageIndex], 
        vk::CommandBuffer const commandBuffers[] = {_commandBuffers[swapchain._currentImageIndex]};

        // Submit draw
        vk::SubmitInfo submitInfo = {};
        vk::Semaphore waitSemaphores[] = {_device._imageAvailableSemaphores[swapchain._currentFrame]};
        vk::PipelineStageFlags waitStages[] = {vk::PipelineStageFlagBits::eColorAttachmentOutput};
        submitInfo.waitSemaphoreCount = 1;
        submitInfo.setPWaitSemaphores(waitSemaphores);
        submitInfo.commandBufferCount = 1;
        submitInfo.setPWaitDstStageMask(waitStages);
        submitInfo.setPCommandBuffers(commandBuffers); // This command puffer specifies which framebuffer to render to
        signalSemaphores[0] = _device._renderFinishedSemaphores[swapchain._currentFrame];
        submitInfo.setPSignalSemaphores(signalSemaphores);
        _device._device.resetFences(_device._inFlightFences[swapchain._currentFrame]);
        //jlog("submit");
        _device._graphicsQueue.submit(submitInfo, _device._inFlightFences[swapchain._currentFrame]);
        //jlog("submit done");

        
    }

    std::vector<vk::CommandBuffer> _commandBuffers = {};
    void createCommandBuffers(Device& device, Pipeline& pipeline, Swapchain& sc, RenderPass& renderPass,Scene& scene, std::vector<Mesh> meshes){
        
        _commandBuffers.resize(sc._swapChainImages.size());

        VkCommandBufferAllocateInfo allocInfo = {};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.commandPool = device._commandPool;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandBufferCount = (uint32_t) _commandBuffers.size();

        _commandBuffers = device._device.allocateCommandBuffers(allocInfo);

        for (size_t i = 0; i < _commandBuffers.size(); i++) {
            VkCommandBufferBeginInfo beginInfo = {};
            beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
            beginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;

            if (vkBeginCommandBuffer(_commandBuffers[i], &beginInfo) != VK_SUCCESS) {
                throw std::runtime_error("failed to begin recording command buffer!");
            }
    
            VkRenderPassBeginInfo renderPassInfo = {};
            renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
            renderPassInfo.renderPass = renderPass._renderPass;
            renderPassInfo.framebuffer = sc._swapChainImages[i]._framebuffer;
            renderPassInfo.renderArea.offset = {0, 0};
            renderPassInfo.renderArea.extent = sc._swapChainExtent;

            std::array<VkClearValue, 2> clearValues = {};
            clearValues[0].color = {0.0f, 0.0f, 0.0f, 1.0f};
            clearValues[1].depthStencil = {1.0f, 0};

            renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
            renderPassInfo.pClearValues = clearValues.data();

            vkCmdBeginRenderPass(_commandBuffers[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

                vkCmdBindPipeline(_commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline._graphicsPipeline);
                _commandBuffers[i].bindDescriptorSets(vk::PipelineBindPoint::eGraphics,pipeline._pipelineLayout,0, scene._descriptorSets[i], {0});
                for (auto &mesh : meshes){
                    mesh.draw(_commandBuffers[i], pipeline._pipelineLayout, i);
                }
                

            vkCmdEndRenderPass(_commandBuffers[i]);

            if (vkEndCommandBuffer(_commandBuffers[i]) != VK_SUCCESS) {
                throw std::runtime_error("failed to record command buffer!");
            }
        }
    }
    
    private:
    
};