#include <napi.h>

#define VK_USE_PLATFORM_WIN32_KHR
#include "j.h"
#include "windowManager.cpp"
#include "engine/renderer.cpp"
#include "engine/renderPass.cpp"
#include "engine/swapchain.cpp"
#include "engine/descriptorSetLayout.cpp"
#include "engine/shader.cpp"
#include "engine/pipeline.cpp"
#include "engine/image.cpp"
#include "object3d/mesh.cpp"

Renderer renderer;
vk::SurfaceKHR surface;
WindowManager wm;

Mesh onlyMesh;

Swapchain swapchain;
RenderPass renderPass;
DescriptorSetLayout descriptorSetLayout;
Pipeline pipeline;

Shader vertShader;
Shader fragShader;





void init(const Napi::CallbackInfo& info) {
  jlog("Started!");
  try{
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

    descriptorSetLayout.init(renderer._device);

    // Load in shaders
    vertShader.init(renderer._device, "shaders/vert.spv", VkShaderStageFlagBits::VK_SHADER_STAGE_VERTEX_BIT);
    fragShader.init(renderer._device, "shaders/frag.spv", VkShaderStageFlagBits::VK_SHADER_STAGE_FRAGMENT_BIT);

    // Creates the pipeline to render color + depth using shaders
    pipeline.init(renderer._device, swapchain._swapChainExtent.width, swapchain._swapChainExtent.height, {vertShader, fragShader}, descriptorSetLayout, renderPass);

    // Initalize swapcahin images as framebuffers which makes them able to be drawn to
    for(auto& im : swapchain._swapChainImages){
      im.createFrameBuffer(swapchain._depthImage, renderPass, swapchain._swapChainExtent.width, swapchain._swapChainExtent.height);
    }
    
    // TODO uniform buffer should be per descriptor pool/set instead of per mesh
    onlyMesh.init(renderer._device, swapchain._swapChainImages.size());

    // Setup materials
    // TODO move this to material class
    descriptorSetLayout.createDescriptorPool(renderer._device, swapchain._swapChainImages.size());
    descriptorSetLayout.createDescriptorSets(renderer._device, swapchain._swapChainImages.size(), onlyMesh._uniformBuffers);
    
    // Creates command buffer to draw a mesh
    // TODO this should be per mesh
    pipeline.createCommandBuffers(renderer._device, swapchain, renderPass, descriptorSetLayout, onlyMesh._indices.size(), onlyMesh._vertexBuffer, onlyMesh._indexBuffer);
    
    // Create syncing objects to avoid drawing too quickly
    renderer._device.createSyncObjects();
    jlog("Bootsup success");

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
    
    renderer.drawFrame(swapchain, onlyMesh._uniformBuffersMemory, pipeline);
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