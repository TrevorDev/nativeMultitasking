#include <napi.h>

#define VK_USE_PLATFORM_WIN32_KHR
#include "j.h"
#include "windowManager.cpp"
#include "vulkanRenderer.cpp"

void run(const Napi::CallbackInfo& info) {
  jlog("Started!");
  try{
    WindowManager wm = WindowManager(800,600);
    VulkanRenderer renderer;
    renderer.createInstance();
    auto surface = wm.createSurface(renderer._instance);

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