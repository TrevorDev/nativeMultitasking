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

Swapchain swapchain;

SceneRenderSetup sceneRenderSetup;
SceneRenderInstance sceneRenderInstance;

uint32_t maxSwapchainImgCount = 2;

void createSwapchain(){
  jlog("Creating swapchain");
  swapchain = Swapchain();
  

  // create swapchain and renderpass with color + depth
  swapchain.init(surface, wm.getFramebufferSize().width, wm.getFramebufferSize().height, renderer._device, maxSwapchainImgCount);
  sceneRenderInstance.init(&renderer._device, &sceneRenderSetup, swapchain._swapChainImages, swapchain._swapChainImageFormat, swapchain._depthImage, swapchain._swapChainExtent);
  
  // set camera projection to match output
  sceneRenderSetup.cam.projectionWidth = swapchain._swapChainExtent.width;
  sceneRenderSetup.cam.projectionHeight = swapchain._swapChainExtent.height;
}

void cleanup(){
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

      sceneRenderSetup.init(&renderer._device);

      // Position camera start pose and handle camera movement
      FreeCameraInput cameraInput = FreeCameraInput(sceneRenderSetup.cam, wm);
      sceneRenderSetup.cam.position.z = 3;
      sceneRenderSetup.cam.position.y = 0.5f;

      // Create syncing objects to avoid drawing too quickly
      renderer._device.createSyncObjects();
      
      createSwapchain();
      jlog("Bootup success");

      while(!wm.shouldClose()){
          try{
            wm.update();
            cameraInput.update();

            renderer.getNextImage(swapchain);
            sceneRenderInstance.render(swapchain._currentImageIndex, swapchain._currentFrame);
            renderer.presentFrame(swapchain, sceneRenderInstance.signalSemaphores);

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