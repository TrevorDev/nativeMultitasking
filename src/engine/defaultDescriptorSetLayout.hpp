#pragma once

#include "../j.hpp"
#include "vulkanInc.hpp"
#include "device.hpp"

class DefaultDescriptorSet {
    public:
    DefaultDescriptorSet(){
        
    }
    vk::DescriptorPool _descriptorPool;
    vk::DescriptorSetLayout _descriptorSetLayout;
    void init(Device& device, uint16_t activeInstances, uint16_t maxSets){
        // See https://vulkan-tutorial.com/Uniform_buffers/Descriptor_pool_and_sets for more details
        // Create layout for single uniform buffer
        VkDescriptorSetLayoutBinding uboLayoutBinding = {};
        uboLayoutBinding.binding = 0;
        uboLayoutBinding.descriptorCount = 1;
        uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        uboLayoutBinding.pImmutableSamplers = nullptr;
        uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

        VkDescriptorSetLayoutBinding samplerLayoutBinding = {};
        samplerLayoutBinding.binding = 1;
        samplerLayoutBinding.descriptorCount = 1;
        samplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        samplerLayoutBinding.pImmutableSamplers = nullptr;
        samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

        std::array<VkDescriptorSetLayoutBinding, 2> bindings = {uboLayoutBinding, samplerLayoutBinding};
        VkDescriptorSetLayoutCreateInfo layoutInfo = {};
        layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
        layoutInfo.pBindings = bindings.data();

        _descriptorSetLayout = device._device.createDescriptorSetLayout(layoutInfo);

        /*
        https://www.reddit.com/r/vulkan/comments/8u9zqr/having_trouble_understanding_descriptor_pool/
        So, for instance, if I need to draw 10 objects, each with its own descriptor set and draw call, and between those 10 objects I need 10 uniform buffers, and 20 samplers, then I would need:
            maxSets = 10
            one VkDescriptorPoolSize with type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER and descriptorCount = 10
            one VkDescriptorPoolSize with type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER and descriptorCount = 20
        */

        // Create descriptor set pool
        std::array<VkDescriptorPoolSize, 2> poolSizes = {};
        poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        poolSizes[0].descriptorCount = static_cast<uint32_t>(activeInstances*maxSets); // Number of set active each frame (eg. swapchain size)
        poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        poolSizes[1].descriptorCount = static_cast<uint32_t>(activeInstances*maxSets);

        VkDescriptorPoolCreateInfo poolInfo = {};
        poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
        poolInfo.pPoolSizes = poolSizes.data();
        poolInfo.maxSets = static_cast<uint32_t>(activeInstances*maxSets); // Max amount that can be allocated (eg. 100 meshes * 2 frames)

        _descriptorPool = device._device.createDescriptorPool(poolInfo);
    }
    
    private:
    
};