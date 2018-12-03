#pragma once

#include "j.h"
#include "vulkanInc.h"


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
        for(auto e : extensions){
            auto found = false;
            for(auto x : actualExtensions){
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

        createInfo.enabledExtensionCount = static_cast<uint32_t>(initExtensions.size());
        createInfo.ppEnabledExtensionNames = (const char* const*)initExtensions.data();

        createInfo.enabledLayerCount = 0;

        try {
            _instance = vk::createInstance(createInfo);
        }catch(...){
            jlog("Unable to initialize vulkan");
        }
    }

    private:
    
};