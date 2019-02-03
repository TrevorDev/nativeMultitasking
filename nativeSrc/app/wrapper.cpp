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

Swapchain swapchain;
RenderPass renderPass;

Material material;
Material otherMaterial;

Shader vertShader;
Shader fragShader;
Pipeline pipeline;

int meshCount = 2;

std::vector<Mesh> meshes = {};

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
    

    // Create layout for single uniform buffer
    VkDescriptorSetLayoutBinding uboLayoutBinding = {};
    uboLayoutBinding.binding = 0;
    uboLayoutBinding.descriptorCount = 1;
    uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    uboLayoutBinding.pImmutableSamplers = nullptr;
    uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

    std::array<VkDescriptorSetLayoutBinding, 1> bindings = {uboLayoutBinding};
    VkDescriptorSetLayoutCreateInfo layoutInfo = {};
    layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
    layoutInfo.pBindings = bindings.data();

    auto _descriptorSetLayout = renderer._device._device.createDescriptorSetLayout(layoutInfo);

    // Load in shaders
    vertShader.init(renderer._device, "shaders/vert.spv", VkShaderStageFlagBits::VK_SHADER_STAGE_VERTEX_BIT);
    fragShader.init(renderer._device, "shaders/frag.spv", VkShaderStageFlagBits::VK_SHADER_STAGE_FRAGMENT_BIT);

    // Creates the pipeline to render color + depth using shaders
    pipeline.init(renderer._device, swapchain._swapChainExtent.width, swapchain._swapChainExtent.height, {vertShader, fragShader}, _descriptorSetLayout, renderPass);

    // Create descriptor set pool
    std::array<VkDescriptorPoolSize, 1> poolSizes = {};
    poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    poolSizes[0].descriptorCount = static_cast<uint32_t>(swapchain._swapChainImages.size());

    VkDescriptorPoolCreateInfo poolInfo = {};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
    poolInfo.pPoolSizes = poolSizes.data();
    poolInfo.maxSets = static_cast<uint32_t>(swapchain._swapChainImages.size()*meshCount);

    auto _descriptorPool = renderer._device._device.createDescriptorPool(poolInfo);

    // Create descriptor set per mesh
    jlog("creating meshes");
    meshes.resize(meshCount);
    for(auto &m : meshes){
      jlog("creating mesh");
      m.init(renderer._device, &material, swapchain);
      jlog("uni");
      m.createUniformBuffer(renderer._device, swapchain._swapChainImages.size());
      jlog("desc");
      m.createDescriptorSet(renderer._device, _descriptorPool,  _descriptorSetLayout, swapchain._swapChainImages.size());
      jlog("d reating mesh");
    }
    renderer.createCommandBuffers(renderer._device, pipeline, swapchain, renderPass, meshes);
    // otherMesh.init(renderer._device, &otherMaterial, swapchain);

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
    meshes[0].position.x = 1;
    meshes[1].position.y = 1;

    renderer.getNextImage(swapchain);
    renderer.drawFrame(swapchain,cam, meshes);
    //onlyMesh.position.x = 0;
    // jlog("second");
    // renderer.drawFrame(swapchain,cam, onlyMesh, 1);
    renderer.presentFrame(swapchain);

    // onlyMesh.position.x = 0;
    // renderer.drawFrame(swapchain,cam, onlyMesh);
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