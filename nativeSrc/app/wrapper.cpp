#include <napi.h>

#define VK_USE_PLATFORM_WIN32_KHR
#include "j.h"
#include "windowManager.cpp"
#include "openvrSession.cpp"
#include "engine/renderer.cpp"
#include "engine/renderPass.cpp"
#include "engine/swapchain.cpp"
#include "engine/descriptorSetLayout.cpp"
#include "engine/shader.cpp"
#include "engine/pipeline.cpp"
#include "engine/image.cpp"
#include "object3d/mesh.cpp"

Mesh m;
WindowManager* wm;
OpenVRSession vrSession;
Renderer renderer;
Swapchain swapchain;
RenderPass renderPass;
DescriptorSetLayout descriptorSetLayout;
Pipeline pipeline;

Shader vertShader;
Shader fragShader;

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
    renderer.init(intanceExtensions);

    // Create surface to render to
    surface = wm->createSurface(renderer._instance._instance);
    renderer.initDevice(surface);
    swapchain.init(surface, width, height, renderer._device);
    renderPass.init(renderer._device, swapchain._swapChainImageFormat, renderer._device.findDepthFormat());
    descriptorSetLayout.init(renderer._device);

    vertShader.init(renderer._device, "shaders/vert.spv", VkShaderStageFlagBits::VK_SHADER_STAGE_VERTEX_BIT);
    fragShader.init(renderer._device, "shaders/frag.spv", VkShaderStageFlagBits::VK_SHADER_STAGE_FRAGMENT_BIT);

    pipeline.init(renderer._device, swapchain, {vertShader, fragShader}, descriptorSetLayout, renderPass);

    for(auto& im : swapchain._swapChainImages){
      im.createFrameBuffer(swapchain.depthImage, renderPass, swapchain._swapChainExtent.width, swapchain._swapChainExtent.height);
      jlog("done img");
    }
    
    
    m.init(renderer._device, swapchain._swapChainImages.size());

    descriptorSetLayout.createDescriptorPool(renderer._device, swapchain._swapChainImages.size());
    descriptorSetLayout.createDescriptorSets(renderer._device, swapchain._swapChainImages.size(), m._uniformBuffers);

    
    pipeline.createCommandBuffers(renderer._device, swapchain, renderPass, descriptorSetLayout, m._indices.size(), m._vertexBuffer, m._indexBuffer);
    renderer._device.createSyncObjects();

    
    jlog("success");
    // Create device to render with
    // auto devices = renderer._instance.enumeratePhysicalDevices();
    // if(vrEnabled){
    //   // getting devices is broken in openVR, fallback to first device
    //   auto device = vrSession.getDesiredVulkanDevice(renderer._instance);
    //   if(device != nullptr){
    //     devices.clear();
    //     devices.push_back(device);
    //     jlog("openVR GetOutputDevice provided recommended device");
    //   }else{
    //     jlog("openVR GetOutputDevice unexpectedly returned null, falling back to first device");
    //   }
    // }
    // renderer.pickPhysicalDevice(surface, devices);
    // renderer.createLogicalDevice(surface);

    // // Create swapchain to render with
    // wm->getFramebufferSize(&width, &height);
    // renderer.createSwapChain(surface, width, height);

    // // setup rendering images and render pass
    // renderer.createImageViews();
    // renderer.createRenderPass();
    // renderer.createDescriptorSetLayout();
    // renderer.createGraphicsPipeline();
    // renderer.createCommandPool(surface);
    // renderer.createDepthResources();
    // renderer.createFramebuffers();

    // // Create image texture
    // renderer.createTextureImage();
    // renderer.createTextureImageView();
    // renderer.createTextureSampler();

    // // Create mesh buffers to render texture on to
    // renderer.createVertexBuffer();
    // renderer.createIndexBuffer();
    // renderer.createUniformBuffers();

    // // bind buffers for rendering
    // renderer.createDescriptorPool();
    // renderer.createDescriptorSets();

    // // Create command buffers for the renderpass
    // renderer.createCommandBuffers();

    // // Create fences and semaphores used to wait until gpu is ready
    // renderer.createSyncObjects();

    
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
    
    renderer.drawFrame(swapchain, m._uniformBuffersMemory, pipeline);
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