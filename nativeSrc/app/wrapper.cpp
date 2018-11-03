#include <napi.h>
#include "lib.cpp"

void Method(const Napi::CallbackInfo& info) {
  auto str = info[0].As<Napi::String>();
  main(str);
}

Napi::Object Init(Napi::Env env, Napi::Object exports) {
  exports.Set(Napi::String::New(env, "run"), Napi::Function::New(env, Method));
  return exports;
}

NODE_API_MODULE(app, Init)