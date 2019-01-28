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
#include "object3d/pointLight.cpp"

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

    void updateUniformBuffer(uint32_t currentImage, Swapchain& swapchain, Mesh m, Camera cam, std::vector<vk::DeviceMemory>& _uniformBuffersMemory, std::vector<vk::DeviceMemory>& _pointLightsUniformBuffersMemory) {
        static auto startTime = std::chrono::high_resolution_clock::now();

        auto currentTime = std::chrono::high_resolution_clock::now();
        float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

        UniformBufferObject ubo = {};
        m.computeWorldMatrix();
        ubo.model = glm::make_mat4((float*)(m._worldMatrix.m)); //glm::mat4(1.0f);//glm::rotate(glm::mat4(1.0f), time * glm::radians(0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        ubo.view = glm::make_mat4((float*)(cam._viewMatrix.m));  //glm::lookAt(glm::vec3(0.0f, 0.0, 3.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        ubo.proj = glm::perspective(cam.projectionAngleRad, swapchain._swapChainExtent.width / (float) swapchain._swapChainExtent.height, cam.nearClip, cam.farClip);
        ubo.proj[1][1] *= -1;
        ubo.cameraPos = glm::vec3(cam.position.x,cam.position.y,cam.position.z);

        void* data;
        vkMapMemory(_device._device, _uniformBuffersMemory[currentImage], 0, sizeof(ubo), 0, &data);
            memcpy(data, &ubo, sizeof(ubo));
        vkUnmapMemory(_device._device, _uniformBuffersMemory[currentImage]);

        // TODO: create this in app code
        PointLight p;
        PointLightsUniformBufferObject pubo = {};
        pubo.lights[0].position = glm::vec4(sin(time),0,5,0);
        pubo.lights[0].color = glm::vec3(1,1,1);
        pubo.lights[0].radius = 0.0f;
        void* dataLight;
        vkMapMemory(_device._device, _pointLightsUniformBuffersMemory[currentImage], 0, sizeof(pubo), 0, &dataLight);
            memcpy(dataLight, &pubo, sizeof(pubo));
        vkUnmapMemory(_device._device, _pointLightsUniformBuffersMemory[currentImage]);
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
        vk::Result result = _device._presentQueue.presentKHR(presentInfo);

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
    void drawFrame(Swapchain& swapchain, Camera& cam, Mesh& onlyMesh, Mesh& otherMesh, int drawNum = 0) {
        
        updateUniformBuffer(swapchain._currentImageIndex, swapchain, onlyMesh, cam, onlyMesh._materialRef->_uniformBuffersMemory, onlyMesh._materialRef->_pointLightsUniformBuffersMemory);
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
    void createCommandBuffers(Device& device, Swapchain& sc, RenderPass& renderPass, std::vector<Mesh> meshes){
        
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

                for (auto &mesh : meshes){
                    vkCmdBindPipeline(_commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, mesh._materialRef->pipeline._graphicsPipeline);

                    VkBuffer vertexBuffers[] = {mesh._vertexBuffer};
                    VkDeviceSize offsets[] = {0};
                    vkCmdBindVertexBuffers(_commandBuffers[i], 0, 1, vertexBuffers, offsets);

                    vkCmdBindIndexBuffer(_commandBuffers[i], mesh._indexBuffer, 0, VK_INDEX_TYPE_UINT16);

                    _commandBuffers[i].bindDescriptorSets(vk::PipelineBindPoint::eGraphics,mesh._materialRef->pipeline._pipelineLayout,0, mesh._materialRef->descriptorSetLayout._descriptorSets[i], {0});

                    vkCmdDrawIndexed(_commandBuffers[i], static_cast<uint32_t>(mesh._indices.size()), 1, 0, 0, 0);
                }
                

            vkCmdEndRenderPass(_commandBuffers[i]);

            if (vkEndCommandBuffer(_commandBuffers[i]) != VK_SUCCESS) {
                throw std::runtime_error("failed to record command buffer!");
            }
        }
    }
    
    private:
    
};