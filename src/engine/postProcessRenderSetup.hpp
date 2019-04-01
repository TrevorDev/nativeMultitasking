#pragma once

#include "../j.hpp"
#include "vulkanInc.hpp"
#include "device.hpp"
#include "shader.hpp"
#include "defaultDescriptorSetLayout.hpp"
#include "shaderCompile.hpp"
#include "../object3d/scene.hpp"
#include "../object3d/mesh.hpp"
#include "image.hpp"




class PostProcessRenderSetup {
    public:
    Shader vertShader;
    Shader fragShader;
    DefaultDescriptorSet postProcessDescSet;
    Device * device;

    PostProcessRenderSetup(){
    }
    void init(Device* d, uint32_t maxSwapchainImgCount){
        device = d;

        // Create main descriptor set pools
        postProcessDescSet.init(*this->device, maxSwapchainImgCount, 1);

        this->createUniformBuffer(*d, maxSwapchainImgCount);
        this->createDescriptorSet(*d, postProcessDescSet._descriptorPool, postProcessDescSet._descriptorSetLayout, maxSwapchainImgCount);
    }

    std::vector<vk::Buffer> _uniformBuffers;
    std::vector<vk::DeviceMemory> _uniformBuffersMemory;
    std::vector<vk::DescriptorSet> _descriptorSets;

    void createUniformBuffer(Device device, uint16_t swapChainImageCount){
        VkDeviceSize bufferSize = sizeof(SceneUniformBufferObject);

        _uniformBuffers.resize(swapChainImageCount);
        _uniformBuffersMemory.resize(swapChainImageCount);

        for (size_t i = 0; i < swapChainImageCount; i++) {
            device.createBuffer(bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, _uniformBuffers[i], _uniformBuffersMemory[i]);
        }
    }

    // void updateUniformBuffer(Device device, uint16_t imageIndex, Camera& cam){
    //     // static auto startTime = std::chrono::high_resolution_clock::now();

    //     // auto currentTime = std::chrono::high_resolution_clock::now();
    //     //float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

    //     SceneUniformBufferObject ubo = {};
    //     ubo.view = glm::make_mat4((float*)(cam._viewMatrix.m));  //glm::lookAt(glm::vec3(0.0f, 0.0, 3.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    //     ubo.proj = glm::perspective(cam.projectionAngleRad, cam.projectionWidth / (float) cam.projectionHeight, cam.nearClip, cam.farClip);
    //     ubo.proj[1][1] *= -1;
    //     ubo.cameraPos = glm::vec3(cam.position.x,cam.position.y,cam.position.z);

    //     void* data;
    //     vkMapMemory(device._device, _uniformBuffersMemory[imageIndex], 0, sizeof(ubo), 0, &data);
    //         memcpy(data, &ubo, sizeof(ubo));
    //     vkUnmapMemory(device._device, _uniformBuffersMemory[imageIndex]);
    // }

    void createDescriptorSet(Device device, vk::DescriptorPool pool, vk::DescriptorSetLayout layout, uint16_t swapChainImageCount){
        std::vector<VkDescriptorSetLayout> layouts(swapChainImageCount, layout);
        VkDescriptorSetAllocateInfo allocInfo = {};
        allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        allocInfo.descriptorPool = pool;
        allocInfo.descriptorSetCount = static_cast<uint32_t>(swapChainImageCount);
        allocInfo.pSetLayouts = layouts.data();

        _descriptorSets.resize(swapChainImageCount);
        _descriptorSets = device._device.allocateDescriptorSets(allocInfo);

        for (size_t i = 0; i < swapChainImageCount; i++) {
            VkDescriptorBufferInfo bufferInfo = {};
            bufferInfo.buffer = _uniformBuffers[i];
            bufferInfo.offset = 0;
            bufferInfo.range = sizeof(SceneUniformBufferObject);

            std::array<VkWriteDescriptorSet, 1> descriptorWrites = {};

            descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptorWrites[0].dstSet = _descriptorSets[i];
            descriptorWrites[0].dstBinding = 0;
            descriptorWrites[0].dstArrayElement = 0;
            descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            descriptorWrites[0].descriptorCount = 1;
            descriptorWrites[0].pBufferInfo = &bufferInfo;

            vkUpdateDescriptorSets(device._device, static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
        }
    }







    void createCommandBuffers(){
        
    }

    void createRenderInstanceForImageArray(){

    }
    
    private:
    
};

class PostProcessRenderInstance {
    public:
    RenderPass renderPass;
    Pipeline pipeline;
    PostProcessRenderSetup* setup;
    std::vector<Image*> images;
    std::vector<vk::CommandBuffer> _commandBuffers = {};
    Device* device;
    vk::Extent2D dimensions;
    PostProcessRenderInstance(){
    }
    void init(Device * device, PostProcessRenderSetup* renderSetup, std::vector<Image>& images, VkFormat imageFormat, Image& depthImage, vk::Extent2D imgDim){
        this->dimensions = imgDim;
        this->device = device;
        this->setup = renderSetup;
       // this->images = {};
        for(auto& im : images){
            auto a = &im;
            this->images.push_back(a);
        }

        renderPass.init((*device), imageFormat, (*device).findDepthFormat());

        // Initalize swapcahin images as framebuffers which makes them able to be drawn to
        for(auto& im : images){
            im.createFrameBuffer(depthImage, renderPass, imgDim.width, imgDim.height);
        }

        // compile and load shaders
        // TODO should recompile shaders be in PostProcessRenderSetup.init to avoid compiling every time?
        compileShader("shaders/ppShader.vert", "shaders/ppvert.spv");
        compileShader("shaders/ppShader.frag", "shaders/ppfrag.spv");
        renderSetup->vertShader.init(*this->device, "shaders/ppvert.spv", VkShaderStageFlagBits::VK_SHADER_STAGE_VERTEX_BIT);
        renderSetup->fragShader.init(*this->device, "shaders/ppfrag.spv", VkShaderStageFlagBits::VK_SHADER_STAGE_FRAGMENT_BIT);

        // Creates the pipeline to render color + depth using shaders
        std::vector<Shader> x = {renderSetup->vertShader, renderSetup->fragShader};
         jlog("init pipe");
        pipeline.init((*device), imgDim.width, imgDim.height, x, {&renderSetup->postProcessDescSet._descriptorSetLayout}, renderPass);
        jlog("create cmd");
        this->createCommandBuffer();
         jlog("cmd created");
    }
    void createCommandBuffer(){
        _commandBuffers.resize(images.size());

    
        VkCommandBufferAllocateInfo allocInfo = {};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.commandPool = device->_commandPool;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandBufferCount = (uint32_t) _commandBuffers.size();

        _commandBuffers = device->_device.allocateCommandBuffers(allocInfo);
    
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
            renderPassInfo.framebuffer = images[i]->_framebuffer;
            renderPassInfo.renderArea.offset = {0, 0};
            renderPassInfo.renderArea.extent = this->dimensions;

            std::array<VkClearValue, 2> clearValues = {};
            clearValues[0].color = {{0.0f, 0.0f, 0.0f, 1.0f}};
            clearValues[1].depthStencil = {1.0f, 0};

            renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
            renderPassInfo.pClearValues = clearValues.data();
            vkCmdBeginRenderPass(_commandBuffers[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
            vkCmdBindPipeline(_commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline._graphicsPipeline);
        
            _commandBuffers[i].bindDescriptorSets(vk::PipelineBindPoint::eGraphics,pipeline._pipelineLayout,0, this->setup->_descriptorSets[i], {0});
        
            vkCmdDraw(_commandBuffers[i], 3, 1, 0, 0);
                
            vkCmdEndRenderPass(_commandBuffers[i]);
            if (vkEndCommandBuffer(_commandBuffers[i]) != VK_SUCCESS) {
                throw std::runtime_error("failed to record command buffer!");
            }
        }
    }

    vk::Semaphore signalSemaphores[1];
    // TODO what is swapchain currentFrame vs swapchain currentImageIndex??
    void render(uint16_t currentImageIndex, uint16_t currentFrame){
        // for(auto &m : this->setup->meshes){
        //     m.updateUniformBuffer((*this->device), currentImageIndex);
        // }
        // this->setup->scene.updateUniformBuffer((*this->device), currentImageIndex, this->setup->cam);
        
        //updateUniformBuffer(currentImageIndex, swapchain, otherMesh, cam, otherMesh._materialRef->_uniformBuffersMemory, otherMesh._materialRef->_pointLightsUniformBuffersMemory);

        //otherMesh._commandBuffers[currentImageIndex], 
        vk::CommandBuffer const commandBuffers[] = {_commandBuffers[currentImageIndex]};

        // Submit draw
        vk::SubmitInfo submitInfo = {};
        vk::Semaphore waitSemaphores[] = {this->device->_imageAvailableSemaphores[currentFrame]};
        vk::PipelineStageFlags waitStages[] = {vk::PipelineStageFlagBits::eColorAttachmentOutput};
        submitInfo.waitSemaphoreCount = 1;
        submitInfo.setPWaitSemaphores(waitSemaphores);
        submitInfo.commandBufferCount = 1;
        submitInfo.setPWaitDstStageMask(waitStages);
        submitInfo.setPCommandBuffers(commandBuffers); // This command puffer specifies which framebuffer to render to
        signalSemaphores[0] = this->device->_renderFinishedSemaphores[currentFrame];
        submitInfo.setPSignalSemaphores(signalSemaphores);
        this->device->_device.resetFences(this->device->_inFlightFences[currentFrame]);
        //jlog("submit");
        this->device->_graphicsQueue.submit(submitInfo, this->device->_inFlightFences[currentFrame]);
        //jlog("submit done");
    }
    void dispose(){}
};
