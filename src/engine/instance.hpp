#pragma once

#include "../j.hpp"
#include "vulkanInc.hpp"


const std::vector<const char*> validationLayers = {
    "VK_LAYER_LUNARG_standard_validation"
};

VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData) {
    jlog("dbg");
    std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;

    return VK_FALSE;
}

VkDebugUtilsMessengerEXT debugMessenger;

VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger) {
    auto func = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
    if (func != nullptr) {
        return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
    } else {
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }
}

class Instance {
    public:
    vk::Instance _instance;
    Instance(){
    }
    
    void init(std::vector<std::string>& extensions){
        vk::ApplicationInfo appInfo;
        appInfo.pApplicationName = "Hello Triangle";
        appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.pEngineName = "No Engine";
        appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.apiVersion = VK_API_VERSION_1_0;
        vk::InstanceCreateInfo createInfo;
        createInfo.pApplicationInfo = &appInfo;
        
        auto actualExtensions = vk::enumerateInstanceExtensionProperties();

        // Convert passed in extensions into vector<char*> of actual extensions
        std::vector<char*> initExtensions = {};
        for(auto& e : extensions){
            auto found = false;
            for(auto& x : actualExtensions){
                if(strcmp(e.c_str(), x.extensionName) == 0){
                    initExtensions.push_back((char*)(new std::string(x.extensionName))->c_str()); // TODO free this, for some reason i needed to make a copy here
                    found = true;
                    break;
                }
            }
            if(!found){
                jlog("Invalid vulkan instance extension, skipping");
                jlog(e.c_str());
            }
        }
        VK_EXT_debug_report;
        VK_EXT_debug_utils;

        createInfo.enabledExtensionCount = static_cast<uint32_t>(initExtensions.size());
        createInfo.ppEnabledExtensionNames = (const char* const*)initExtensions.data();

        createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
        createInfo.ppEnabledLayerNames = validationLayers.data();

        try {
            _instance = vk::createInstance(createInfo);
        }catch(...){
            jlog("Unable to initialize vulkan");
        }

        VkDebugUtilsMessengerCreateInfoEXT extCreateInfo = {};
        extCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        extCreateInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
        extCreateInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
        extCreateInfo.pfnUserCallback = debugCallback;

        if (CreateDebugUtilsMessengerEXT(_instance, &extCreateInfo, nullptr, &debugMessenger) != VK_SUCCESS) {
            throw std::runtime_error("failed to set up debug messenger!");
        }

        for(auto& str : initExtensions){
            delete str;
        }
    }

    private:
    
};