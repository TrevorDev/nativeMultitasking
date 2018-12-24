#pragma once

#include "j.h"
#include "vulkanInc.h"
#include "device.cpp"

class DescriptorSetLayout {
    public:
    vk::DescriptorSetLayout _descriptorSetLayout;
    DescriptorSetLayout(){

    }
    void init(Device device){
        // Create descriptor layout which contains a uniform buffer
        VkDescriptorSetLayoutBinding uboLayoutBinding = {};
        uboLayoutBinding.binding = 0;
        uboLayoutBinding.descriptorCount = 1;
        uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        uboLayoutBinding.pImmutableSamplers = nullptr;
        uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

        // point lights
        VkDescriptorSetLayoutBinding pointLightsUboLayoutBinding = {};
        pointLightsUboLayoutBinding.binding = 1;
        pointLightsUboLayoutBinding.descriptorCount = 1;
        pointLightsUboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        pointLightsUboLayoutBinding.pImmutableSamplers = nullptr;
        pointLightsUboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

        // and a single image sampler
        // VkDescriptorSetLayoutBinding samplerLayoutBinding = {};
        // samplerLayoutBinding.binding = 1;
        // samplerLayoutBinding.descriptorCount = 1;
        // samplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        // samplerLayoutBinding.pImmutableSamplers = nullptr;
        // samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

        std::array<VkDescriptorSetLayoutBinding, 2> bindings = {uboLayoutBinding, pointLightsUboLayoutBinding};
        VkDescriptorSetLayoutCreateInfo layoutInfo = {};
        layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
        layoutInfo.pBindings = bindings.data();

        _descriptorSetLayout = device._device.createDescriptorSetLayout(layoutInfo);
    }

    vk::DescriptorPool _descriptorPool;
    std::vector<vk::DescriptorSet> _descriptorSets;
    void createDescriptorPool(Device device, uint32_t swapChainImageCount) {
        std::array<VkDescriptorPoolSize, 2> poolSizes = {};
        poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        poolSizes[0].descriptorCount = static_cast<uint32_t>(swapChainImageCount);
        poolSizes[1].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        poolSizes[1].descriptorCount = static_cast<uint32_t>(swapChainImageCount);
        // poolSizes[2].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        // poolSizes[2].descriptorCount = static_cast<uint32_t>(swapChainImageCount);

        VkDescriptorPoolCreateInfo poolInfo = {};
        poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
        poolInfo.pPoolSizes = poolSizes.data();
        poolInfo.maxSets = static_cast<uint32_t>(swapChainImageCount);

        _descriptorPool = device._device.createDescriptorPool(poolInfo);
    }

    void createDescriptorSets(Device device, uint32_t swapChainImageCount, std::vector<vk::Buffer> _uniformBuffers, std::vector<vk::Buffer> _pointLightsUniformBuffers) {
        std::vector<VkDescriptorSetLayout> layouts(swapChainImageCount, _descriptorSetLayout);
        VkDescriptorSetAllocateInfo allocInfo = {};
        allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        allocInfo.descriptorPool = _descriptorPool;
        allocInfo.descriptorSetCount = static_cast<uint32_t>(swapChainImageCount);
        allocInfo.pSetLayouts = layouts.data();

        _descriptorSets.resize(swapChainImageCount);
        _descriptorSets = device._device.allocateDescriptorSets(allocInfo);

        for (size_t i = 0; i < swapChainImageCount; i++) {
            VkDescriptorBufferInfo bufferInfo = {};
            bufferInfo.buffer = _uniformBuffers[i];
            bufferInfo.offset = 0;
            bufferInfo.range = sizeof(UniformBufferObject);

            VkDescriptorBufferInfo pointLightsBufferInfo = {};
            pointLightsBufferInfo.buffer = _pointLightsUniformBuffers[i];
            pointLightsBufferInfo.offset = 0;
            pointLightsBufferInfo.range = sizeof(PointLightsUniformBufferObject);

            // VkDescriptorImageInfo imageInfo = {};
            // imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            // imageInfo.imageView = _textureImageView;
            // imageInfo.sampler = _textureSampler;

            std::array<VkWriteDescriptorSet, 2> descriptorWrites = {};

            descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptorWrites[0].dstSet = _descriptorSets[i];
            descriptorWrites[0].dstBinding = 0;
            descriptorWrites[0].dstArrayElement = 0;
            descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            descriptorWrites[0].descriptorCount = 1;
            descriptorWrites[0].pBufferInfo = &bufferInfo;

            descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptorWrites[1].dstSet = _descriptorSets[i];
            descriptorWrites[1].dstBinding = 1;
            descriptorWrites[1].dstArrayElement = 0;
            descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            descriptorWrites[1].descriptorCount = 1;
            descriptorWrites[1].pBufferInfo = &pointLightsBufferInfo;

            // descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            // descriptorWrites[1].dstSet = _descriptorSets[i];
            // descriptorWrites[1].dstBinding = 1;
            // descriptorWrites[1].dstArrayElement = 0;
            // descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            // descriptorWrites[1].descriptorCount = 1;
            // descriptorWrites[1].pImageInfo = &imageInfo;

            vkUpdateDescriptorSets(device._device, static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
        }
    }
    
    private:
    
};