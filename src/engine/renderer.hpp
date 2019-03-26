#pragma once
#include "../j.hpp"
#include "vulkanInc.hpp"
#include "../engine/instance.hpp"
#include "../engine/device.hpp"
#include "../engine/swapchain.hpp"
#include "../engine/pipeline.hpp"

#include "../engine/glmInc.h"
#include "../object3d/camera.hpp"
#include "../object3d/mesh.hpp"
#include "../object3d/pointLight.hpp"
#include "../object3d/scene.hpp"

class Renderer {
    public:
    Instance _instance;
    Device _device;

    Renderer(){
        
    }

    void initInstance(std::vector<std::string>& instanceExtensions){
        _instance.init(instanceExtensions);
    }

    void initDevice(VkSurfaceKHR surface){
        std::vector<vk::PhysicalDevice> x =  _instance._instance.enumeratePhysicalDevices();
        _device.init(surface, x);
    }
    
    private:
    
};