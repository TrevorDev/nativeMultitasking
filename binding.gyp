{
  "targets": [
    {
      'defines': [ 'NAPI_CPP_EXCEPTIONS' ],
      "target_name": "appWindow",
      "sources": [ "customAddons/appWindow/appWindow.cc" ],
      "include_dirs": [
        "include",
        "./externalLibs/glfw-3.2.1.bin.WIN64/glfw-3.2.1.bin.WIN64/include",
        "<!@(node -p \"require('node-addon-api').include\")",
        "D:/Programs/vulkan/1.1.85.0/Include"
      ],
      'dependencies': ["<!(node -p \"require('node-addon-api').gyp\")"],
      "libraries": [
        "../externalLibs/glfw-3.2.1.bin.WIN64/glfw-3.2.1.bin.WIN64/lib-vc2015/glfw3dll.lib",
        "D:/Programs/vulkan/1.1.85.0/Lib/vulkan-1.lib"
      ]
    }
  ]
}