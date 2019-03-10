#pragma once

#include "../j.hpp"
#include "../engine/vulkanInc.hpp"
#include "../engine/glmInc.h"
#include "../engine/device.cpp"
#include "../engine/pipeline.cpp"


class Mesh : public Node  {
    public:

    const std::vector<Vertex> _vertices = {
        // Lookup done in shader, layout (1,2,3)
        // Possition           // Color             // Normal           // Texture coord
        {{-0.5f, -0.5f, 0.0f}, {0.05f, 0.8f, 0.8f}, {0.0f, 0.0f, 1.0f}, {1.0f, 0.0f}},
        {{0.5f, -0.5f, 0.0f}, {0.05f, 0.8f, 0.8f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f}},
        {{0.5f, 0.5f, 0.0f}, {0.05f, 0.8f, 0.8f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}},
        {{-0.5f, 0.5f, 0.0f}, {0.05f, 0.8f, 0.8f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}}
    };

    const std::vector<uint16_t> _indices = {
        0, 1, 2, 2, 3, 0
    };

    
    
    vk::Buffer _vertexBuffer;
    vk::DeviceMemory _vertexBufferMemory;
    vk::Buffer _indexBuffer;
    vk::DeviceMemory _indexBufferMemory;

    Material* _materialRef;
    Mesh(){
        
    }

    std::vector<vk::CommandBuffer> _commandBuffers = {};
    void init(Device& device, Material* material){
        createVertexBuffer(device);
        createIndexBuffer(device);
        
        _materialRef = material;
    }
    
    void createVertexBuffer(Device device) {
        VkDeviceSize bufferSize = sizeof(_vertices[0]) * _vertices.size();

        vk::Buffer stagingBuffer;
        vk::DeviceMemory stagingBufferMemory;
        device.createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

        void* data;
        vkMapMemory(device._device, stagingBufferMemory, 0, bufferSize, 0, &data);
            memcpy(data, _vertices.data(), (size_t) bufferSize);
        vkUnmapMemory(device._device, stagingBufferMemory);

        device.createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, _vertexBuffer, _vertexBufferMemory);

        device.copyBuffer(stagingBuffer, _vertexBuffer, bufferSize);

        vkDestroyBuffer(device._device, stagingBuffer, nullptr);
        vkFreeMemory(device._device, stagingBufferMemory, nullptr);
    }

    void createIndexBuffer(Device device) {
        VkDeviceSize bufferSize = sizeof(_indices[0]) * _indices.size();

        vk::Buffer stagingBuffer;
        vk::DeviceMemory stagingBufferMemory;
        device.createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

        void* data;
        vkMapMemory(device._device, stagingBufferMemory, 0, bufferSize, 0, &data);
            memcpy(data, _indices.data(), (size_t) bufferSize);
        vkUnmapMemory(device._device, stagingBufferMemory);

        device.createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, _indexBuffer, _indexBufferMemory);

        device.copyBuffer(stagingBuffer, _indexBuffer, bufferSize);

        vkDestroyBuffer(device._device, stagingBuffer, nullptr);
        vkFreeMemory(device._device, stagingBufferMemory, nullptr);
    }

    std::vector<vk::Buffer> _uniformBuffers;
    std::vector<vk::DeviceMemory> _uniformBuffersMemory;
    void createUniformBuffer(Device device, uint16_t swapChainImageCount){
        VkDeviceSize bufferSize = sizeof(UniformBufferObject);

        _uniformBuffers.resize(swapChainImageCount);
        _uniformBuffersMemory.resize(swapChainImageCount);

        for (size_t i = 0; i < swapChainImageCount; i++) {
            device.createBuffer(bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, _uniformBuffers[i], _uniformBuffersMemory[i]);
        }
    }

    void updateUniformBuffer(Device device, uint16_t imageIndex){
         static auto startTime = std::chrono::high_resolution_clock::now();

        auto currentTime = std::chrono::high_resolution_clock::now();
        float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

        UniformBufferObject ubo = {};
        this->computeWorldMatrix();
        ubo.model = glm::make_mat4((float*)(this->_worldMatrix.m)); //glm::mat4(1.0f);//glm::rotate(glm::mat4(1.0f), time * glm::radians(0.0f), glm::vec3(0.0f, 0.0f, 1.0f));

        void* data;
        vkMapMemory(device._device, _uniformBuffersMemory[imageIndex], 0, sizeof(ubo), 0, &data);
            memcpy(data, &ubo, sizeof(ubo));
        vkUnmapMemory(device._device, _uniformBuffersMemory[imageIndex]);

        // TODO: create this in app code
        // PointLight p;
        // PointLightsUniformBufferObject pubo = {};
        // pubo.lights[0].position = glm::vec4(sin(time),0,5,0);
        // pubo.lights[0].color = glm::vec3(1,1,1);
        // pubo.lights[0].radius = 0.0f;
        // void* dataLight;
        // vkMapMemory(device._device, _pointLightsUniformBuffersMemory[imageIndex], 0, sizeof(pubo), 0, &dataLight);
        //     memcpy(dataLight, &pubo, sizeof(pubo));
        // vkUnmapMemory(device._device, _pointLightsUniformBuffersMemory[imageIndex]);
    }

    std::vector<vk::DescriptorSet> _descriptorSets;
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
            bufferInfo.range = sizeof(UniformBufferObject);

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

    void draw(vk::CommandBuffer cmdbuffer, VkPipelineLayout pipelineLayout, uint16_t i){
        // VkDeviceSize offsets[1] = { 0 };
        // auto x = (VkBuffer)(_vertexBuffer);
        //this->_materialRef->
        // vkCmdBindDescriptorSets(cmdbuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, &descriptorSet, 0, NULL);
        
        // vkCmdBindVertexBuffers(cmdbuffer, 0, 1, &x, offsets);
        // vkCmdBindIndexBuffer(cmdbuffer, _indexBuffer, 0, VK_INDEX_TYPE_UINT32);
        // vkCmdDrawIndexed(cmdbuffer, indexCount, 1, 0, 0, 1);


        VkBuffer vertexBuffers[] = {_vertexBuffer};
        VkDeviceSize offsets[] = {0};
        vkCmdBindVertexBuffers(cmdbuffer, 0, 1, vertexBuffers, offsets);

        vkCmdBindIndexBuffer(cmdbuffer, _indexBuffer, 0, VK_INDEX_TYPE_UINT16);

        cmdbuffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics,pipelineLayout,1, _descriptorSets[i], {0});

        vkCmdDrawIndexed(cmdbuffer, static_cast<uint32_t>(_indices.size()), 1, 0, 0, 0);
    }

    private:
    
};