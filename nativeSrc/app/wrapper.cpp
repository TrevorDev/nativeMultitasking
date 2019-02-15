#include <napi.h>

#define VK_USE_PLATFORM_WIN32_KHR
#include "j.h"
#include "windowManager.cpp"
#include "engine/renderer.cpp"
#include "engine/renderPass.cpp"
#include "engine/swapchain.cpp"
#include "engine/defaultDescriptorSetLayout.cpp"

#include "engine/material.cpp"
#include "engine/image.cpp"
#include "object3d/mesh.cpp"
#include "object3d/scene.cpp"
#include "object3d/camera.cpp"
#include "object3d/pointLight.cpp"

Renderer renderer;
vk::SurfaceKHR surface;
WindowManager wm;

Camera cam;

Swapchain swapchain;
RenderPass renderPass;

Material material;
Material otherMaterial;

Shader vertShader;
Shader fragShader;
Pipeline pipeline;

DefaultDescriptorSet descSetScene;
DefaultDescriptorSet descSet;

int meshCount = 500;

std::vector<Mesh> meshes = {};
Scene scene;

void init(const Napi::CallbackInfo& info) {
  jlog("Started!");
  try{
    cam.position.z = 3;
    cam.position.y = 0.5f;

    wm.init(800,600);
    // Create vulkan instance with extensions from display api's and with external memory for compositing
    std::vector<std::string> intanceExtensions = {
      VK_KHR_EXTERNAL_MEMORY_CAPABILITIES_EXTENSION_NAME,
      VK_KHR_EXTERNAL_SEMAPHORE_CAPABILITIES_EXTENSION_NAME
    };
    wm.getRequiredInstanceExtensions(intanceExtensions);
    
    renderer.initInstance(intanceExtensions);
    
    // Create surface to render to and initialize a device compatable with that surface
    surface = wm.createSurface(renderer._instance._instance);
    renderer.initDevice(surface);

    // create swapchain and renderpass with color + depth
    swapchain.init(surface, wm.getFramebufferSize().width, wm.getFramebufferSize().height, renderer._device);
    renderPass.init(renderer._device, swapchain._swapChainImageFormat, renderer._device.findDepthFormat());

    // Initalize swapcahin images as framebuffers which makes them able to be drawn to
    for(auto& im : swapchain._swapChainImages){
      im.createFrameBuffer(swapchain._depthImage, renderPass, swapchain._swapChainExtent.width, swapchain._swapChainExtent.height);
    }

    // Create a mesh with a standard material
    // TODO: these shouldnt be dependant on swapchain
    material.init(renderer._device, renderPass, swapchain);
    // otherMaterial.init(renderer._device, renderPass, swapchain);

    descSetScene.init(renderer._device, swapchain._swapChainImages.size(), 1);
    

    descSet.init(renderer._device, swapchain._swapChainImages.size(), meshCount);
  


    // Load in shaders
    vertShader.init(renderer._device, "shaders/vert.spv", VkShaderStageFlagBits::VK_SHADER_STAGE_VERTEX_BIT);
    fragShader.init(renderer._device, "shaders/frag.spv", VkShaderStageFlagBits::VK_SHADER_STAGE_FRAGMENT_BIT);

    // Creates the pipeline to render color + depth using shaders
    pipeline.init(renderer._device, swapchain._swapChainExtent.width, swapchain._swapChainExtent.height, {vertShader, fragShader}, descSetScene._descriptorSetLayout, descSet._descriptorSetLayout, renderPass);

    // Create descriptor set per mesh
    jlog("creating meshes");
    meshes.resize(meshCount);
    for(auto &m : meshes){
      m.init(renderer._device, &material, swapchain);
      m.createUniformBuffer(renderer._device, swapchain._swapChainImages.size());
      m.createDescriptorSet(renderer._device, descSet._descriptorPool,  descSet._descriptorSetLayout, swapchain._swapChainImages.size());
    }
    
    scene.createUniformBuffer(renderer._device, swapchain._swapChainImages.size());
    scene.createDescriptorSet(renderer._device, descSetScene._descriptorPool,  descSetScene._descriptorSetLayout, swapchain._swapChainImages.size());
    
    renderer.createCommandBuffers(renderer._device, pipeline, swapchain, renderPass, scene, meshes);
    jlog("creating meshes done");
    // otherMesh.init(renderer._device, &otherMaterial, swapchain);

    // Create syncing objects to avoid drawing too quickly
    renderer._device.createSyncObjects();

    cam.projectionWidth = swapchain._swapChainExtent.width;
    cam.projectionHeight = swapchain._swapChainExtent.height;
    jlog("Bootup success");

  }catch (const std::exception& e) {
    jlog("Native code threw an exception:");
    std::cerr << e.what() << std::endl;
    throw;
  }
}

Napi::Boolean shouldClose(const Napi::CallbackInfo& info) {
  try{
    Napi::Env env = info.Env();
    return Napi::Boolean::New(env, wm.shouldClose());
  }catch (const std::exception& e) {
    jlog("Native code threw an exception:");
    std::cerr << e.what() << std::endl;
    throw;
  }
}

float camRotX = 0.0f;
float camRotY = 0.0f;
float spd = 0.01f;
void render(const Napi::CallbackInfo& info) {
  try{
    wm.update();
    if(wm.keys[262]){
      // Right
      cam.position.x += spd;
    }
    if(wm.keys[263]){
      // Left
      cam.position.x -= spd;
    }
    if(wm.keys[264]){
      // Down
      cam.position.z += spd;
    }
    if(wm.keys[265]){
      // Up
      cam.position.z -= spd;
    }

    if(wm.mouseDown){
      camRotY += (float)(-wm.lastCursorPosDifX/1000.0);
      camRotX += (float)(-wm.lastCursorPosDifY/1000.0);
    }

    Quaternion::FromEuler(camRotX,camRotY,0, cam.rotation);

    cam.computeWorldMatrix();
    cam.computeViewMatrix();

    int pos = 0;
    for(auto &mesh : meshes){
      mesh.position.x = pos;
      mesh.position.y = pos/100.0;
      mesh.position.z = -pos;
      pos+=1;
    }

    renderer.getNextImage(swapchain);
    renderer.drawFrame(swapchain,cam, scene, meshes);
    renderer.presentFrame(swapchain);

  }catch (const std::exception& e) {
    // jlog("Native code threw an exception:");
    // std::cerr << e.what() << std::endl;
    // throw;
    
    // Cleanup start
    wm.WaitForFrameBuffer();
    jcount();    
    vkDeviceWaitIdle(renderer._device._device);

    renderer._device._device.destroyImageView(swapchain._depthImage._imageView);
    renderer._device._device.destroyImage(swapchain._depthImage._image);
    renderer._device._device.freeMemory(swapchain._depthImage._imageMemory);

    for(auto &img : swapchain._swapChainImages){
      renderer._device._device.destroyFramebuffer(img._framebuffer);
    }


    renderer._device._device.freeCommandBuffers(renderer._device._commandPool, static_cast<uint32_t>(renderer._commandBuffers.size()), renderer._commandBuffers.data());
    
    renderer._commandBuffers = {};

    // TODO missing other distr

    renderer._device._device.destroySwapchainKHR(swapchain._swapChain, nullptr);

    
    

    swapchain = Swapchain();
    

    //cleanup end


    // Create surface to render to and initialize a device compatable with that surface
    //surface = wm.createSurface(renderer._instance._instance);
    //renderer.initDevice(surface);
    jlog("recreate sc");
    // create swapchain and renderpass with color + depth
    swapchain.init(surface, wm.getFramebufferSize().width, wm.getFramebufferSize().height, renderer._device);
    jlog("recreate sc d");
    jcount();
    renderPass.init(renderer._device, swapchain._swapChainImageFormat, renderer._device.findDepthFormat());
    jcount();
    // Initalize swapcahin images as framebuffers which makes them able to be drawn to
    for(auto& im : swapchain._swapChainImages){
      im.createFrameBuffer(swapchain._depthImage, renderPass, swapchain._swapChainExtent.width, swapchain._swapChainExtent.height);
    }
    jcount();
    // Create a mesh with a standard material
    // TODO: these shouldnt be dependant on swapchain
    material.init(renderer._device, renderPass, swapchain);
    // otherMaterial.init(renderer._device, renderPass, swapchain);



    // Load in shaders
    vertShader.init(renderer._device, "shaders/vert.spv", VkShaderStageFlagBits::VK_SHADER_STAGE_VERTEX_BIT);
    fragShader.init(renderer._device, "shaders/frag.spv", VkShaderStageFlagBits::VK_SHADER_STAGE_FRAGMENT_BIT);
    jcount();

    // Creates the pipeline to render color + depth using shaders
    pipeline.init(renderer._device, swapchain._swapChainExtent.width, swapchain._swapChainExtent.height, {vertShader, fragShader}, descSetScene._descriptorSetLayout, descSet._descriptorSetLayout, renderPass);

    jcount();
    
    renderer.createCommandBuffers(renderer._device, pipeline, swapchain, renderPass, scene, meshes);
    jlog("creating meshes done");



  }
}

Napi::Object Init(Napi::Env env, Napi::Object exports) {
  exports.Set(Napi::String::New(env, "init"), Napi::Function::New(env, init));
  exports.Set(Napi::String::New(env, "shouldClose"), Napi::Function::New(env, shouldClose));
  exports.Set(Napi::String::New(env, "render"), Napi::Function::New(env, render));
  return exports;
}

NODE_API_MODULE(app, Init)