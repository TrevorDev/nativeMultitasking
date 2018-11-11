{
  "targets": [
    {
      "target_name": "app",
      "sources": [ "app/wrapper.cpp" ],
      'include_dirs': [
        "<!@(node -p \"require('node-addon-api').include\")",
        "external/glfw/include",
        "external",
        "external/stb",
        "$(VULKAN_SDK)/Include",
      ],
      'dependencies': ["<!(node -p \"require('node-addon-api').gyp\")"],
      "libraries": [
        "$(VULKAN_SDK)/Lib/vulkan-1.lib",
        "../../nativeSrc/external/glfw/lib-vc2015/glfw3dll.lib" # to link statically and not need to put .dll in root (use glfw3.lib but also need other libs) see http://www.glfw.org/docs/latest/build.html#build_link_win32
      ],
      # START: node-addon-api config --------------------------------------------
      # 'defines': [ 'NAPI_CPP_EXCEPTIONS' ],
      'cflags!': [ '-fno-exceptions' ],
      'cflags_cc!': [ '-fno-exceptions', "-std=c++17" ],
      'xcode_settings': {
        'GCC_ENABLE_CPP_EXCEPTIONS': 'YES',
        'CLANG_CXX_LIBRARY': 'libc++',
        'MACOSX_DEPLOYMENT_TARGET': '10.7',
      },
      'msvs_settings': {
        'VCCLCompilerTool': { 'ExceptionHandling': 1, 'AdditionalOptions': ['/std:c++latest'] },
      },
      # END: node-addon-api config --------------------------------------------
    }
  ]
}