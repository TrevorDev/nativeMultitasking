{
  "targets": [
    {
      "target_name": "app",
      "sources": [ "app/wrapper.cpp" ],
      'include_dirs': ["<!@(node -p \"require('node-addon-api').include\")"],
      'dependencies': ["<!(node -p \"require('node-addon-api').gyp\")"],
      "libraries": [
      ],
      # START: node-addon-api config --------------------------------------------
      # 'defines': [ 'NAPI_CPP_EXCEPTIONS' ],
      'cflags!': [ '-fno-exceptions' ],
      'cflags_cc!': [ '-fno-exceptions' ],
      'xcode_settings': {
        'GCC_ENABLE_CPP_EXCEPTIONS': 'YES',
        'CLANG_CXX_LIBRARY': 'libc++',
        'MACOSX_DEPLOYMENT_TARGET': '10.7',
      },
      'msvs_settings': {
        'VCCLCompilerTool': { 'ExceptionHandling': 1 },
      },
      # END: node-addon-api config --------------------------------------------
    }
  ]
}