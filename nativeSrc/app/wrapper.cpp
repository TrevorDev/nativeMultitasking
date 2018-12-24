#include <napi.h>

#define VK_USE_PLATFORM_WIN32_KHR
#include "j.h"
#include "windowManager.cpp"
#include "engine/renderer.cpp"
#include "engine/renderPass.cpp"
#include "engine/swapchain.cpp"

#include "engine/material.cpp"
#include "engine/image.cpp"
#include "object3d/mesh.cpp"
#include "object3d/camera.cpp"
#include "object3d/pointLight.cpp"

Renderer renderer;
vk::SurfaceKHR surface;
WindowManager wm;

Camera cam;
Mesh onlyMesh;

Swapchain swapchain;
RenderPass renderPass;

Material material;

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
    onlyMesh.init(renderer._device, &material, swapchain);

    // Create syncing objects to avoid drawing too quickly
    renderer._device.createSyncObjects();
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

void render(const Napi::CallbackInfo& info) {
  try{
    wm.update();
    if(wm.keys[262]){
      // Right
      cam.position.x += 0.01f;
    }
    if(wm.keys[263]){
      // Left
      cam.position.x -= 0.01f;
    }
    if(wm.keys[264]){
      // Down
      cam.position.z += 0.01f;
    }
    if(wm.keys[265]){
      // Up
      cam.position.z -= 0.01f;
    }
    cam.computeWorldMatrix();
    cam.computeViewMatrix();
    renderer.drawFrame(swapchain,cam, onlyMesh);
  }catch (const std::exception& e) {
    jlog("Native code threw an exception:");
    std::cerr << e.what() << std::endl;
    throw;
  }
}

Napi::Object Init(Napi::Env env, Napi::Object exports) {
  exports.Set(Napi::String::New(env, "init"), Napi::Function::New(env, init));
  exports.Set(Napi::String::New(env, "shouldClose"), Napi::Function::New(env, shouldClose));
  exports.Set(Napi::String::New(env, "render"), Napi::Function::New(env, render));
  return exports;
}

NODE_API_MODULE(app, Init)