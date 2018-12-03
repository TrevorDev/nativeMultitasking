#pragma once
#include "j.h"
#include "vulkanInc.h"
#include "instance.cpp"
#include "device.cpp"

class Renderer {
    public:
    Instance _instance;
    Device _device;

    Renderer(){
        
    }

    void init(std::vector<std::string>& instanceExtensions){
        _instance.init(instanceExtensions);
    }

    void initDevice(VkSurfaceKHR surface){
        _device.init(surface, _instance._instance.enumeratePhysicalDevices());
    }
    
    private:
    
};