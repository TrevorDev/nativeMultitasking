#include "src/engine/vulkanInc.hpp"

#include <GLFW/glfw3.h>
#include <sstream> 

#include "src/j.hpp"
#include "src/windowManager.hpp"
#include "src/engine/vulkanInc.hpp"
#include "src/engine/renderer.hpp"
#include "src/engine/renderPass.hpp"
#include "src/engine/swapchain.hpp"
#include "src/engine/defaultDescriptorSetLayout.hpp"
#include "src/engine/shaderCompile.hpp"

#include "src/engine/sceneRenderSetup.hpp"
#include "src/engine/image.hpp"
#include "src/object3d/mesh.hpp"
#include "src/object3d/scene.hpp"
#include "src/object3d/camera.hpp"
#include "src/object3d/pointLight.hpp"

#include "src/object3d/freeCameraInput.hpp"

Renderer renderer;
vk::SurfaceKHR surface;
WindowManager wm;

Camera cam;

Swapchain swapchain;
RenderPass renderPass;

Shader vertShader;
Shader fragShader;
Pipeline pipeline;

DefaultDescriptorSet sceneDescSet;
DefaultDescriptorSet meshDescSet;

SceneRenderSetup sceneRenderSetup;

int meshCount = 500;
uint32_t maxSwapchainImgCount = 2;

std::vector<Mesh> meshes = {};
Scene scene;

void createSwapchain(){
  jlog("Creating swapchain");
  swapchain = Swapchain();
  

  // create swapchain and renderpass with color + depth
  swapchain.init(surface, wm.getFramebufferSize().width, wm.getFramebufferSize().height, renderer._device, maxSwapchainImgCount);
  renderPass.init(renderer._device, swapchain._swapChainImageFormat, renderer._device.findDepthFormat());

  // Initalize swapcahin images as framebuffers which makes them able to be drawn to
  for(auto& im : swapchain._swapChainImages){
    im.createFrameBuffer(swapchain._depthImage, renderPass, swapchain._swapChainExtent.width, swapchain._swapChainExtent.height);
  }

  // Creates the pipeline to render color + depth using shaders
  pipeline.init(renderer._device, swapchain._swapChainExtent.width, swapchain._swapChainExtent.height, {vertShader, fragShader}, sceneDescSet._descriptorSetLayout, meshDescSet._descriptorSetLayout, renderPass);
  renderer.createCommandBuffers(renderer._device, pipeline, swapchain, renderPass, scene, meshes);
  
  // set camera projection to match output
  cam.projectionWidth = swapchain._swapChainExtent.width;
  cam.projectionHeight = swapchain._swapChainExtent.height;
}

void cleanup(){
  // Cleanup start
  wm.waitForFrameBuffer();
  vkDeviceWaitIdle(renderer._device._device);

  // Reset swapchain
  swapchain.cleanup(renderer._device);
  renderer._device._device.freeCommandBuffers(renderer._device._commandPool, static_cast<uint32_t>(renderer._commandBuffers.size()), renderer._commandBuffers.data());
  renderer._commandBuffers = {};

  // TODO not everything is properly freed
}

int main() 
{
    try{
      // Create vulkan instance with extensions from display api's and with external memory for compositing
      std::vector<std::string> intanceExtensions = {
        VK_KHR_EXTERNAL_MEMORY_CAPABILITIES_EXTENSION_NAME,
        VK_KHR_EXTERNAL_SEMAPHORE_CAPABILITIES_EXTENSION_NAME
      };
      wm.init(800,600);
      wm.getRequiredInstanceExtensions(intanceExtensions);
      
      // Create renderer with extension
      renderer.initInstance(intanceExtensions);

      // Create surface to render to and initialize a device compatable with that surface
      surface = wm.createSurface(renderer._instance._instance);
      renderer.initDevice(surface);

      // compile and load shaders
      compileShader("shaders/shader.vert", "shaders/vert.spv");
      compileShader("shaders/shader.frag", "shaders/frag.spv");
      vertShader.init(renderer._device, "shaders/vert.spv", VkShaderStageFlagBits::VK_SHADER_STAGE_VERTEX_BIT);
      fragShader.init(renderer._device, "shaders/frag.spv", VkShaderStageFlagBits::VK_SHADER_STAGE_FRAGMENT_BIT);

      // Create main descriptor set pools
      sceneDescSet.init(renderer._device, maxSwapchainImgCount, 1);
      meshDescSet.init(renderer._device, maxSwapchainImgCount, meshCount);

      // Create scene buffers and pool
      scene.createUniformBuffer(renderer._device, maxSwapchainImgCount);
      scene.createDescriptorSet(renderer._device, sceneDescSet._descriptorPool,  sceneDescSet._descriptorSetLayout, maxSwapchainImgCount);

      // Position camera start pose and handle camera movement
      FreeCameraInput cameraInput = FreeCameraInput(cam, wm);
      cam.position.z = 3;
      cam.position.y = 0.5f;

      // Create descriptor set per mesh
      jlog("creating meshes");
      meshes.resize(meshCount);
      for(auto &m : meshes){
          m.init(renderer._device);
          m.createUniformBuffer(renderer._device, maxSwapchainImgCount);
          m.createDescriptorSet(renderer._device, meshDescSet._descriptorPool,  meshDescSet._descriptorSetLayout, maxSwapchainImgCount);
      }
      int pos = 0;
      for(auto &mesh : meshes){
        mesh.position.x = pos;
        mesh.position.y = pos/100.0;
        mesh.position.z = -pos;
        pos+=1;
      }
      jlog("creating meshes done");

      // Create syncing objects to avoid drawing too quickly
      renderer._device.createSyncObjects();
      
      createSwapchain();
      jlog("Bootup success");

      while(!wm.shouldClose()){
          try{
            wm.update();
            cameraInput.update();

            renderer.getNextImage(swapchain);
            renderer.drawFrame(swapchain,cam, scene, meshes);
            renderer.presentFrame(swapchain);

          }catch (const std::exception& e) {
            // TODO only do this on the proper exception
            cleanup();
            createSwapchain();
          }
      }
    }catch (const char* e) {
      jlog("Native code threw an exception:");
      std::cerr << e << std::endl;
      throw;
    }catch (const std::string& e) {
      jlog("Native code threw an exception:");
      std::cerr << e << std::endl;
      throw;
    }catch (const std::exception& e) {
      jlog("Native code threw an exception:");
      std::cerr << e.what() << std::endl;
      throw;
    }catch (...) {
      jlog("Unknown native exception occured");
      throw;
    }
    
    return 0;
}