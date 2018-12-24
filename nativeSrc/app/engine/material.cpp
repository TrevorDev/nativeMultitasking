#pragma once

#include "j.h"
#include "vulkanInc.h"

#include "engine/descriptorSetLayout.cpp"
#include "engine/shader.cpp"
#include "engine/pipeline.cpp"
#include "engine/device.cpp"

class Material {
    public:

    Shader vertShader;
    Shader fragShader;
    DescriptorSetLayout descriptorSetLayout;
    Pipeline pipeline;
    RenderPass* renderPass;

    std::vector<vk::Buffer> _uniformBuffers;
    std::vector<vk::DeviceMemory> _uniformBuffersMemory;

    std::vector<vk::Buffer> _pointLightsUniformBuffers;
    std::vector<vk::DeviceMemory> _pointLightsUniformBuffersMemory;

    Material(){
        
    }

    void init(Device& device, RenderPass& renderPass, Swapchain& swapchain){
        this->renderPass = &renderPass;
        descriptorSetLayout.init(device);

        // Load in shaders
        vertShader.init(device, "shaders/vert.spv", VkShaderStageFlagBits::VK_SHADER_STAGE_VERTEX_BIT);
        fragShader.init(device, "shaders/frag.spv", VkShaderStageFlagBits::VK_SHADER_STAGE_FRAGMENT_BIT);

        // Creates the pipeline to render color + depth using shaders
        pipeline.init(device, swapchain._swapChainExtent.width, swapchain._swapChainExtent.height, {vertShader, fragShader}, descriptorSetLayout, renderPass);

        
        createUniformBuffers(device, swapchain._swapChainImages.size());

        // Setup materials
        // TODO move this to material class
        descriptorSetLayout.createDescriptorPool(device, swapchain._swapChainImages.size());
        
        descriptorSetLayout.createDescriptorSets(device, swapchain._swapChainImages.size(), _uniformBuffers, _pointLightsUniformBuffers);
        
        
    }
    

    void createUniformBuffers(Device& device, uint32_t swapChainImageCount) {
        VkDeviceSize bufferSize = sizeof(UniformBufferObject);

        _uniformBuffers.resize(swapChainImageCount);
        _uniformBuffersMemory.resize(swapChainImageCount);

        for (size_t i = 0; i < swapChainImageCount; i++) {
            device.createBuffer(bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, _uniformBuffers[i], _uniformBuffersMemory[i]);
        }

        // Point lights
        VkDeviceSize lightBufferSize = sizeof(PointLightsUniformBufferObject);

        _pointLightsUniformBuffers.resize(swapChainImageCount);
        _pointLightsUniformBuffersMemory.resize(swapChainImageCount);

        for (size_t i = 0; i < swapChainImageCount; i++) {
            device.createBuffer(lightBufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, _pointLightsUniformBuffers[i], _pointLightsUniformBuffersMemory[i]);
        }
    }

    
    std::vector<vk::CommandBuffer> createCommandBuffers(Device& device, Swapchain& sc, uint32_t _indicesSize, vk::Buffer& _vertexBuffer, vk::Buffer& _indexBuffer) {
        std::vector<vk::CommandBuffer> commandBuffers = {};
        commandBuffers.resize(sc._swapChainImages.size());

        VkCommandBufferAllocateInfo allocInfo = {};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.commandPool = device._commandPool;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandBufferCount = (uint32_t) commandBuffers.size();

        commandBuffers = device._device.allocateCommandBuffers(allocInfo);

        for (size_t i = 0; i < commandBuffers.size(); i++) {
            VkCommandBufferBeginInfo beginInfo = {};
            beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
            beginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;

            if (vkBeginCommandBuffer(commandBuffers[i], &beginInfo) != VK_SUCCESS) {
                throw std::runtime_error("failed to begin recording command buffer!");
            }

            VkRenderPassBeginInfo renderPassInfo = {};
            renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
            renderPassInfo.renderPass = renderPass->_renderPass;
            renderPassInfo.framebuffer = sc._swapChainImages[i]._framebuffer;
            renderPassInfo.renderArea.offset = {0, 0};
            renderPassInfo.renderArea.extent = sc._swapChainExtent;

            std::array<VkClearValue, 2> clearValues = {};
            clearValues[0].color = {0.0f, 0.0f, 0.0f, 1.0f};
            clearValues[1].depthStencil = {1.0f, 0};

            renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
            renderPassInfo.pClearValues = clearValues.data();

            vkCmdBeginRenderPass(commandBuffers[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

                vkCmdBindPipeline(commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline._graphicsPipeline);

                VkBuffer vertexBuffers[] = {_vertexBuffer};
                VkDeviceSize offsets[] = {0};
                vkCmdBindVertexBuffers(commandBuffers[i], 0, 1, vertexBuffers, offsets);

                vkCmdBindIndexBuffer(commandBuffers[i], _indexBuffer, 0, VK_INDEX_TYPE_UINT16);

                commandBuffers[i].bindDescriptorSets(vk::PipelineBindPoint::eGraphics,pipeline._pipelineLayout,0, descriptorSetLayout._descriptorSets[i], {0});

                vkCmdDrawIndexed(commandBuffers[i], static_cast<uint32_t>(_indicesSize), 1, 0, 0, 0);

            vkCmdEndRenderPass(commandBuffers[i]);

            if (vkEndCommandBuffer(commandBuffers[i]) != VK_SUCCESS) {
                throw std::runtime_error("failed to record command buffer!");
            }
        }
        return commandBuffers;
    }
    private:
    
};