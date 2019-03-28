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




class SceneRenderSetup {
    public:
    Shader vertShader;
    Shader fragShader;

    DefaultDescriptorSet sceneDescSet;
    DefaultDescriptorSet meshDescSet;
    Device * device;
    std::vector<Mesh> meshes = {};
    int meshCount = 500;
    Scene scene;
    Camera cam;
    SceneRenderSetup(){
    }
    void init(Device* d, uint32_t maxSwapchainImgCount){
        device = d;

        // Create main descriptor set pools
        sceneDescSet.init(*this->device, maxSwapchainImgCount, 1);
        meshDescSet.init(*this->device, maxSwapchainImgCount, meshCount);

        // Create scene buffers and pool
        scene.createUniformBuffer(*this->device, maxSwapchainImgCount);
        scene.createDescriptorSet(*this->device, sceneDescSet._descriptorPool,  sceneDescSet._descriptorSetLayout, maxSwapchainImgCount);

        // Create descriptor set per mesh
        jlog("creating meshes");
        meshes.resize(meshCount);
        for(auto &m : meshes){
            m.init(*this->device);
            m.createUniformBuffer(*this->device, maxSwapchainImgCount);
            m.createDescriptorSet(*this->device, meshDescSet._descriptorPool,  meshDescSet._descriptorSetLayout, maxSwapchainImgCount);
        }
        int pos = 0;
        for(auto &mesh : meshes){
            mesh.position.x = pos;
            mesh.position.y = pos/100.0;
            mesh.position.z = -pos;
            pos+=1;
        }
        jlog("creating meshes done");
    }
    void createCommandBuffers(){
        
    }

    void createRenderInstanceForImageArray(){

    }
    
    private:
    
};

class SceneRenderInstance {
    public:
    RenderPass renderPass;
    Pipeline pipeline;
    SceneRenderSetup* setup;
    std::vector<Image*> images;
    std::vector<vk::CommandBuffer> _commandBuffers = {};
    Device* device;
    vk::Extent2D dimensions;
    SceneRenderInstance(){
    }
    void init(Device * device, SceneRenderSetup* renderSetup, std::vector<Image>& images, VkFormat imageFormat, Image& depthImage, vk::Extent2D imgDim){
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
        // TODO should recompile shaders be in SceneRenderSetup.init to avoid compiling every time?
        compileShader("shaders/shader.vert", "shaders/vert.spv");
        compileShader("shaders/shader.frag", "shaders/frag.spv");
        renderSetup->vertShader.init(*this->device, "shaders/vert.spv", VkShaderStageFlagBits::VK_SHADER_STAGE_VERTEX_BIT);
        renderSetup->fragShader.init(*this->device, "shaders/frag.spv", VkShaderStageFlagBits::VK_SHADER_STAGE_FRAGMENT_BIT);

        // Creates the pipeline to render color + depth using shaders
        std::vector<Shader> x = {renderSetup->vertShader, renderSetup->fragShader};
         jlog("init pipe");
        pipeline.init((*device), imgDim.width, imgDim.height, x, renderSetup->sceneDescSet._descriptorSetLayout, renderSetup->meshDescSet._descriptorSetLayout, renderPass);
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
        
            _commandBuffers[i].bindDescriptorSets(vk::PipelineBindPoint::eGraphics,pipeline._pipelineLayout,0, this->setup->scene._descriptorSets[i], {0});
        
            for (auto &mesh : this->setup->meshes){
                mesh.draw(_commandBuffers[i], pipeline._pipelineLayout, i);
            }
                
            vkCmdEndRenderPass(_commandBuffers[i]);
            if (vkEndCommandBuffer(_commandBuffers[i]) != VK_SUCCESS) {
                throw std::runtime_error("failed to record command buffer!");
            }
        }
    }

    vk::Semaphore signalSemaphores[1];
    // TODO what is swapchain currentFrame vs swapchain currentImageIndex??
    void render(uint16_t currentImageIndex, uint16_t currentFrame){
        for(auto &m : this->setup->meshes){
            m.updateUniformBuffer((*this->device), currentImageIndex);
        }
        this->setup->scene.updateUniformBuffer((*this->device), currentImageIndex, this->setup->cam);
        
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
