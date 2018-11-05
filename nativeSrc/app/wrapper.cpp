#include <napi.h>
#include "lib.cpp"

void InitVulkan(const Napi::CallbackInfo& info) {
  auto str = info[0].As<Napi::String>();
  initVulkan(str);
}

void Render(const Napi::CallbackInfo& info) {
  auto str = info[0].As<Napi::String>();
  render(str);
}

Napi::Object Init(Napi::Env env, Napi::Object exports) {
  exports.Set(Napi::String::New(env, "init"), Napi::Function::New(env, InitVulkan));
  exports.Set(Napi::String::New(env, "render"), Napi::Function::New(env, Render));
  return exports;
}

NODE_API_MODULE(app, Init)