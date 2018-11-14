#include <napi.h>

#define VK_USE_PLATFORM_WIN32_KHR
#include "j.h"
#include "windowManager.cpp"
#include "vulkanRenderer.cpp"

void run(const Napi::CallbackInfo& info) {
  jlog("Started!");
  try{
    int width = 800;
    int height = 600;
    WindowManager wm = WindowManager(width,height);
    VulkanRenderer renderer;
    renderer.createInstance();
    auto surface = wm.createSurface(renderer._instance);
    renderer.pickPhysicalDevice(surface);
    renderer.createLogicalDevice(surface);
    wm.getFramebufferSize(&width, &height);
    renderer.createSwapChain(surface, width, height);
    renderer.createImageViews();
    renderer.createRenderPass();
    renderer.createDescriptorSetLayout();
    renderer.createGraphicsPipeline();
    renderer.createCommandPool(surface);
    renderer.createDepthResources();
    renderer.createFramebuffers();
    renderer.createTextureImage();
    renderer.createTextureImageView();
    renderer.createTextureSampler();
    renderer.createVertexBuffer();

    renderer.createIndexBuffer();
    renderer.createUniformBuffers();
    renderer.createDescriptorPool();
    renderer.createDescriptorSets();
    renderer.createCommandBuffers();
    renderer.createSyncObjects();

    
    while (!wm.shouldClose()) {
        wm.update();
        renderer.drawFrame();
    }

    //vkDeviceWaitIdle(device);

  }catch (const std::exception& e) {
    jlog("Native code threw an exception:");
    std::cerr << e.what() << std::endl;
    throw;
  }
  

}

Napi::Object Init(Napi::Env env, Napi::Object exports) {
  exports.Set(Napi::String::New(env, "run"), Napi::Function::New(env, run));
  return exports;
}

NODE_API_MODULE(app, Init)