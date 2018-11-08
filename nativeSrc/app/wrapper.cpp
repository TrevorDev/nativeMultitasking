#include <napi.h>

#include <iostream>
#include <string>
#include "vulkan.cpp"

HelloTriangleApplication app;


Napi::Number InitVulkan(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  auto str = info[0].As<Napi::String>();
  unsigned long handle = 0;
  try {
      app.initWindow();
      auto x = app.initVulkan();
      //app.createTextureImageExt(x);
      app.initVulkan2();
      handle = (unsigned long)x;

  } catch (const std::exception& e) {
      std::cerr << e.what() << std::endl;
  }

  // const int n = snprintf(NULL, 0, "%lu", handle);
  // assert(n > 0);
  // char buf[50];//n+1];
  // int c = snprintf(buf, n+1, "%lu", handle);
  // assert(buf[n] == '\0');
  // assert(c == n);

  return Napi::Number::New(env, handle);
}

void InitVulkan2(const Napi::CallbackInfo& info) {
  try {
    Napi::Env env = info.Env();
    auto number = info[0].As<Napi::Number>();
    HANDLE handle = (HANDLE)number.Uint32Value();
    //std::cerr << handle << std::endl;
    app.createTextureImageExt(handle);
  } catch (const std::exception& e) {
      std::cerr << e.what() << std::endl;
  }
}

void Render(const Napi::CallbackInfo& info) {
  auto str = info[0].As<Napi::String>();
  try {
      app.mainLoop();
      app.cleanup();
  } catch (const std::exception& e) {
      std::cerr << e.what() << std::endl;
  }
}

Napi::Object Init(Napi::Env env, Napi::Object exports) {
  exports.Set(Napi::String::New(env, "init"), Napi::Function::New(env, InitVulkan));
  exports.Set(Napi::String::New(env, "init2"), Napi::Function::New(env, InitVulkan2));
  exports.Set(Napi::String::New(env, "render"), Napi::Function::New(env, Render));
  return exports;
}

NODE_API_MODULE(app, Init)