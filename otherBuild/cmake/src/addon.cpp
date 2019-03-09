#include "../node_modules/node-addon-api/napi.h"

#include <iostream>
#include <string>
#include <fstream>

void init(const Napi::CallbackInfo& info) {
  std::cout << "HELLO WORLD!" << std::endl;
}

Napi::Object Init(Napi::Env env, Napi::Object exports) {
  exports.Set(Napi::String::New(env, "init"), Napi::Function::New(env, init));
  return exports;
}

NODE_API_MODULE(app, Init)