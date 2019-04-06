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
#include "src/engine/postProcessRenderSetup.hpp"
#include "src/engine/image.hpp"
#include "src/object3d/mesh.hpp"
#include "src/object3d/scene.hpp"
#include "src/object3d/camera.hpp"
#include "src/object3d/pointLight.hpp"
#include "src/object3d/freeCameraInput.hpp"




void createSwapchain(WindowManager& wm, Renderer& renderer, Swapchain& swapchain, SceneRenderInstance& sceneRenderInstance, vk::SurfaceKHR& surface, uint32_t maxSwapchainImgCount, SceneRenderSetup& sceneRenderSetup, PostProcessRenderInstance& postProcessRenderInstance, PostProcessRenderSetup& postProcessRenderSetup){
  jlog("Creating swapchain");
  // Recreate objects to ensure internals get reset eg. vectors
  // Todo call cleanup functions on these
  swapchain = Swapchain();
  sceneRenderInstance = SceneRenderInstance();

  // create swapchain and renderpass with color + depth
  swapchain.init(surface, wm.getFramebufferSize().width, wm.getFramebufferSize().height, renderer._device, maxSwapchainImgCount);
  //sceneRenderInstance.init(&renderer._device, &sceneRenderSetup, swapchain._swapChainImages, swapchain._swapChainImageFormat, swapchain._depthImage, swapchain._swapChainExtent);
  
  // set camera projection to match output
  sceneRenderSetup.cam.projectionWidth = swapchain._swapChainExtent.width;
  sceneRenderSetup.cam.projectionHeight = swapchain._swapChainExtent.height;

  postProcessRenderInstance = PostProcessRenderInstance();
  postProcessRenderInstance.init(&renderer._device, &postProcessRenderSetup, swapchain._swapChainImages, swapchain._swapChainImageFormat, swapchain._depthImage, swapchain._swapChainExtent);
}

void cleanup(WindowManager& wm, Renderer& renderer, Swapchain& swapchain, SceneRenderInstance& sceneRenderInstance){
  // Cleanup start
  wm.waitForFrameBuffer();
  vkDeviceWaitIdle(renderer._device._device);

  // Reset swapchain
  swapchain.cleanup(renderer._device);
  renderer._device._device.freeCommandBuffers(renderer._device._commandPool, static_cast<uint32_t>(sceneRenderInstance._commandBuffers.size()), sceneRenderInstance._commandBuffers.data());
  sceneRenderInstance._commandBuffers = {};

  // TODO not everything is properly freed
}

int main() 
{
    //jwaitForEnter();
    uint32_t maxSwapchainImgCount = 2;
    Renderer renderer;
    vk::SurfaceKHR surface;
    WindowManager wm;

    Swapchain swapchain;

    SceneRenderSetup sceneRenderSetup;
    SceneRenderInstance sceneRenderInstance;
    
    PostProcessRenderSetup postProcessRenderSetup;
    PostProcessRenderInstance postProcessRenderInstance;

    try{
      // Create vulkan instance with extensions from display api's and with external memory for compositing
      std::vector<std::string> intanceExtensions = {
        VK_KHR_EXTERNAL_MEMORY_CAPABILITIES_EXTENSION_NAME,
        VK_KHR_EXTERNAL_SEMAPHORE_CAPABILITIES_EXTENSION_NAME,
        VK_EXT_DEBUG_UTILS_EXTENSION_NAME
      };
      wm.init(800,600);
      wm.getRequiredInstanceExtensions(intanceExtensions);
      
      // Create renderer with extension
      renderer.initInstance(intanceExtensions);

      // Create surface to render to and initialize a device compatable with that surface
      surface = wm.createSurface(renderer._instance._instance);
      renderer.initDevice(surface);



      loadedImg._device = renderer._device;
      loadedImg.loadImageFromFile(&renderer._device);

      VkSamplerCreateInfo samplerInfo = {};
      samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
      samplerInfo.magFilter = VK_FILTER_LINEAR;
      samplerInfo.minFilter = VK_FILTER_LINEAR;
      samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
      samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
      samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
      samplerInfo.anisotropyEnable = VK_TRUE;
      samplerInfo.maxAnisotropy = 16;
      samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
      samplerInfo.unnormalizedCoordinates = VK_FALSE;
      samplerInfo.compareEnable = VK_FALSE;
      samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
      samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;

      if (vkCreateSampler(renderer._device._device, &samplerInfo, nullptr, &textureSampler) != VK_SUCCESS) {
          throw std::runtime_error("failed to create texture sampler!");
      }
      jlog("TEXTURE LOADED");

      // loadedImg.createImage(_swapChainExtent.width, _swapChainExtent.height, depthFormat, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
      
      // loadedImg.transitionImageLayout(depthFormat, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);

      sceneRenderSetup.init(&renderer._device, maxSwapchainImgCount);
      postProcessRenderSetup.init(&renderer._device, maxSwapchainImgCount);

      // Position camera start pose and handle camera movement
      FreeCameraInput cameraInput = FreeCameraInput(sceneRenderSetup.cam, wm);
      sceneRenderSetup.cam.position.z = 3;
      sceneRenderSetup.cam.position.y = 0.5f;

      // Create syncing objects to avoid drawing too quickly
      renderer._device.createSyncObjects();
      
      createSwapchain(wm, renderer, swapchain, sceneRenderInstance, surface, maxSwapchainImgCount, sceneRenderSetup, postProcessRenderInstance, postProcessRenderSetup);
      jlog("Bootup success");

      while(!wm.shouldClose()){
          try{
            wm.update();
            cameraInput.update();

            swapchain.getNextImage(renderer._device);
            postProcessRenderInstance.render(swapchain._currentImageIndex, swapchain._currentFrame);
            swapchain.presentFrame(renderer._device, postProcessRenderInstance.signalSemaphores);

          }catch (const std::exception& e) {
            // TODO only do this on the proper exception
            cleanup(wm, renderer, swapchain, sceneRenderInstance);
            createSwapchain(wm, renderer, swapchain, sceneRenderInstance, surface, maxSwapchainImgCount, sceneRenderSetup, postProcessRenderInstance, postProcessRenderSetup);
          }
      }
      wm.dispose();
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
    jlog("Closing");
    
    return 0;
}