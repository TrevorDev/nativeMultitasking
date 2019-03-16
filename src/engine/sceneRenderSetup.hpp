#pragma once

#include "../j.hpp"
#include "vulkanInc.hpp"
#include "device.hpp"

class SceneRenderInstance {
    public:
    SceneRenderInstance(){
    }
    void render(){}
    void dispose(){}
};

class SceneRenderSetup {
    public:
    Device * device;
    SceneRenderSetup(){
    }
    void init(Device* d){
        device = d;
    }
    void createCommandBuffers(){
        
    }

    void createRenderInstanceForImage(){

    }
    
    private:
    
};

