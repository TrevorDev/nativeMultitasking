#pragma once

#include "../j.hpp"
#include "vulkanInc.hpp"
#include "device.hpp"

class Shader {
    public:
    VkShaderModule _shaderModule;
    VkPipelineShaderStageCreateInfo _shaderStageInfo = {};
    Shader(){
        
    }
    void init(Device device, const std::string& filePath, VkShaderStageFlagBits stage){
        auto shaderCode = jreadFile(filePath);

        createShaderModule(device, shaderCode);

        
        _shaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        _shaderStageInfo.stage = stage;
        _shaderStageInfo.module = _shaderModule;
        _shaderStageInfo.pName = "main";
    }
    
    private:
    void createShaderModule(Device device, const std::vector<char>& code) {
        VkShaderModuleCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        createInfo.codeSize = code.size();
        createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());
        if (vkCreateShaderModule(device._device, &createInfo, nullptr, &_shaderModule) != VK_SUCCESS) {
            throw std::runtime_error("failed to create shader module!");
        }
    }
};