#include <napi.h>

#define VK_USE_PLATFORM_WIN32_KHR
#include "j.h"
#include "windowManager.cpp"
#include "vulkanRenderer.cpp"
#include "openvrSession.cpp"



WindowManager* wm;
VulkanRenderer ogRenderer;
OpenVRSession vrSession;


vk::SurfaceKHR surface;
std::vector<std::string> intanceExtensions = {
      VK_KHR_EXTERNAL_MEMORY_CAPABILITIES_EXTENSION_NAME,
      VK_KHR_EXTERNAL_SEMAPHORE_CAPABILITIES_EXTENSION_NAME
    };
void init(const Napi::CallbackInfo& info) {
  jlog("Started!");
  try{
    // Init VR
    auto vrEnabled = false;//= vrSession.init();

    // Init window
    int width = 800;
    int height = 600;
    wm = new WindowManager(width,height);

    // Create vulkan instance with extensions from display api's
    // And with external memory for compositing

    wm->getRequiredInstanceExtensions(intanceExtensions);
    if(vrEnabled){
      vrSession.getVulkanInstanceExtensionsRequired(intanceExtensions);
    }
    //ogRenderer.createInstance(intanceExtensions);
    renderer.init(intanceExtensions);

    // Create surface to render to
    surface = wm->createSurface(renderer._instance._instance);
    renderer.initDevice(surface);
    swapchain.init(surface, width, height, renderer._device);
    renderPass.init(renderer._device, swapchain._swapChainImageFormat, renderer._device.findDepthFormat());
    descriptorSetLayout.init(renderer._device);




    // setup rendering images and render pass
    //ogRenderer.createImageViews();
    //ogRenderer.createRenderPass();
    //ogRenderer.createDescriptorSetLayout();
    ogRenderer.createGraphicsPipeline();
    ogRenderer.createCommandPool(surface);
    ogRenderer.createFramebuffers();
    

    // Create image texture
    ogRenderer.createTextureImage();
    ogRenderer.createTextureImageView();
    ogRenderer.createTextureSampler();

    // Create mesh buffers to render texture on to
    ogRenderer.createVertexBuffer();
    ogRenderer.createIndexBuffer();
    ogRenderer.createUniformBuffers();

    // bind buffers for rendering
    ogRenderer.createDescriptorPool();
    ogRenderer.createDescriptorSets();

    // Create command buffers for the renderpass
    ogRenderer.createCommandBuffers();

    // Create fences and semaphores used to wait until gpu is ready
    ogRenderer.createSyncObjects();

    
    // while (!wm->shouldClose()) {
    //     wm->update();
    //     ogRenderer.drawFrame();
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
    ogRenderer.drawFrame();
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