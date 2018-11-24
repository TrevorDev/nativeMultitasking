#include <napi.h>

#define VK_USE_PLATFORM_WIN32_KHR
#include "j.h"
#include "windowManager.cpp"
#include "vulkanRenderer.cpp"
#include "openvrSession.cpp"

WindowManager* wm;
VulkanRenderer renderer;
OpenVRSession vrSession;

void init(const Napi::CallbackInfo& info) {
  jlog("Started!");
  try{
    vrSession.init();
    // Init objects
    int width = 800;
    int height = 600;
    wm = new WindowManager(width,height);
    renderer.createInstance();

    // Create surface to render to
    auto surface = wm->createSurface(renderer._instance);

    // Create device to render with
    renderer.pickPhysicalDevice(surface);
    renderer.createLogicalDevice(surface);

    // Create swapchain to render with
    wm->getFramebufferSize(&width, &height);
    renderer.createSwapChain(surface, width, height);

    // setup rendering images and render pass
    renderer.createImageViews();
    renderer.createRenderPass();
    renderer.createDescriptorSetLayout();
    renderer.createGraphicsPipeline();
    renderer.createCommandPool(surface);
    renderer.createDepthResources();
    renderer.createFramebuffers();

    // Create image texture
    renderer.createTextureImage();
    renderer.createTextureImageView();
    renderer.createTextureSampler();

    // Create mesh buffers to render texture on to
    renderer.createVertexBuffer();
    renderer.createIndexBuffer();
    renderer.createUniformBuffers();

    // bind buffers for rendering
    renderer.createDescriptorPool();
    renderer.createDescriptorSets();

    // Create command buffers for the renderpass
    renderer.createCommandBuffers();

    // Create fences and semaphores used to wait until gpu is ready
    renderer.createSyncObjects();

    
    // while (!wm->shouldClose()) {
    //     wm->update();
    //     renderer.drawFrame();
    // }

    //vkDeviceWaitIdle(device);

  }catch (const std::exception& e) {
    jlog("Native code threw an exception:");
    std::cerr << e.what() << std::endl;
    throw;
  }
}

Napi::Boolean shouldClose(const Napi::CallbackInfo& info) {
  try{
    Napi::Env env = info.Env();
    return Napi::Boolean::New(env, wm->shouldClose());
  }catch (const std::exception& e) {
    jlog("Native code threw an exception:");
    std::cerr << e.what() << std::endl;
    throw;
  }
}

void render(const Napi::CallbackInfo& info) {
  try{
    wm->update();
    renderer.drawFrame();
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