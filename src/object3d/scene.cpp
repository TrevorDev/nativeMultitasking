#pragma once

#include "../engine/vulkanInc.hpp"
#include "../engine/glmInc.h"
#include "../engine/device.cpp"
#include "../engine/pipeline.cpp"

class Scene {
    public:
    Scene(){
        
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

    void updateUniformBuffer(Device device, uint16_t imageIndex, Camera& cam){
        static auto startTime = std::chrono::high_resolution_clock::now();

        auto currentTime = std::chrono::high_resolution_clock::now();
        float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

        SceneUniformBufferObject ubo = {};
        ubo.view = glm::make_mat4((float*)(cam._viewMatrix.m));  //glm::lookAt(glm::vec3(0.0f, 0.0, 3.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        ubo.proj = glm::perspective(cam.projectionAngleRad, cam.projectionWidth / (float) cam.projectionHeight, cam.nearClip, cam.farClip);
        ubo.proj[1][1] *= -1;
        ubo.cameraPos = glm::vec3(cam.position.x,cam.position.y,cam.position.z);

        void* data;
        vkMapMemory(device._device, _uniformBuffersMemory[imageIndex], 0, sizeof(ubo), 0, &data);
            memcpy(data, &ubo, sizeof(ubo));
        vkUnmapMemory(device._device, _uniformBuffersMemory[imageIndex]);
    }

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

    private:
    
};