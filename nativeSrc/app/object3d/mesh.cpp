#pragma once

#include "j.h"
#include "../engine/vulkanInc.h"
#include "../engine/glmInc.h"
#include "../engine/device.cpp"
#include "../engine/pipeline.cpp"



class Mesh {
    public:

    

    const std::vector<Vertex> _vertices = {
        {{-0.5f, -0.5f, 0.0f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},
        {{0.5f, -0.5f, 0.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},
        {{0.5f, 0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}},
        {{-0.5f, 0.5f, 0.0f}, {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f}},

        {{-0.5f, -0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},
        {{0.5f, -0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},
        {{0.5f, 0.5f, -0.5f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}},
        {{-0.5f, 0.5f, -0.5f}, {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f}}
    };

    const std::vector<uint16_t> _indices = {
        0, 1, 2, 2, 3, 0,
        4, 5, 6, 6, 7, 4
    };

    std::vector<vk::Buffer> _uniformBuffers;
    std::vector<vk::DeviceMemory> _uniformBuffersMemory;
    
    vk::Buffer _vertexBuffer;
    vk::DeviceMemory _vertexBufferMemory;
    vk::Buffer _indexBuffer;
    vk::DeviceMemory _indexBufferMemory;

    Mesh(){
        
    }

    void init(Device device, int swapChainImageCount){
        createVertexBuffer(device);
        createIndexBuffer(device);
        createUniformBuffers(device, swapChainImageCount);
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

    void createUniformBuffers(Device device, int swapChainImageCount) {
        VkDeviceSize bufferSize = sizeof(UniformBufferObject);

        _uniformBuffers.resize(swapChainImageCount);
        _uniformBuffersMemory.resize(swapChainImageCount);

        for (size_t i = 0; i < swapChainImageCount; i++) {
            device.createBuffer(bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, _uniformBuffers[i], _uniformBuffersMemory[i]);
        }
    }

    private:
    
};